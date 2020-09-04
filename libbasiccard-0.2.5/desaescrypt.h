/***************************************************************************
                          desaescrypt.h  -  description
                             -------------------
    begin                : Sat Mar 27 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef DESAESCRYPT_H
#define DESAESCRYPT_H

#include "abstractcrypt.h"

/**
  *@author Christian Zietz
  */

class DESAESCrypt : public virtual AbstractCrypt  {
private:
  BYTE algo;
  BYTE *key;
  BYTE *iv;
  int blocklen;
  int keylen;
  bool isProfessional;
public:
  DESAESCrypt(BYTE algo, BYTE *key, BYTE *RA, BYTE *RB);
  virtual ~DESAESCrypt();
  virtual bool encryptCommand(CommandAPDU *c);
  virtual bool decryptResponse(ResponseAPDU *r);
  virtual bool encryptBlock(BYTE *block);
};

#endif
