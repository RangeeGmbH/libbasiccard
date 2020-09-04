/***************************************************************************
                          bccommandapdu.h  -  description
                             -------------------
    begin                : So Mär 28 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef BCCOMMANDAPDU_H
#define BCCOMMANDAPDU_H

#include "commandapdu.h"
#include <zcbci.h>

/**
  *@author Christian Zietz
  */

class BCCommandAPDU : public virtual CommandAPDU  {
public: 
  BCCommandAPDU();
  BCCommandAPDU(BYTE Cla, BYTE Ins, BYTE P1, BYTE P2);
  BCCommandAPDU(BYTE Cla, BYTE Ins, BYTE P1, BYTE P2, BYTE Lc, const BYTE *data, BYTE Le, BYTE LeEnabled);

  virtual void appendByte(const BYTE param);
  virtual void appendWord(const WORD param);
  virtual void appendLong(const LONG param);
  virtual void appendFloat(const FLOAT param);
  virtual void appendStringN(const BYTE* param, const BYTE n);
  virtual void appendString(const CHAR* param);
};

#endif
