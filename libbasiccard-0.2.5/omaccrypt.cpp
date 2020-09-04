/***************************************************************************
                          omaccrypt  -  description
                             -------------------
    begin                : Fri Sep 17 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/

#include "winport.h"
#include "omaccrypt.h"
#include <zcbci.h>

#define NO_MP_S_RMAP
#include <stdio.h>
#include <tomcrypt.h>

#include <stdlib.h>
#include <cstring>

OMACCrypt::OMACCrypt(BYTE algo, BYTE *key){
  this->key = NULL;

  this->algo = algo;
  switch (algo) {
    case 0x81: // OMAC_AES128
      keylen = 16;
      break;
    case 0x82: // OMAC_AES192
      keylen = 24;
      break;
    case 0x83: // OMAC_AES256
      keylen = 32;
      break;
    default:
      keylen = 0;
      return;
  }

  this->key = new BYTE[keylen];

  memcpy(this->key, key, keylen);

  register_cipher(&aes_desc);
}

OMACCrypt::~OMACCrypt(){
  if (key!=NULL)
    delete []key;
}

bool OMACCrypt::encryptCommand(CommandAPDU *c){
  BYTE *cryptBuf = new BYTE[300];
  BYTE Le, Lc;
  long unsigned int tagLen;
  int lenCryptBuf;
  omac_state omac;

  cryptBuf[0] = c->getCla();
  cryptBuf[1] = c->getIns();
  cryptBuf[2] = c->getP1();
  cryptBuf[3] = c->getP2();

  Lc = cryptBuf[4] = c->getLc()+16;
  c->getData(&cryptBuf[5]);

  lenCryptBuf = 5 + c->getLc();

  if (c->isLeEnabled()) {
    Le = cryptBuf[lenCryptBuf] = (c->getLe()==0?0:c->getLe()+16);
  } else {
    Le = cryptBuf[lenCryptBuf] = 16;
  }
  lenCryptBuf++;

  omac_init(&omac, find_cipher("aes"), key, keylen);
  omac_process(&omac, cryptBuf, lenCryptBuf);
  tagLen = 16;
  omac_done(&omac, &cryptBuf[lenCryptBuf-1], &tagLen);

  c->setLc(Lc);
  c->setData(&cryptBuf[5]);
  c->setLeEnabled(true);
  c->setLe(Le);

  return true;
}

bool OMACCrypt::decryptResponse(ResponseAPDU *r) {
  BYTE *cryptBuf = new BYTE[300];
  BYTE *tag1 = new BYTE[16];
  BYTE *tag2 = new BYTE[16];
  int lenCryptBuf = r->getData(cryptBuf);
  long unsigned int tagLen;
  omac_state omac;

  if (lenCryptBuf==0) {
    delete []cryptBuf;
    return true;
  }

  memcpy(tag1, &cryptBuf[lenCryptBuf-16], 16); // strip tag
  lenCryptBuf -= 16;
  cryptBuf[lenCryptBuf++] = r->getSW1SW2() >> 8;
  cryptBuf[lenCryptBuf++] = r->getSW1SW2() & 0xff;

  omac_init(&omac, find_cipher("aes"), key, keylen);
  omac_process(&omac, cryptBuf, lenCryptBuf);
  tagLen = 16;
  omac_done(&omac, tag2, &tagLen);

  r->setData(cryptBuf, lenCryptBuf-2);

  return (memcmp(tag1, tag2, 16)==0? true:false);
}





