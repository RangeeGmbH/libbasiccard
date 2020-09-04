/***************************************************************************
                          omaccrypt  -  description
                             -------------------
    begin                : Fri Sep 17 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/

#ifndef OMACCRYPT_H
#define OMACCRYPT_H

#include "abstractcrypt.h"

/**
  *@author Christian Zietz
  */

class OMACCrypt : public virtual AbstractCrypt  {
private:
  BYTE algo;
  BYTE *key;
  int keylen;
public: 
  OMACCrypt(BYTE algo, BYTE *key);
  virtual ~OMACCrypt();
  virtual bool encryptCommand(CommandAPDU *c);
  virtual bool decryptResponse(ResponseAPDU *r);  
};

#endif
