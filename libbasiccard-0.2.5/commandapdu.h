/***************************************************************************
                          commandapdu.h  -  description
                             -------------------
    begin                : Fr Mär 26 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef COMMANDAPDU_H
#define COMMANDAPDU_H

#include <winscard.h>

/**
  *@author Christian Zietz
  */

class CommandAPDU {
protected:
  BYTE Cla;
  BYTE Ins;
  BYTE P1, P2;
  BYTE Lc, Le;
  bool LeEnabled;
  BYTE *data;
  BYTE *packed;
public: 
  CommandAPDU();
  CommandAPDU(BYTE Cla, BYTE Ins, BYTE P1, BYTE P2);
  CommandAPDU(BYTE Cla, BYTE Ins, BYTE P1, BYTE P2, BYTE Lc, const BYTE *data, BYTE Le, BYTE LeEnabled);
  virtual ~CommandAPDU();
  virtual void setCla(BYTE param);
  virtual void setIns(BYTE param);
  virtual void setP1(BYTE param);
  virtual void setP2(BYTE param);
  virtual bool setLc(int param);
  virtual void setLe(BYTE param);
  virtual void setLeEnabled(bool param);
  virtual void setData(const BYTE *param);
  virtual void setData(const BYTE *param, BYTE offset, BYTE len);
  
  virtual BYTE getCla() {return Cla;};
  virtual BYTE getIns() {return Ins;};
  virtual BYTE getP1() {return P1;};
  virtual BYTE getP2() {return P2;};
  virtual BYTE getLc() {return Lc;};
  virtual BYTE getLe() {return Le;};
  virtual bool isLeEnabled() {return LeEnabled;};
  virtual BYTE getData(BYTE *param);

  virtual BYTE packAPDU(BYTE *param);
  virtual void unpackAPDU(BYTE *param, BYTE len);
};

#endif
