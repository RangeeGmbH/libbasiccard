/***************************************************************************
                          basiccard.cpp  -  description
                             -------------------
    begin                : Sat Mar 27 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/

#include "winport.h"
#include "basiccard.h"
#include "desaescrypt.h"
#include "eaxcrypt.h"
#include "omaccrypt.h"
#include "bccommandapdu.h"
#include "bcresponseapdu.h"
#include "myrandom.h"
#include <stdlib.h>
#include <time.h>
#include <cstring>

BasicCard::BasicCard(AbstractReader *r) : AbstractCard(r) {
  crypt = NULL;
  algo = 0;
  seedmyrandom(time(NULL));
}

BasicCard::~BasicCard(){
  if (crypt!=NULL)
    delete crypt;
}

ZCCRIRET BasicCard::setKey(BYTE key, BYTE *keydata, BYTE keylen){
  memcpy(&(this->keydata[key][0]), keydata, (keylen>32?32:keylen));
  keylens[key] = keylen;
  return ZCBCI_NOERROR;
}

ZCCRIRET BasicCard::getKey(BYTE key, BYTE *keydata, BYTE *pkeylen){
  if (*pkeylen >= keylens[key]) {
    memcpy(keydata, &(this->keydata[key][0]), keylens[key]);
    *pkeylen = keylens[key];
    return ZCBCI_NOERROR;
  } else {
    *pkeylen = keylens[key];
    return ZCBCI_ERROR_OVERFLOW;
  }
}

ZCCRIRET BasicCard::startEncryption(BYTE algo, BYTE key, WORD keyCID){
  CommandAPDU *c = new CommandAPDU(0xc0, 0x10, 0, 0, 0, NULL, 0, true);
  ResponseAPDU *r = new ResponseAPDU;
  long i;
  BYTE RA[8];
  BYTE RB[10];
  bool isProfessional;
  int RAoffset;

  i = myrandom();
  memcpy(RA, &i, sizeof(i));
  if (sizeof(i) == 4) {
    i = myrandom();
    memcpy(RA+4, &i, sizeof(i));
  }

  if (keyCID != 0xffff) { // MultiApplication Card
   c->setP1(keyCID >> 8);
   c->setP2(keyCID & 0xff);
   isProfessional = true;
   RAoffset = 1;
  } else { // normal card
   c->setP1(algo);
   c->setP2(key);
   isProfessional = false;
   RAoffset = 0;
  } 
  
  switch (algo) {
    case ZCBCI_DEFAULT_ENC:
    case ZCBCI_3DES:
    case ZCBCI_DES:
      c->setLc(RAoffset+4);
      c->setData(RA, RAoffset, 4);
      isProfessional = false;
      break;
    case ZCBCI_3DES_CRC:
    case ZCBCI_DES_CRC:
      c->setLc(RAoffset+4);
      c->setData(RA, RAoffset, 4);
      isProfessional = true;
      break;
    case ZCBCI_AES:
    case CHZBCI_AES192: // AES_192
    case CHZBCI_AES256: // AES_256
    case CHZBCI_EAX_AES128: // EAX_AES128
    case CHZBCI_EAX_AES192: // EAX_AES192
    case CHZBCI_EAX_AES256: // EAX_AES256
    case CHZBCI_OMAC_AES128: // OMAC_AES128
    case CHZBCI_OMAC_AES192: // OMAC_AES192
    case CHZBCI_OMAC_AES256: // OMAC_AES256    
      c->setLc(RAoffset+8);
      c->setData(RA, RAoffset, 8);
      isProfessional = true;
      break;  
    case ZCBCI_SG_LFSR:
    case ZCBCI_SG_LFSR_CRC:
      delete c;
      delete r;
      return ZCBCI_ERROR_MISC; // Compact Basic Card not supported
      break;
  }
  
  if (keyCID != 0xffff) {
   c->setData(&algo, 0, 1);
  }
  
  c->setLe(isProfessional?0:4);

  transaction(c, r);
  if ((algo == ZCBCI_DEFAULT_ENC) && ((getSW1SW2() == swLcLeError) || (getSW1SW2() == 0x69ce))) { 
    // Professional / MultiApp BasicCard which needs 8 bytes RA
    c->setLc(RAoffset+8);
    c->setData(RA, RAoffset, 8);
    if (keyCID != 0xffff) {
     c->setData(&algo, 0, 1);
    }
    c->setLe(0);
    transaction(c, r);
  }

  delete c;    

  if (isNoError()) {
    r->getData(RB);

    delete r;
    
    if (isProfessional || ((getSW1SW2()&0xff00)==0x6100)) {
      algo = RB[0];
      memmove(&RB[0], &RB[1], sizeof(RB)-1);
    } else if (algo == ZCBCI_DEFAULT_ENC) { // automatic selection for Enhanced BasicCard
      if (keylens[key]>=16) {
        algo = ZCBCI_3DES;
      } else {
        algo = ZCBCI_DES;
      }
    }
    
    if ((algo >= CHZBCI_EAX_AES128) && (algo <= CHZBCI_EAX_AES256)) {
      crypt = new EAXCrypt(algo, &keydata[key][0], RA, RB);
    } else if ((algo >= CHZBCI_OMAC_AES128) && (algo <= CHZBCI_OMAC_AES256)) {  
      crypt = new OMACCrypt(algo, &keydata[key][0]);
    } else {
      crypt = new DESAESCrypt(algo, &keydata[key][0], RA, RB);
    }
    this->algo = algo;
    return ZCBCI_NOERROR;
  } else {
    delete r;
    return ZCBCI_ERROR_TRANS_FAILED;
  }
}

ZCCRIRET BasicCard::endEncryption() {
  ZCCRIRET rv;
  CommandAPDU *c = new CommandAPDU(0xc0, 0x12, 0, 0, 0, NULL, 0, false);
  ResponseAPDU *r = new ResponseAPDU;

  rv = transaction(c, r);
  delete c;
  delete r;

  if ((rv==ZCBCI_NOERROR)&&(crypt!=NULL)) {
    delete crypt;
    crypt = NULL;
    algo = 0;
  }

  return rv;
}

ZCCRIRET BasicCard::transaction(CommandAPDU *cmd, ResponseAPDU *res) {
  ZCCRIRET rv;
  
  if (crypt!=NULL) {
    if (!crypt->encryptCommand(cmd)) {
      return ZCBCI_ERROR_MISC;
    }
  }

  rv = AbstractCard::transaction(cmd, res);

  if (crypt!=NULL) {
    if (!crypt->decryptResponse(res)) {
      res->setSW1SW2(swBadDesResponse);
      SW1SW2 = swBadDesResponse;
      return ZCBCI_ERROR_TRANS_FAILED;
    }
  }

  return rv;
}

ZCCRIRET BasicCard::getState(BYTE *state, BYTE *version){
  ZCCRIRET rv;
  CommandAPDU *c = new CommandAPDU(0xc0, 0x00, 0, 0, 0, NULL, 0, true);
  BCResponseAPDU *r = new BCResponseAPDU;

  rv = transaction(c, r);

  if (isNoError()) {
    r->extractByte(state);
    if (version!=NULL) {
      r->extractString((char *)version);
    }
  }

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::eepromSize(WORD *pStart, WORD *pLength) {
  ZCCRIRET rv;
  CommandAPDU *c = new CommandAPDU(0xc0, 0x02, 0, 0, 0, NULL, 4, true);
  BCResponseAPDU *r = new BCResponseAPDU;

  rv = transaction(c, r);

  if (isNoError()) {
    r->extractWord(pStart);
    r->extractWord(pLength);
  }

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::clearEeprom(WORD start, WORD length) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x04, 0, 0);
  ResponseAPDU *r = new ResponseAPDU;

  if ((start) || (length)) {
    c->setP1(start>>8);
    c->setP2(start&0xff);
    c->appendWord(length);
  }
  
  rv = transaction(c, r);

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::writeEeprom(WORD start, BYTE *pData, BYTE cbData) {            
  ZCCRIRET rv;
  CommandAPDU *c = new CommandAPDU(0xc0, 0x06, 0, 0);
  ResponseAPDU *r = new ResponseAPDU;

  c->setP1(start>>8);
  c->setP2(start&0xff);
  c->setLc(cbData);
  c->setData(pData);
  rv = transaction(c, r);

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::readEeprom(WORD start, BYTE *pData, BYTE cbData) {
  ZCCRIRET rv;
  CommandAPDU *c = new CommandAPDU(0xc0, 0x08, 0, 0);
  ResponseAPDU *r = new ResponseAPDU;

  c->setP1(start>>8);
  c->setP2(start&0xff);
  c->setLe(cbData);
  c->setLeEnabled(true);
  rv = transaction(c, r);

  if (isNoError()) {
    r->getData(pData);
  }

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::eepromCRC(WORD start, WORD length, WORD *pCRC) {  
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x0a, 0, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setP1(start>>8);
  c->setP2(start&0xff);
  c->appendWord(length);
  c->setLe(2);
  c->setLeEnabled(true);
  
  rv = transaction(c, r);

  r->extractWord(pCRC);

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::setState(BYTE state) {
  ZCCRIRET rv;
  CommandAPDU *c = new CommandAPDU(0xc0, 0x0c, 0, 0);
  ResponseAPDU *r = new ResponseAPDU;

  c->setP1(state);
  rv = transaction(c, r);

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::getApplicationID(BYTE index, BYTE type, CHAR *pszID, BYTE cbID) {
  ZCCRIRET rv;
  CommandAPDU *c = new CommandAPDU(0xc0, 0x0e, index, type);
  ResponseAPDU *r = new ResponseAPDU;

  c->setLe(0);
  c->setLeEnabled(true);
  rv = transaction(c, r);

  if (isNoError()) {
    if (r->getData(NULL)<cbID) {
      r->getData((BYTE *)pszID);
      pszID[r->getData(NULL)] = 0;
    } else {
      rv = ZCBCI_ERROR_OVERFLOW;
    }
  }
  
  delete r;
  delete c;
  return rv;
}  

ZCCRIRET BasicCard::echo(BYTE inc, CHAR *pszEcho) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x14, inc, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendString(pszEcho);
  c->setLe(c->getLc());
  c->setLeEnabled(true);
  
  rv = transaction(c, r);

  r->extractString(pszEcho);
  return rv;
}

// MultiApplication functions

ZCCRIRET BasicCard::findComponent(BYTE type, const CHAR *pszPath, WORD *pCID) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xae, type, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendString(pszPath);
  c->setLe(2);
  c->setLeEnabled(true);
  
  rv = transaction(c, r);

  r->extractWord(pCID);

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::selectApplication(const CHAR *pszPath) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xa0, 0, 0);
  BCResponseAPDU *r = new BCResponseAPDU;
  BYTE fError;
  
  c->appendString(pszPath);
  
  rv = transaction(c, r);
  
  // workaround for a bug in some BasicCards where in some situations a file error is returned
  if (r->getSW1SW2() == 0x6101) {
    r->extractByte(&fError);
    if (fError == 0x0e) { // feAccessDenied
      SW1SW2 = 0x69c2; // swAccessDenied
      rv =  ZCBCI_ERROR_TRANS_FAILED;
    }
  }  

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::externalAuthenticate(BYTE algo, BYTE key, WORD keyCID) {
  ZCCRIRET rv;
  BYTE challenge[16];
  
  
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x40, 0, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLe(0);
  c->setLeEnabled(true);
  
  rv = transaction(c, r);

  r->extractStringN(challenge, sizeof(challenge));
  
  delete r;
  delete c;
  if (!isNoError()) {
    return rv;
  }
  
  DESAESCrypt *temp = new DESAESCrypt(algo, &keydata[key][0], challenge, challenge); // RA, RB not needed: just dummies
  temp->encryptBlock(challenge);
  delete temp;
  
  c = new BCCommandAPDU(0xc0, 0x42, keyCID >> 8, keyCID & 0xff);
  r = new BCResponseAPDU;
  c->appendByte(algo);
  c->appendStringN(challenge, (algo >= ZCBCI_AES? 16:8));
  
  rv = transaction(c, r);
  
  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::verify(WORD keyCID, const CHAR *pszPassword) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x46, keyCID >> 8, keyCID & 0xff);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendString(pszPassword);
  
  rv = transaction(c, r);

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::grantPrivilege(WORD prvCID, const CHAR *pszPath) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xb2, prvCID >> 8, prvCID & 0xff);
  BCResponseAPDU *r = new BCResponseAPDU;

  if (pszPath != NULL) {
    c->appendString(pszPath);
  }
  
  rv = transaction(c, r);

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::internalAuthenticate(BYTE algo, BYTE key, WORD keyCID) {
  ZCCRIRET rv;
  BYTE challenge[16];
  BYTE response[16];
  int i;
  BYTE l, a;
  
  for (i=0; i<16; i++) {
    challenge[i] = myrandom() & 0xff;
  }  
  
  l = (algo >= ZCBCI_AES? 16:8);
  
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x44, keyCID >> 8, keyCID & 0xff);
  BCResponseAPDU *r = new BCResponseAPDU;
  
  c->appendByte(algo);
  c->appendStringN(challenge, l);
  c->setLe(l);
  c->setLeEnabled(true);
  
  rv = transaction(c, r);
  
  DESAESCrypt *temp = new DESAESCrypt(algo, &keydata[key][0], challenge, challenge); // RA, RB not needed: just dummies
  temp->encryptBlock(challenge);
  delete temp;
  
  if (isNoError()) {
    r->extractByte(&a);
    r->extractStringN(response, l);
    if (memcmp(challenge, response, l) != 0) {
      SW1SW2 = 0x66c7; // swBadAuthenticate
      rv = ZCBCI_ERROR_TRANS_FAILED;
    }
  }
  
  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::getFreeMemory(WORD *pTotal, WORD *pBlock) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x48, 0, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLe(4);
  c->setLeEnabled(true);
  
  rv = transaction(c, r);
  
  if (isNoError()) {
    r->extractWord(pTotal);
    r->extractWord(pBlock);
  }

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::componentName(WORD cmpCID, CHAR *pszName, BYTE cbName) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xb0, cmpCID >> 8, cmpCID & 0xff);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLe(0);
  c->setLeEnabled(true);
  
  rv = transaction(c, r);
  
  if (isNoError()) {
    if (r->getRemainingLen() >= cbName) {
      rv = ZCBCI_ERROR_OVERFLOW;
    } else {
      r->extractString(pszName);
    }
  }

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::createComponent(BYTE type, const CHAR *pszPath, const void *pAttr, BYTE cbAttr,
                                    const void *pData, BYTE cbData, WORD *pCID) {
  ZCCRIRET rv;
  
  if (strlen(pszPath) + cbAttr + cbData > 250) {
    return ZCBCI_ERROR_OVERFLOW;
  }
  
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xa2, type, 0);
  BCResponseAPDU *r = new BCResponseAPDU;
  
  BYTE cbPath = strlen(pszPath) & 0xff;
  
  c->appendByte(cbPath);
  c->appendString(pszPath);
  c->appendByte(cbAttr);
  if (cbAttr)
    c->appendStringN((BYTE *)pAttr, cbAttr);
  if (cbData)
    c->appendStringN((BYTE *)pData, cbData);
  
  c->setLe(2);
  c->setLeEnabled(true);
  
  rv = transaction(c, r);
  
  if (isNoError()) {
    if (pCID)
      r->extractWord(pCID);
  }

  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::deleteComponent(WORD cmpCID) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xa4, cmpCID >> 8, cmpCID & 0xff);
  BCResponseAPDU *r = new BCResponseAPDU;

  rv = transaction(c, r);
  
  delete r;
  delete c;
  return rv;
}  

ZCCRIRET BasicCard::writeComponentAttr(WORD cmpCID, const void *pAttr, BYTE cbAttr) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xa6, cmpCID >> 8, cmpCID & 0xff);
  BCResponseAPDU *r = new BCResponseAPDU;
  
  c->appendStringN((BYTE *)pAttr, cbAttr);
  
  rv = transaction(c, r);
  
  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::readComponentAttr(WORD cmpCID, void *pAttr, BYTE *pcbAttr) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xa8, cmpCID >> 8, cmpCID & 0xff);
  BCResponseAPDU *r = new BCResponseAPDU;
  
  c->setLe(0);
  c->setLeEnabled(true);
  
  rv = transaction(c, r);
  
  if (isNoError()) {
    if (r->getRemainingLen() > *pcbAttr) {
      *pcbAttr = r->getRemainingLen();     
      rv = ZCBCI_ERROR_OVERFLOW;
    } else {
      *pcbAttr = r->getRemainingLen();
      r->extractStringN((BYTE *)pAttr, *pcbAttr);
    }
  }  
  
  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::writeComponentData(WORD cmpCID, const void *pData, BYTE cbData) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xaa, cmpCID >> 8, cmpCID & 0xff);
  BCResponseAPDU *r = new BCResponseAPDU;
  
  c->appendStringN((BYTE *)pData, cbData);
  
  rv = transaction(c, r);
  
  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::readComponentData(WORD cmpCID, void *pData, BYTE *pcbData) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xac, cmpCID >> 8, cmpCID & 0xff);
  BCResponseAPDU *r = new BCResponseAPDU;
  
  c->setLe(0);
  c->setLeEnabled(true);
  
  rv = transaction(c, r);
  
  if (isNoError()) {
    if (r->getRemainingLen() > *pcbData) {
      *pcbData = r->getRemainingLen();    
      rv = ZCBCI_ERROR_OVERFLOW;
    } else {
      *pcbData = r->getRemainingLen();
      r->extractStringN((BYTE *)pData, *pcbData);
    }
  }  
  
  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::authenticateFile(WORD keyCID, BYTE algo, const CHAR *pszPath, const void *pSignature) {
  ZCCRIRET rv;
  int signlen;
  
  switch (algo) {
    case CHZBCI_OMAC_AES128:
    case CHZBCI_OMAC_AES192:
    case CHZBCI_OMAC_AES256:
      signlen = 16;
      break;
    case CHZBCI_EC167:
      signlen = 42;
      break;
    default:
      SW1SW2 = 0x6615; // swUnknownAlgorithm
      return ZCBCI_ERROR_TRANS_FAILED;
      break;
  }
  
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xb4, keyCID >> 8, keyCID & 0xff);
  BCResponseAPDU *r = new BCResponseAPDU;
  
  c->appendByte(algo);
  c->appendByte(strlen(pszPath));
  c->appendString(pszPath);
  c->appendStringN((BYTE *)pSignature, signlen);
  
  rv = transaction(c, r);
  
  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::readRightsList(const CHAR *pszPath, WORD *pList, BYTE *pcbList) {
  ZCCRIRET rv;
  BYTE toRead, numRead, totalRead, i;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xb6, 0, 0);
  BCResponseAPDU *r = new BCResponseAPDU;
  
  c->appendString(pszPath);
  c->setLeEnabled(true);
  totalRead = numRead = 0;

  do {
    toRead = *pcbList>55?55:*pcbList;
    c->setP1(totalRead);
    c->setLe(toRead*2);
    rv = transaction(c, r);
    if (!isNoError())
      break;
    numRead = r->getRemainingLen() / 2;
    numRead = numRead>toRead?toRead:numRead;
    totalRead += numRead;
    *pcbList -= numRead;
    for (i=0;i<numRead;i++) {
      r->extractWord(pList);
      pList++;
    }
  } while (1);
  
  *pcbList = totalRead;
  
  if ((rv = ZCBCI_ERROR_TRANS_FAILED) && (SW1SW2 == 0x6a88) && (totalRead > 0)) { // swDataNotFound
    rv = ZCBCI_NOERROR;
    SW1SW2 = 0x6100 | (totalRead * 2>255?255:totalRead * 2);
  } 
    
  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::loadSequence(BYTE phase) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xb8, phase, 0);
  BCResponseAPDU *r = new BCResponseAPDU;
  
  rv = transaction(c, r);
  
  delete r;
  delete c;
  return rv;
}

ZCCRIRET BasicCard::secureTransport(WORD keyCID, BYTE algo, const void *pNonce, BYTE cbNonce) {
  ZCCRIRET rv;
  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0xba, keyCID >> 8, keyCID & 0xff);
  BCResponseAPDU *r = new BCResponseAPDU;
  
  if (algo) {
    c->appendByte(algo);
    c->appendStringN((BYTE *)pNonce, cbNonce);
  }
  
  rv = transaction(c, r);
  
  delete r;
  delete c;
  return rv;
}

// file I/O functions

#define IsFileErr(x) (fileErr!=0)?ZCBCI_ERROR_TRANS_FAILED:(x)

ZCCRIRET BasicCard::mkDir(const CHAR *pszPath) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x80, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendString(pszPath);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  return IsFileErr(rv);
}

ZCCRIRET BasicCard::rmDir(const CHAR *pszPath) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x81, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendString(pszPath);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  return IsFileErr(rv);
}

ZCCRIRET BasicCard::chDir(const CHAR *pszPath) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x82, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendString(pszPath);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  return IsFileErr(rv);;
}

ZCCRIRET BasicCard::curDir(CHAR *pszPath, BYTE cbPath) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x83, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  if (r->getRemainingLen() < cbPath) {
    r->extractString(pszPath);
    return IsFileErr(rv);;
  } else {
    return ZCBCI_ERROR_OVERFLOW;
  }
}

ZCCRIRET BasicCard::rename(const CHAR *pszOldPath, const CHAR *pszNewPath) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x87, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendByte(strlen(pszOldPath) & 0xff);
  c->appendString(pszOldPath);
  c->appendString(pszNewPath);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  return IsFileErr(rv);;
}

ZCCRIRET BasicCard::dirCount(const CHAR *pszFileSpec, WORD *pCount) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x84, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendString(pszFileSpec);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractWord(pCount);
  return IsFileErr(rv);;
}

ZCCRIRET BasicCard::dirFile(const CHAR *pszFileSpec, WORD num, CHAR *pszFile, BYTE cbFile) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x85, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendWord(num);
  c->appendString(pszFileSpec);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  
  if (r->getRemainingLen() < cbFile) {
    r->extractString(pszFile);
    return IsFileErr(rv);
  } else {
    return ZCBCI_ERROR_OVERFLOW;
  }
}

ZCCRIRET BasicCard::getAttr(const CHAR *pszPath, WORD *pAttrib) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0xa9, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendString(pszPath);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractWord(pAttrib);
  return IsFileErr(rv);
}

ZCCRIRET BasicCard::eraseFile(const CHAR *pszPath) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x86, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendString(pszPath);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  return IsFileErr(rv);
}

ZCCRIRET BasicCard::openFile(const CHAR *pszPath, BYTE mode, WORD recordlen, WORD *pFileNum) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x89, mode);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendWord(recordlen);
  c->appendString(pszPath);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractWord(pFileNum);
  
  return IsFileErr(rv);
}

ZCCRIRET BasicCard::closeFile(WORD FileNum) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x8a, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}

ZCCRIRET BasicCard::closeAllFiles() {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x8b, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}

ZCCRIRET BasicCard::fileLength(WORD FileNum, DWORD *pFileLength) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x8d, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractLong((LONG *)pFileLength);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::getFilepos(WORD FileNum, DWORD *pFilePos) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x8e, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractLong((LONG *)pFilePos);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::setFilepos(WORD FileNum, DWORD FilePos) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x8f, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendLong(FilePos);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::queryEof(WORD FileNum, BOOL *pfEof) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x90, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;
  WORD eof;
  
  c->setLc(0);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractWord(&eof);

  *pfEof = (eof == 0xffff);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::fileGet(WORD FileNum, BYTE *pData, BYTE cbData) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x91, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(cbData);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractStringN(pData, cbData);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::fileGetPos(WORD FileNum, DWORD pos, BYTE *pData, BYTE cbData) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x92, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendLong(pos);
  c->setLe(cbData);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractStringN(pData, cbData);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::filePut(WORD FileNum, const BYTE *pData, BYTE cbData) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x93, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendStringN(pData, cbData);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::filePutPos(WORD FileNum, DWORD pos, const BYTE *pData, BYTE cbData) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x94, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendLong(pos);
  c->appendStringN(pData, cbData);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::fileRead(WORD FileNum, BYTE *pData, BYTE cbData) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x97, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(cbData);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractStringN(pData, cbData);

  return IsFileErr(rv);
}

ZCCRIRET BasicCard::fileReadLong(WORD FileNum, LONG *plVar) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x98, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractLong(plVar);

  return IsFileErr(rv);
}

ZCCRIRET BasicCard::fileReadSingle(WORD FileNum, float *pflVar) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x99, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractFloat(pflVar);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::fileReadString(WORD FileNum, CHAR *pszVar, BYTE cbVar) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x9a, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  
  if (r->getRemainingLen() < cbVar) {
    r->extractString(pszVar);
    return IsFileErr(rv);
  } else {
    return ZCBCI_ERROR_OVERFLOW;
  }
}
  
ZCCRIRET BasicCard::fileReadUser(WORD FileNum, BYTE *pData, BYTE *pcbData) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x9a, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  if (r->getRemainingLen() <= *pcbData) {
    *pcbData = r->getRemainingLen();
    r->extractStringN(pData, *pcbData);
    return IsFileErr(rv);
  } else {
    return ZCBCI_ERROR_OVERFLOW;
  }
}  

ZCCRIRET BasicCard::fileReadUser2(WORD FileNum, BYTE *pData, BYTE cbData) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x9b, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(cbData);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  r->extractStringN(pData, cbData);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::fileReadLine(WORD FileNum, CHAR *pszLine, BYTE cbLine) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0x9c, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->setLc(0);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  if (r->getRemainingLen() < cbLine) {
    r->extractString(pszLine);
    return IsFileErr(rv);;
  } else {
    return ZCBCI_ERROR_OVERFLOW;
  }
}

ZCCRIRET BasicCard::fileWriteLong(WORD FileNum, LONG lVal) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0xa2, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendByte(3);
  c->appendLong(lVal);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::fileWriteSingle(WORD FileNum, float flVal) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0xa2, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendByte(4);
  c->appendFloat(flVal);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::fileWriteUser(WORD FileNum, const BYTE *pData, BYTE cbData) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0xa2, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendByte(5);
  c->appendByte(cbData);
  c->appendStringN(pData, cbData);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::fileWriteString(WORD FileNum, const CHAR *pszVal) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0xa2, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendByte(5);
  c->appendByte(strlen(pszVal) & 0xff);
  c->appendString(pszVal);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}

ZCCRIRET BasicCard::fileWrite(WORD FileNum, const BYTE *pData, BYTE cbData) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0xa2, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendStringN(pData, cbData);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}
  
ZCCRIRET BasicCard::filePrint(WORD FileNum, const CHAR *pszVal) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0xa2, FileNum);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendString(pszVal);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}  

ZCCRIRET BasicCard::getLockInfo(const CHAR *pszPath, PZCBCILOCKINFO pInfo) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0xa8, 0);
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendString(pszPath);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);
  r->extractByte(&(pInfo->ReadLock));
  r->extractByte(&(pInfo->WriteLock));
  r->extractByte(&(pInfo->CustomLock));
  r->extractByte(&(pInfo->ReadKey[1]));
  r->extractByte(&(pInfo->ReadKey[2]));
  r->extractByte(&(pInfo->WriteKey[1]));
  r->extractByte(&(pInfo->WriteKey[2]));

  return IsFileErr(rv);
}  

ZCCRIRET BasicCard::setFileLock(const CHAR *pszPath, ZCBCILOCK type, BOOL fLock, BYTE KeyCount, BYTE Key1, BYTE Key2) {
  ZCCRIRET rv;

  BCCommandAPDU *c = new BCCommandAPDU(0xc0, 0x18, 0xa7, type+KeyCount+(fLock?0x00:0x10));
  BCResponseAPDU *r = new BCResponseAPDU;

  c->appendByte(Key1);
  c->appendByte(Key2);
  c->appendString(pszPath);
  c->setLe(0);
  c->setLeEnabled(true);

  rv = transaction(c, r);

  r->extractByte(&fileErr);

  return IsFileErr(rv);
}
