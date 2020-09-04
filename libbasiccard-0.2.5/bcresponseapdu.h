/***************************************************************************
                          bcresponseapdu.h  -  description
                             -------------------
    begin                : So Mär 28 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef BCRESPONSEAPDU_H
#define BCRESPONSEAPDU_H

#include "responseapdu.h"
#include <zcbci.h>

/**
  *@author Christian Zietz
  */

class BCResponseAPDU : public ResponseAPDU  {
protected:
  BYTE *dataptr;
public: 
  BCResponseAPDU();
  virtual void setData(const BYTE *param, BYTE len);

  virtual void restart();
  virtual int getRemainingLen();
  virtual int extractByte(BYTE *param);
  virtual int extractWord(WORD *param);
  virtual int extractLong(LONG *param);
  virtual int extractFloat(FLOAT *param);
  virtual int extractStringN(BYTE* param, const BYTE n);
  virtual int extractString(CHAR* param);  
};

#endif
