/***************************************************************************
                          zcbci.cpp  -  description
                             -------------------
    begin                : So Mär 28 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/

#include "winport.h"
#include <zccri.h>
#include <zcbci.h>
#include <chzbci.h>
#include <stdio.h>
#include <string.h>
#include <keyparse.h>
#include "abstractreader.h"
#include "basiccard.h"
#include "bccommandapdu.h"
#include "bcresponseapdu.h"

BasicCard *c[8] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

#define Check(x)  (c[card] != NULL)?(x):(ZCBCI_ERROR_PARM)
#define CheckAndReturn() if (c[card] == NULL){return ZCBCI_ERROR_PARM;}
#define SetSW1SW2() *pSW1SW2 = c[card]->getSW1SW2()
#define SetFileErr() *pFileErr = c[card]->getFileErr()

unsigned int ZCCriIntReaderCount();
AbstractReader *ZCCriIntGetReader(ZCCRIREADER hReader);

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciAttach(ZCCRIREADER reader, PZCBCICARD pCard) {

  if (reader>=ZCCriIntReaderCount())
    return ZCBCI_ERROR_PARM;

  *pCard = reader;
  c[reader] = new BasicCard(ZCCriIntGetReader(reader));
  return ZCBCI_NOERROR;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciCreateHandle(PZCBCICARD pCard) {
  return ZCBCI_ERROR_PARM; //FIXME: not supported
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciAttachHandle(ZCCRIREADER reader, ZCBCICARD card) {
  return ZCBCI_ERROR_PARM; //FIXME: not supported
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciDetach(ZCBCICARD card) {
  if (c[card]!=NULL) {
    delete c[card];
    c[card] = NULL;
    return ZCBCI_NOERROR;
  } else {
    return ZCBCI_ERROR_PARM;
  }
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciSetKey(ZCBCICARD card,
                                        BYTE keynum, BYTE *pKey, BYTE cbKey) {
  return Check(c[card]->setKey(keynum, pKey, cbKey));
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciSetPoly(ZCBCICARD card,
                                         DWORD polynomA, DWORD polynomS) {
  return ZCBCI_ERROR_MISC; // Compact Basic Card not supported
}

void bci_int_keycallback(int card, int keynum, unsigned char* keydata, int keylen) {
  c[card]->setKey(keynum, keydata, keylen);
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciReadKeyFile(ZCBCICARD card,
                                             CHAR *pszFile) {
  int l = 0;
  FILE *f;
  struct bci_int_parm parm;

  f = fopen(pszFile, "r");

  if (f != NULL) {
    parm.cardindex = card;
    parm.callback = bci_int_keycallback;
    l = bci_int_getkey2(f, &parm);
    fclose(f);
  }

  return (l>0)?ZCBCI_NOERROR:ZCBCI_ERROR_INVALID_FILE;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciStartEncryption(ZCBCICARD card, WORD *pSW1SW2,
                                                 BYTE algo, BYTE key, DWORD random) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->startEncryption(algo, key, 0xffff);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciStartProEncryption(ZCBCICARD card, WORD *pSW1SW2,
                                                    BYTE *pAlgo, BYTE key,
                                                    DWORD random, DWORD random2) {
  // random values are generated internally
  ZCBCIRET rv = ZCBciStartEncryption(card, pSW1SW2, *pAlgo, key, random);
  *pAlgo = c[card]->getAlgo();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciEndEncryption(ZCBCICARD card, WORD *pSW1SW2) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->endEncryption();
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetState(ZCBCICARD card, WORD *pSW1SW2,
                                          BYTE *pState, WORD *pVersion) {
  BYTE data[300];
  BYTE hiver, lover;
  CheckAndReturn();

  ZCBCIRET rv = c[card]->getState(pState, data);
  SetSW1SW2();

  if (sscanf((char *)data, "ZC%c.%c", &hiver, &lover)==2) { // Professional BasicCard
    *pVersion = ((hiver-'0')<<8) + lover-'0';
  } else {
    *pVersion = (data[0]<<8) + data[1];
  }

  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetStateVersion(ZCBCICARD card, WORD *pSW1SW2,
                                                 BYTE *pState, WORD *pVersion,
                                                 CHAR *pVersionStr, DWORD *pVersionStrLen) {
  BYTE data[300];
  CheckAndReturn();
  ZCBCIRET rv = c[card]->getState(pState, data);
  SetSW1SW2();
  if (*pVersionStrLen >= strlen((char *)(data+1))) {
    strcpy(pVersionStr, (char *)(data+1));
    *pVersionStrLen = strlen((char *)(data+1));
  } else {
    rv = ZCBCI_ERROR_OVERFLOW;
  }
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciEepromSize(ZCBCICARD card, WORD *pSW1SW2,
                                            WORD *pStart, WORD *pLength) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->eepromSize(pStart, pLength);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciClearEeprom(ZCBCICARD card, WORD *pSW1SW2,
                                             WORD start, WORD length) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->clearEeprom(start, length);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciWriteEeprom(ZCBCICARD card, WORD *pSW1SW2,
                                             WORD start, BYTE *pData, BYTE cbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->writeEeprom(start, pData, cbData);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciReadEeprom(ZCBCICARD card, WORD *pSW1SW2,
                                            WORD start, BYTE *pData, BYTE cbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->readEeprom(start, pData, cbData);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciEepromCRC(ZCBCICARD card, WORD *pSW1SW2,
                                           WORD start, WORD length, WORD *pCRC) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->eepromCRC(start, length, pCRC);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciSetState(ZCBCICARD card, WORD *pSW1SW2,
                                          BYTE state) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->setState(state);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetApplicationID(ZCBCICARD card, WORD *pSW1SW2,
                                                  CHAR *pszID, BYTE cbID) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->getApplicationID(0, 0, pszID, cbID);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciEcho(ZCBCICARD card, WORD *pSW1SW2,
                                      BYTE inc, CHAR *pszEcho) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->echo(inc, pszEcho);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciTransaction(ZCBCICARD card, BYTE Cla, BYTE Ins,
                                             PZCBCIPARM pData, BYTE bDataCount,
                                             WORD *pSW1SW2) {
  return ZCBciTransaction2(card, Cla, Ins, 0, 0, ZCBCI_DEFAULT_LC, ZCBCI_DEFAULT_LE, pData, bDataCount, pSW1SW2);
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciTransaction2(ZCBCICARD card, BYTE Cla, BYTE Ins,
                                              BYTE P1, BYTE P2, WORD Lc, WORD Le,
                                              PZCBCIPARM pData, BYTE bDataCount,
                                              WORD *pSW1SW2) {
  ZCBCIRET rv;
  bool setLeToZero = false;
  BYTE data[300];
  memset(data, 0, sizeof(data));

  CheckAndReturn();
  BCCommandAPDU *cmd = new BCCommandAPDU(Cla, Ins, 0, 0);
  BCResponseAPDU *res = new BCResponseAPDU;
  cmd->setP1(P1);
  cmd->setP2(P2);

  for (int i=0; i<bDataCount; i++) {
    ZCBCIPARM *d =  &(pData[i]);
    switch (d->parmtype) {
      case ZCBCIBYTE:
        cmd->appendByte(d->data.valbyte);
        break;
      case ZCBCIINT:
        cmd->appendWord(d->data.valint);
        break;
      case ZCBCILONG:
        cmd->appendLong(d->data.vallong);
        break;
      case ZCBCISTRING:
        cmd->appendString(d->data.valstring.pString);
        setLeToZero = true;
        break;
      case ZCBCISTRINGN:
        cmd->appendStringN((BYTE *)d->data.valstringn.pString, d->data.valstringn.bLength);
        break;
      case ZCBCIUSER:
        cmd->appendStringN(d->data.valuser.pData, d->data.valuser.bSize);
        break;
      case ZCBCIRAW:
        cmd->appendStringN(d->data.valraw.pData, d->data.valraw.bLen);
        setLeToZero = true;
        break;
      case ZCBCIBINSTRING:
        cmd->appendStringN(d->data.valbinstring.pData, d->data.valbinstring.bLength);
        setLeToZero = true;
        break;
      case ZCBCIBINSTRINGN:
        cmd->appendStringN(d->data.valbinstringn.pData, d->data.valbinstringn.bLength);
        break;
      default:
        break;
    }
  }

  if (Le==ZCBCI_DEFAULT_LE) {
    // set Le to zero if variable length result is expected
    cmd->setLe((setLeToZero)?0:cmd->getLc());
    cmd->setLeEnabled(setLeToZero || (cmd->getLc()!=0));
  } else if (Le==ZCBCI_DISABLE_LE) {
    cmd->setLeEnabled(false);
  } else {
    cmd->setLe(Le);
    cmd->setLeEnabled(true);
  }

  if (Lc!=ZCBCI_DEFAULT_LC) {
    cmd->getData(data);
    cmd->setLc(Lc);
    cmd->setData(data);
  }

  rv = c[card]->transaction(cmd, res);

  for (int i=0; i<bDataCount; i++) {
    ZCBCIPARM *d =  &(pData[i]);
    switch (d->parmtype) {
      case ZCBCIBYTE:
        res->extractByte(&(d->data.valbyte));
        break;
      case ZCBCIINT:
        res->extractWord((WORD *)&(d->data.valint));
        break;
      case ZCBCILONG:
        res->extractLong(&(d->data.vallong));
        break;
      case ZCBCISTRING:
        if (res->getRemainingLen() < d->data.valstring.cbString) {
          res->extractString(d->data.valstring.pString);
        } else {
          rv = ZCBCI_ERROR_OVERFLOW;
        }
        break;
      case ZCBCISTRINGN:
        res->extractStringN((BYTE *)d->data.valstringn.pString, d->data.valstringn.bLength);
        break;
      case ZCBCIUSER:
        res->extractStringN(d->data.valuser.pData, d->data.valuser.bSize);
        break;
      case ZCBCIRAW:
        if (res->getRemainingLen() <= d->data.valraw.bSize) {
          d->data.valraw.bLen = res->getRemainingLen();
          res->extractStringN(d->data.valraw.pData, d->data.valraw.bLen);
        } else {
          rv = ZCBCI_ERROR_OVERFLOW;
        }
        break;
      case ZCBCIBINSTRING:
        if (res->getRemainingLen() <= d->data.valbinstring.bSize) {
          d->data.valbinstring.bLength = res->getRemainingLen();
          res->extractStringN(d->data.valbinstring.pData, d->data.valbinstring.bLength);
        } else {
          rv = ZCBCI_ERROR_OVERFLOW;
        }
        break;
      case ZCBCIBINSTRINGN:
        res->extractStringN(d->data.valbinstringn.pData, d->data.valbinstringn.bLength);
        break;
      default:
        break;
    }
  }

  SetSW1SW2();
  delete cmd;
  delete res;
  return rv;
}

// unofficial functions

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciGetLibVersion(DWORD *pVer) {
  if (pVer)
    *pVer = CHZBCI_VER;
  return ZCBCI_NOERROR;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciGetKey(ZCBCICARD card,
                                        BYTE keynum, BYTE *pKey, BYTE *pcbKey) {
  return Check(c[card]->getKey(keynum, pKey, pcbKey));
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciStartMultiAppEncryption(ZCBCICARD card, WORD *pSW1SW2, BYTE *pAlgo, BYTE key,
                                                          WORD keyCID) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->startEncryption(*pAlgo, key, keyCID);
  *pAlgo = c[card]->getAlgo();
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciFindComponent(ZCBCICARD card, WORD *pSW1SW2, BYTE type, const CHAR *pszPath,
                                                WORD *pCID) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->findComponent(type, pszPath, pCID);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciSelectApplication(ZCBCICARD card, WORD *pSW1SW2, const CHAR *pszPath) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->selectApplication(pszPath);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciExternalAuthenticate(ZCBCICARD card, WORD *pSW1SW2, BYTE algo, BYTE key,
                                                       WORD keyCID) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->externalAuthenticate(algo, key, keyCID);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciInternalAuthenticate(ZCBCICARD card, WORD *pSW1SW2, BYTE algo, BYTE key,
                                                      WORD keyCID) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->internalAuthenticate(algo, key, keyCID);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciVerify(ZCBCICARD card, WORD *pSW1SW2, WORD keyCID, const CHAR *pszPassword) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->verify(keyCID, pszPassword);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciGrantPrivilege(ZCBCICARD card, WORD *pSW1SW2, WORD prvCID, const CHAR *pszPath) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->grantPrivilege(prvCID, pszPath);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciGetExtendedID(ZCBCICARD card, WORD *pSW1SW2,
                                                BYTE index, BYTE type, CHAR *pszID, BYTE cbID) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->getApplicationID(index, type, pszID, cbID);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciGetFreeMemory(ZCBCICARD card, WORD *pSW1SW2, WORD *pTotal, WORD *pBlock) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->getFreeMemory(pTotal, pBlock);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciComponentName(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID, CHAR *pszName, BYTE cbName) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->componentName(cmpCID, pszName, cbName);
  SetSW1SW2();
  return rv;
}


ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciCreateComponent(ZCBCICARD card, WORD *pSW1SW2, BYTE type, const CHAR *pszPath,
                                                  const void *pAttr, BYTE cbAttr,
                                                  const void *pData, BYTE cbData, WORD *pCID) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->createComponent(type, pszPath, pAttr, cbAttr, pData, cbData, pCID);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciDeleteComponent(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->deleteComponent(cmpCID);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciWriteComponentAttr(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID,
                                                     const void *pAttr, BYTE cbAttr) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->writeComponentAttr(cmpCID, pAttr, cbAttr);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciReadComponentAttr(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID,
                                                    void *pAttr, BYTE *pcbAttr) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->readComponentAttr(cmpCID, pAttr, pcbAttr);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciWriteComponentData(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID,
                                                     const void *pData, BYTE cbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->writeComponentData(cmpCID, pData, cbData);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciReadComponentData(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID,
                                                    void *pData, BYTE *pcbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->readComponentData(cmpCID, pData, pcbData);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciAuthenticateFile(ZCBCICARD card, WORD *pSW1SW2, WORD keyCID,
                                                   BYTE algo, const CHAR *pszPath, const void *pSignature) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->authenticateFile(keyCID, algo, pszPath, pSignature);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciReadRightsList(ZCBCICARD card, WORD *pSW1SW2, const CHAR *pszPath,
                                                 WORD *pList, BYTE *pcbList) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->readRightsList(pszPath, pList, pcbList);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciLoadSequence(ZCBCICARD card, WORD *pSW1SW2, BYTE phase) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->loadSequence(phase);
  SetSW1SW2();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciSecureTransport(ZCBCICARD card, WORD *pSW1SW2, WORD keyCID,
                                                  BYTE algo, const void *pNonce, BYTE cbNonce) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->secureTransport(keyCID, algo, pNonce, cbNonce);
  SetSW1SW2();
  return rv;
}

// File I/O functions

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciMkDir(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                       const CHAR *pszPath) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->mkDir(pszPath);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciRmDir(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                       const CHAR *pszPath) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->rmDir(pszPath);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciChDir(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                       const CHAR *pszPath) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->chDir(pszPath);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciCurDir(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                        CHAR *pszPath, BYTE cbPath) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->curDir(pszPath, cbPath);
  SetSW1SW2();
  SetFileErr();
  return rv;
}
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciRename(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                        const CHAR *pszOldPath, const CHAR *pszNewPath) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->rename(pszOldPath, pszNewPath);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciDirCount(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                          const CHAR *pszFileSpec, WORD *pCount) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->dirCount(pszFileSpec, pCount);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciDirFile(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                         const CHAR *pszFileSpec, WORD num,
                                         CHAR *pszFile, BYTE cbFile) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->dirFile(pszFileSpec, num, pszFile, cbFile);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetAttr(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                         const CHAR *pszPath, WORD *pAttrib) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->getAttr(pszPath, pAttrib);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciEraseFile(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                           const CHAR *pszPath) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->eraseFile(pszPath);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciOpenFile(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                          const CHAR *pszPath, BYTE mode, WORD recordlen,
                                          WORD *pFileNum) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->openFile(pszPath, mode, recordlen, pFileNum);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciCloseFile(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                           WORD FileNum) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->closeFile(FileNum);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciCloseAllFiles(ZCBCICARD card, WORD *pSW1SW2,
                                               BYTE *pFileErr) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->closeAllFiles();
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileLength(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                            WORD FileNum, DWORD *pFileLength) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileLength(FileNum, pFileLength);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetFilepos(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                            WORD FileNum, DWORD *pFilePos) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->getFilepos(FileNum, pFilePos);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciSetFilepos(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                            WORD FileNum, DWORD FilePos) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->setFilepos(FileNum, FilePos);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciQueryEof(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                          WORD FileNum, BOOL *pfEof) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->queryEof(FileNum, pfEof);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileGet(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                         WORD FileNum, BYTE *pData, BYTE cbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileGet(FileNum, pData, cbData);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileGetPos(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                            WORD FileNum, DWORD pos,
                                            BYTE *pData, BYTE cbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileGetPos(FileNum, pos, pData, cbData);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFilePut(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                         WORD FileNum, const BYTE *pData, BYTE cbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->filePut(FileNum, pData, cbData);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFilePutPos(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                            WORD FileNum, DWORD pos,
                                            const BYTE *pData, BYTE cbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->filePutPos(FileNum, pos, pData, cbData);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileRead(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                          WORD FileNum, BYTE *pData, BYTE cbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileRead(FileNum, pData, cbData);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadLong(ZCBCICARD card, WORD *pSW1SW2,
                                              BYTE *pFileErr, WORD FileNum,
                                              LONG *plVar) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileReadLong(FileNum, plVar);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadSingle(ZCBCICARD card, WORD *pSW1SW2,
                                                BYTE *pFileErr, WORD FileNum,
                                                float *pflVar) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileReadSingle(FileNum, pflVar);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadString(ZCBCICARD card, WORD *pSW1SW2,
                                                BYTE *pFileErr, WORD FileNum,
                                                CHAR *pszVar, BYTE cbVar) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileReadString(FileNum, pszVar, cbVar);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadUser(ZCBCICARD card, WORD *pSW1SW2,
                                              BYTE *pFileErr, WORD FileNum,
                                              BYTE *pData, BYTE *pcbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileReadUser(FileNum, pData, pcbData);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadUser2(ZCBCICARD card, WORD *pSW1SW2,
                                               BYTE *pFileErr, WORD FileNum,
                                               BYTE *pData, BYTE cbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileReadUser2(FileNum, pData, cbData);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadLine(ZCBCICARD card, WORD *pSW1SW2,
                                              BYTE *pFileErr, WORD FileNum,
                                              CHAR *pszLine, BYTE cbLine) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileReadLine(FileNum, pszLine, cbLine);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileWriteLong(ZCBCICARD card, WORD *pSW1SW2,
                                               BYTE *pFileErr, WORD FileNum,
                                               LONG lVal) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileWriteLong(FileNum, lVal);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileWriteSingle(ZCBCICARD card, WORD *pSW1SW2,
                                                 BYTE *pFileErr, WORD FileNum,
                                                 float flVal) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileWriteSingle(FileNum, flVal);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileWriteUser(ZCBCICARD card, WORD *pSW1SW2,
                                               BYTE *pFileErr, WORD FileNum,
                                               const BYTE *pData, BYTE cbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileWriteUser(FileNum, pData, cbData);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileWriteString(ZCBCICARD card, WORD *pSW1SW2,
                                                 BYTE *pFileErr, WORD FileNum,
                                                 const CHAR *pszVal) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileWriteString(FileNum, pszVal);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileWrite(ZCBCICARD card, WORD *pSW1SW2,
                                           BYTE *pFileErr, WORD FileNum,
                                           const BYTE *pData, BYTE cbData) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->fileWrite(FileNum, pData, cbData);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFilePrint(ZCBCICARD card, WORD *pSW1SW2,
                                           BYTE *pFileErr, WORD FileNum,
                                           const CHAR *pszVal) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->filePrint(FileNum, pszVal);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetLockInfo(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                             const CHAR *pszPath, PZCBCILOCKINFO pInfo) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->getLockInfo(pszPath, pInfo);
  SetSW1SW2();
  SetFileErr();
  return rv;
}

ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciSetFileLock(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
                                             const CHAR *pszPath, ZCBCILOCK type, BOOL fLock,
                                             BYTE KeyCount, BYTE Key1, BYTE Key2) {
  CheckAndReturn();
  ZCBCIRET rv = c[card]->setFileLock(pszPath, type, fLock, KeyCount, Key1, Key2);
  SetSW1SW2();
  SetFileErr();
  return rv;
}