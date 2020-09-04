/***************************************************************************
                          eaxcrypt  -  description
                             -------------------
    begin                : Fri Sep 17 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/

#ifndef EAXCRYPT_H
#define EAXCRYPT_H

#include "abstractcrypt.h"

/**
  *@author Christian Zietz
  */

class EAXCrypt : public virtual AbstractCrypt  {
private:
  BYTE algo;
  BYTE *key;
  BYTE *nonce;
  int keylen;
public: 
  EAXCrypt(BYTE algo, BYTE *key, BYTE *RA, BYTE *RB);
  virtual ~EAXCrypt();
  virtual bool encryptCommand(CommandAPDU *c);
  virtual bool decryptResponse(ResponseAPDU *r);  
};

#endif
