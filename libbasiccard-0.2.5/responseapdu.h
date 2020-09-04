/***************************************************************************
                          responseapdu.h  -  description
                             -------------------
    begin                : Fr Mär 26 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef RESPONSEAPDU_H
#define RESPONSEAPDU_H

#include <winscard.h>

/**
  *@author Christian Zietz
  */

class ResponseAPDU {
protected:
  WORD SW1SW2;
  BYTE *data;
  BYTE datalen;
  BYTE *packed;
public: 
  ResponseAPDU();
  virtual ~ResponseAPDU();
  
  virtual void setSW1SW2(WORD param);
  virtual void setData(const BYTE *param, BYTE len);
  virtual WORD getSW1SW2() { return SW1SW2; };
  virtual bool isNoError() { return (SW1SW2==0x9000) || (SW1SW2>>8==0x61); };
  virtual BYTE getData(BYTE *param);
  virtual BYTE getData(BYTE *param, BYTE offset, BYTE len);

  virtual BYTE packAPDU(BYTE *param);
  virtual void unpackAPDU(BYTE *param, BYTE len);  
};

#endif
