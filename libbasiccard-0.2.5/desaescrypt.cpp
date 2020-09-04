/***************************************************************************
                          desaescrypt.cpp  -  description
                             -------------------
    begin                : Sat Mar 27 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#include "winport.h"
#include "desaescrypt.h"
#include <zcbci.h>
#include <chzbci.h>

#include <mydes.h>
#include <my3des.h>
#include <myaes.h>
#include <crc.h>

#include <tomcrypt.h>

#include <stdlib.h>
#include <cstring>

#include <stdio.h>

DESAESCrypt::DESAESCrypt(BYTE algo, BYTE *key, BYTE *RA, BYTE *RB){
  this->key = this->iv = NULL;
  this->isProfessional = false;

  this->algo = algo;
  switch (algo) {
    case ZCBCI_DES:
      this->keylen = 8;
      this->blocklen = 8;
      this->isProfessional = false;
      break;
    case ZCBCI_DES_CRC:
      this->keylen = 8;
      this->blocklen = 8;
      this->isProfessional = true;
      break;
    case ZCBCI_3DES:
      this->keylen = 16;
      this->blocklen = 8;
      this->isProfessional = false;
      break;
    case ZCBCI_3DES_CRC:
      this->keylen = 16;
      this->blocklen = 8;
      this->isProfessional = true;
      break;
    case ZCBCI_AES:
      this->keylen = 16;
      this->blocklen = 16;
      this->isProfessional = true;
      break;
    case CHZBCI_AES192: // AES_192
      this->keylen = 24;
      this->blocklen = 16;
      this->isProfessional = true;
      break;
    case CHZBCI_AES256: // AES_256
      this->keylen = 32;
      this->blocklen = 16;
      this->isProfessional = true;
      break;
    default:
      this->keylen = 0;
      this->blocklen = 0;
      return;
  }

  this->key = new BYTE[this->keylen];
  this->iv = new BYTE[this->blocklen];

  memcpy(this->key, key, keylen);
  memcpy(this->iv, RA, blocklen/4);
  memcpy(this->iv+(blocklen/4), RB, blocklen/2);
  memcpy(this->iv+((blocklen/4)*3), RA+(blocklen/4), blocklen/4);
}

DESAESCrypt::~DESAESCrypt(){
  if (key!=NULL)
    delete []key;
  if (iv!=NULL)
    delete []iv;
}

bool DESAESCrypt::encryptCommand(CommandAPDU *c){
  BYTE *cryptBuf = new BYTE[300];
  BYTE i;
  int lenCryptBuf = 0;

  if (!isProfessional) {
    lenCryptBuf = 5+c->getLc();
    cryptBuf[0] = c->getCla();
    cryptBuf[1] = c->getIns();
    cryptBuf[2] = c->getP1();
    cryptBuf[3] = c->getP2();
    cryptBuf[4] = c->getLc();
    c->getData(cryptBuf+5);
    BYTE Le = (c->isLeEnabled()?c->getLe():0);
    cryptBuf[lenCryptBuf++] = Le;
    cryptBuf[lenCryptBuf++] = 0;
    cryptBuf[lenCryptBuf++] = 0;
  } else {
    unsigned long crc = 0;
    i = c->getCla();
    UpdateCRC32(crc, &i, sizeof(i));
    i = c->getIns();
    UpdateCRC32(crc, &i, sizeof(i));
    i = c->getP1();
    UpdateCRC32(crc, &i, sizeof(i));
    i = c->getP2();
    UpdateCRC32(crc, &i, sizeof(i));
    i = c->getLc();
    lenCryptBuf = i;
    c->getData(cryptBuf);
    if (lenCryptBuf!=0) {
      UpdateCRC32(crc, &i, sizeof(i)); // Lc
      UpdateCRC32(crc, cryptBuf, lenCryptBuf);
    }
    BYTE LeFlag = (c->isLeEnabled())?1:0;
    i = (c->isLeEnabled())?c->getLe():0;
    UpdateCRC32(crc, &LeFlag, sizeof(LeFlag));
    UpdateCRC32(crc, &i, sizeof(i)); // Le
    cryptBuf[lenCryptBuf++] = LeFlag;
    cryptBuf[lenCryptBuf++] = i;

    BYTE zeros = (blocklen == 8)?2:10;
    for (i=0; i<zeros; i++) {
      cryptBuf[lenCryptBuf++] = 0;
    }
    cryptBuf[lenCryptBuf++] = (BYTE) ((crc >> 24) & 0xff);
    cryptBuf[lenCryptBuf++] = (BYTE) ((crc >> 16) & 0xff);
    cryptBuf[lenCryptBuf++] = (BYTE) ((crc >> 8) & 0xff);
    cryptBuf[lenCryptBuf++] = (BYTE) ((crc >> 0) & 0xff);
  }

  switch (algo) {
    case ZCBCI_DES:
    case ZCBCI_DES_CRC:
      mekdes(cryptBuf, lenCryptBuf, key, iv);
      break;
    case ZCBCI_3DES:
    case ZCBCI_3DES_CRC:
      mek3des(cryptBuf, lenCryptBuf, key, iv);
      break;
    case ZCBCI_AES:
    case CHZBCI_AES192: // AES_192
    case CHZBCI_AES256: // AES_256
      mekaes(cryptBuf, lenCryptBuf, key, keylen, iv);
      break;
  }

  c->setLc(lenCryptBuf);
  c->setData(cryptBuf);

  if (isProfessional) {
    i = c->getLe();
    if (!c->isLeEnabled()) {
      i = blocklen;
    } else {
      if (i != 0) {
        i += blocklen;
      }
    }
    c->setLe(i);
    c->setLeEnabled(true);
  }

  delete []cryptBuf;
  return true;
}

bool DESAESCrypt::decryptResponse(ResponseAPDU *r) {
  BYTE *cryptBuf = new BYTE[300];
  int lenCryptBuf = r->getData(cryptBuf);

  if (lenCryptBuf==0) {
    delete []cryptBuf;
    return true;
  }

  switch (algo) {
    case ZCBCI_DES:
    case ZCBCI_DES_CRC:
      mdkdes(cryptBuf, lenCryptBuf, key, iv);
      break;
    case ZCBCI_3DES:
    case ZCBCI_3DES_CRC:
      mdk3des(cryptBuf, lenCryptBuf, key, iv);
      break;
    case ZCBCI_AES:
    case CHZBCI_AES192: // AES_192
    case CHZBCI_AES256: // AES_256
      mdkaes(cryptBuf, lenCryptBuf, key, keylen, iv);
      break;
  }

  if (!isProfessional) {
    if ((lenCryptBuf < 8) ||
        (cryptBuf[lenCryptBuf-1] != 0) ||
        (cryptBuf[lenCryptBuf-2] != 0) ||
        (cryptBuf[lenCryptBuf-3] != 0) ||
        (cryptBuf[lenCryptBuf-4] != 0) ||
        (cryptBuf[lenCryptBuf-5] != 0) ||
        (cryptBuf[lenCryptBuf-6] != 0)) {
      delete []cryptBuf;
      return false;
    }
    r->setData(cryptBuf, lenCryptBuf-8);
    delete []cryptBuf;
    return true;
  } else {
    int zeros = (blocklen == 8)?2:10;
    int lendata = lenCryptBuf-zeros-4;
    int crcpos = lenCryptBuf-4;
    unsigned long crc, crcverify;

    if (lendata<0) {
      delete []cryptBuf;
      return false;
    }
    crcverify = CRC32(cryptBuf, lendata);
    crc = (cryptBuf[crcpos]<<24)+(cryptBuf[crcpos+1]<<16)+(cryptBuf[crcpos+2]<<8)+(cryptBuf[crcpos+3]<<0);

    r->setData(cryptBuf, lendata-2);
    delete []cryptBuf;
    return (crc == crcverify);
  }
}

bool DESAESCrypt::encryptBlock(BYTE *block) {
  symmetric_key k;
  unsigned char newkey[24];

  switch (algo) {
    case ZCBCI_DES:
    case ZCBCI_DES_CRC:
      des_setup(key, keylen, 0, &k);
      des_ecb_encrypt(block, block, &k);
      break;
    case ZCBCI_3DES:
    case ZCBCI_3DES_CRC:
      memcpy(newkey, key, 16);
      memcpy(newkey+16, key, 8);
      des3_setup(newkey, 24, 0, &k);
      des3_ecb_encrypt(block, block, &k);
      break;
    case ZCBCI_AES:
    case CHZBCI_AES192:
    case CHZBCI_AES256:
      aes_setup(key, keylen, 0, &k);
      aes_ecb_encrypt(block, block, &k);
      break;
  }

  return true;
}

