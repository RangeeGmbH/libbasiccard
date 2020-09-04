/***************************************************************************
                          eaxcrypt  -  description
                             -------------------
    begin                : Fri Sep 17 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/

#include "winport.h"
#include "eaxcrypt.h"
#include <zcbci.h>

#include <stdio.h>

#define NO_MP_S_RMAP
#include <tomcrypt.h>

#include <stdlib.h>
#include <cstring>

EAXCrypt::EAXCrypt(BYTE algo, BYTE *key, BYTE *RA, BYTE *RB){
  this->key = this->nonce = NULL;

  this->algo = algo;
  switch (algo) {
    case 0x41: // EAX_AES128
      keylen = 16;
      break;
    case 0x42: // EAX_AES192
      keylen = 24;
      break;
    case 0x43: // EAX_AES256
      keylen = 32;
      break;
    default:
      keylen = 0;
      return;
  }

  this->key = new BYTE[keylen];
  this->nonce = new BYTE[16];

  memcpy(this->key, key, keylen);
  memcpy(this->nonce, RA, 4);
  memcpy(this->nonce+4, RB, 8);
  memcpy(this->nonce+12, RA+4, 4);

  register_cipher(&aes_desc);
}

EAXCrypt::~EAXCrypt(){
  if (key!=NULL)
    delete []key;
  if (nonce!=NULL)
    delete []nonce;
}

bool EAXCrypt::encryptCommand(CommandAPDU *c){
  BYTE *cryptBuf = new BYTE[300];
  BYTE *authBuf = new BYTE[6];
  BYTE Le, Lc;
  long unsigned int tagLen;
  eax_state eax;

  authBuf[0] = c->getCla();
  authBuf[1] = c->getIns();
  authBuf[2] = c->getP1();
  authBuf[3] = c->getP2();

  Lc = authBuf[4] = c->getLc()+16;
  if (c->isLeEnabled()) {
    Le = authBuf[5] = (c->getLe()==0?0:c->getLe()+16);
  } else {
    Le = authBuf[5] = 16;
  }

  c->getData(cryptBuf);
  eax_init(&eax, find_cipher("aes"), key, keylen, nonce, 16, authBuf, 6);
  if (c->getLc()>0) {
    eax_encrypt(&eax, cryptBuf, cryptBuf, c->getLc());
  }
  tagLen = 16;
  eax_done(&eax, nonce, &tagLen);
  memcpy(&cryptBuf[c->getLc()], nonce, 16);

  c->setLc(Lc);
  c->setData(cryptBuf);
  c->setLeEnabled(true);
  c->setLe(Le);

  return true;
}

bool EAXCrypt::decryptResponse(ResponseAPDU *r) {
  BYTE *cryptBuf = new BYTE[300];
  BYTE SW1SW2[2];
  int lenCryptBuf = r->getData(cryptBuf);
  long unsigned int tagLen;
  eax_state eax;

  if (lenCryptBuf==0) {
    delete []cryptBuf;
    return true;
  }

  SW1SW2[0] = r->getSW1SW2() >> 8;
  SW1SW2[1] = r->getSW1SW2() & 0xff;

  eax_init(&eax, find_cipher("aes"), key, keylen, nonce, 16, SW1SW2, 2);
  if (lenCryptBuf > 16) {
    eax_decrypt(&eax, cryptBuf, cryptBuf, lenCryptBuf-16);
    r->setData(cryptBuf, lenCryptBuf-16);
  } else {
    r->setData(cryptBuf, 0);
  }
  tagLen = 16;
  eax_done(&eax, nonce, &tagLen);

  return (memcmp(&cryptBuf[lenCryptBuf-16], nonce, 16)==0? true:false);
}





