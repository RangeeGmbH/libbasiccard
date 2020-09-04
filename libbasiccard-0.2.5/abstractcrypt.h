/***************************************************************************
                          abstractcrypt.h  -  description
                             -------------------
    begin                : Sat Mar 27 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef ABSTRACTCRYPT_H
#define ABSTRACTCRYPT_H

#include <winscard.h>
#include "commandapdu.h"
#include "responseapdu.h"

/**
  *@author Christian Zietz
  */

class AbstractCrypt {
public: 
	AbstractCrypt() {};
	virtual ~AbstractCrypt() {};
  virtual bool encryptCommand(CommandAPDU *c) = 0;
  virtual bool decryptResponse(ResponseAPDU *r) = 0;
};

#endif
