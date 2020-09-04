/***************************************************************************
                          commandapdu.cpp  -  description
                             -------------------
    begin                : Fr Mär 26 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#include "winport.h"
#include "commandapdu.h"
#include <stdlib.h>
#include <stdio.h>
#include <cstring>

CommandAPDU::CommandAPDU(){
  data=NULL;
  packed=NULL;
  Cla = Ins = P1 = P2 = Lc = Le = 0;
  LeEnabled = false;
}

CommandAPDU::CommandAPDU(BYTE Cla, BYTE Ins, BYTE P1, BYTE P2) {
  this->data = NULL;
  this->packed = NULL;
  setCla(Cla);
  setIns(Ins);
  setP1(P1);
  setP2(P2);
  setLc(0);
  setLe(0);
  setLeEnabled(false);
}

CommandAPDU::CommandAPDU(BYTE Cla, BYTE Ins, BYTE P1, BYTE P2, BYTE Lc, const BYTE *data, BYTE Le, BYTE LeEnabled){
  this->data = NULL;
  this->packed = NULL;
  setCla(Cla);
  setIns(Ins);
  setP1(P1);
  setP2(P2);
  setLc(Lc);
  setLe(Le);
  setLeEnabled(LeEnabled);
  setData(data);
}

CommandAPDU::~CommandAPDU(){
  if (data!=NULL)
    delete []data;
  if (packed!=NULL)
    delete []packed;
}

void CommandAPDU::setCla(BYTE param){
  Cla = param;
}

void CommandAPDU::setIns(BYTE param){
  Ins = param;
}

void CommandAPDU::setP1(BYTE param){
  P1 = param;
}

void CommandAPDU::setP2(BYTE param){
  P2 = param;
}

bool CommandAPDU::setLc(int param){
  if ((param > 255) | (param < 0)){
    return false;
  }
  
  Lc = param;
  if (data!=NULL)
    delete []data;
  data = new BYTE[Lc];
  
  return data!=NULL;
}

void CommandAPDU::setLe(BYTE param){
  Le = param;
}

void CommandAPDU::setLeEnabled(bool param){
  LeEnabled = param;
}

void CommandAPDU::setData(const BYTE *param){
  if (param!=NULL)
    memcpy(data, param, Lc);
}

void CommandAPDU::setData(const BYTE *param, BYTE offset, BYTE len){
  if (offset+len>Lc)
    return;
  memcpy(data+offset, param, len);
}

BYTE CommandAPDU::getData(BYTE *param){
  if (param!=NULL)
    memcpy(param, data, Lc);
  return Lc;
}

BYTE CommandAPDU::packAPDU(BYTE *param){
  BYTE packedsize;
  bool LcEnabled;

  if (packed!=NULL)
    delete []packed;

  LcEnabled = (Lc != 0);
  packedsize = (LcEnabled?(5+Lc):4)+(LeEnabled?1:0);
  packed = new BYTE[packedsize];

  packed[0] = Cla;
  packed[1] = Ins;
  packed[2] = P1;
  packed[3] = P2;
  if (LcEnabled) {
    packed[4] = Lc;
    memcpy(packed+5, data, Lc);
  }
  if (LeEnabled) {
     packed[packedsize-1] = Le;
  }

  if (param!=NULL)
    memcpy(param, packed, packedsize);

  return packedsize;
}

void CommandAPDU::unpackAPDU(BYTE *param, BYTE len){

  setCla(param[0]);
  setIns(param[1]);
  setP1(param[2]);
  setP2(param[3]);
  if (len<=4) {
    setLc(0);
    setLe(0);
    setLeEnabled(false);
  } else if (len==5) {
    setLc(0);
    setLe(param[4]);
    setLeEnabled(true);
  } else {
    setLc(param[4]);
    memcpy(data, param+5, Lc);
    if (len>5+Lc) {
     setLe(param[5+Lc]);
     setLeEnabled(true);
    } else {
      setLe(0);
      setLeEnabled(false);
    }
  }
}
