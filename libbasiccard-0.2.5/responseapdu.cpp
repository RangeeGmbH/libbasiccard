/***************************************************************************
                          responseapdu.cpp  -  description
                             -------------------
    begin                : Fr Mär 26 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#include "winport.h"
#include "responseapdu.h"
#include <stdlib.h>
#include <cstring>

ResponseAPDU::ResponseAPDU(){
  data=NULL;
  packed=NULL;
  datalen = 0;
  SW1SW2 = 0;
}

ResponseAPDU::~ResponseAPDU(){
  if (data!=NULL)
    delete []data;
  if (packed!=NULL)
    delete []packed;
}

void ResponseAPDU::setSW1SW2(WORD param){
  SW1SW2 = param;
}

void ResponseAPDU::setData(const BYTE *param, BYTE len){
  if (data!=NULL)
    delete []data;

  datalen = len;
  data = new BYTE[datalen];
  memcpy(data, param, len);
}

BYTE ResponseAPDU::getData(BYTE *param){
  if (param!=NULL)
    memcpy(param, data, datalen);
  return datalen;
}

BYTE ResponseAPDU::getData(BYTE *param, BYTE offset, BYTE len){
  if ((param!=NULL)&&(offset+len<=datalen)) {
    memcpy(param, data+offset, len);
  }
  return len;
}

BYTE ResponseAPDU::packAPDU(BYTE *param){
  BYTE packedsize;

  if (packed!=NULL)
    delete []packed;

  packedsize = 2+datalen;
  packed = new BYTE[packedsize];

  memcpy(packed, data, datalen);
  packed[datalen] = SW1SW2 >> 8;
  packed[datalen+1] = SW1SW2 & 0xff;

  if (param!=NULL)
    memcpy(param, packed, packedsize);

  return packedsize;
}

void ResponseAPDU::unpackAPDU(BYTE *param, BYTE len){
  setData(param, len-2);
  setSW1SW2((param[len-2]<<8) + param[len-1]);
}
