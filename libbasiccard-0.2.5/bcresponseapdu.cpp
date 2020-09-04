/***************************************************************************
                          bcresponseapdu.cpp  -  description
                             -------------------
    begin                : So Mär 28 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#include "winport.h"
#include "bcresponseapdu.h"
#include <cstring>

BCResponseAPDU::BCResponseAPDU() : ResponseAPDU() {
  dataptr = NULL;
}

void BCResponseAPDU::setData(const BYTE *param, BYTE len) {
  ResponseAPDU::setData(param, len);
  restart();
}

void BCResponseAPDU::restart() {
  dataptr = data;
}

int BCResponseAPDU::getRemainingLen() {
  return (data+datalen)-dataptr;
}

int BCResponseAPDU::extractByte(BYTE *param) {
  if (dataptr+1<=data+datalen) {
    *param = dataptr[0];
    dataptr += 1;
    return 1;
  } else {
    return 0;
  }
}

int BCResponseAPDU::extractWord(WORD *param) {
  if (dataptr+2<=data+datalen) {
    *param = (dataptr[0]<<8)+dataptr[1];
    dataptr += 2;
    return 2;
  } else {
    return 0;
  }
}

int BCResponseAPDU::extractLong(LONG *param) {
  if (dataptr+4<=data+datalen) {
    *param = (dataptr[0]<<24)+(dataptr[1]<<16)+(dataptr[2]<<8)+dataptr[3];
    dataptr += 4;
    return 4;
  } else {
    return 0;
  }
}

int BCResponseAPDU::extractFloat(FLOAT *param) {
  DWORD dummy;
  if (dataptr+4<=data+datalen) {
    dummy = (dataptr[0]<<24)+(dataptr[1]<<16)+(dataptr[2]<<8)+dataptr[3];
    memcpy(param, &dummy, 4);
    dataptr += 4;
    return 4;
  } else {
    return 0;
  }
}

int BCResponseAPDU::extractStringN(BYTE* param, const BYTE n) {
  if (dataptr+n<=data+datalen) {
    memcpy(param, dataptr, n);
    dataptr += n;
    return n;
  } else {
    return 0;
  }
}

int BCResponseAPDU::extractString(CHAR* param) {
  if (dataptr<data+datalen) {
    BYTE n = data+datalen-dataptr;
    memcpy(param, dataptr, n);
    param[n] = 0;
    dataptr += n;
    return n;
  } else {
    return 0;
  }
}
