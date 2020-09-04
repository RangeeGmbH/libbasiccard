/***************************************************************************
                          abstractcard.h  -  description
                             -------------------
    begin                : Sat Mar 27 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef ABSTRACTCARD_H
#define ABSTRACTCARD_H

#include "abstractreader.h"
#include "commandapdu.h"
#include "responseapdu.h"

/**
  *@author Christian Zietz
  */

class AbstractCard {
protected:
  AbstractReader *reader;
  WORD SW1SW2;
public: 
	AbstractCard(AbstractReader *r);
	virtual ~AbstractCard();
  
  virtual ZCCRIRET transaction(CommandAPDU *cmd, ResponseAPDU *res);
  virtual WORD getSW1SW2() { return SW1SW2; };
  virtual bool isNoError() { return (SW1SW2==0x9000) || (SW1SW2>>8==0x61); };
    
  virtual AbstractReader *getReader();
  virtual void setReader(AbstractReader *param);
};

#endif
