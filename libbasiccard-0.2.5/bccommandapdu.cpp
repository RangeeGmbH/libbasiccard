/***************************************************************************
                          bccommandapdu.cpp  -  description
                             -------------------
    begin                : So Mär 28 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#include "winport.h"
#include "bccommandapdu.h"
#include <cstring>

BCCommandAPDU::BCCommandAPDU() : CommandAPDU() {

}

BCCommandAPDU::BCCommandAPDU(BYTE Cla, BYTE Ins, BYTE P1, BYTE P2)
              : CommandAPDU(Cla, Ins, P1, P2) {

}

BCCommandAPDU::BCCommandAPDU(BYTE Cla, BYTE Ins, BYTE P1, BYTE P2, BYTE Lc, const BYTE *data, BYTE Le, BYTE LeEnabled)
              : CommandAPDU(Cla, Ins, P1, P2, Lc, data, Le, LeEnabled) {
}

void BCCommandAPDU::appendByte(const BYTE param) {
  BYTE tempdata[300];
  BYTE templen;

  templen = getData(tempdata);
  if (!setLc(templen+1)) {
    return;
  }
  setData(tempdata, 0, templen);
  data[templen] = param;
}

void BCCommandAPDU::appendWord(const WORD param) {
  BYTE tempdata[300];
  BYTE templen;

  templen = getData(tempdata);
  if (!setLc(templen+2)) {
    return;
  }
  setData(tempdata, 0, templen);
  data[templen] = param >> 8;
  data[templen+1] = param & 0xff;
}

void BCCommandAPDU::appendLong(const LONG param) {
  BYTE tempdata[300];
  BYTE templen;

  templen = getData(tempdata);
  if (!setLc(templen+4)) {
    return;
  }
  setData(tempdata, 0, templen);
  data[templen] = param >> 24;
  data[templen+1] = (param >> 16) & 0xff;
  data[templen+2] = (param >> 8) & 0xff;
  data[templen+3] = param & 0xff;
}

void BCCommandAPDU::appendFloat(const FLOAT param) {
  BYTE tempdata[300];
  BYTE templen;
  DWORD dummy;

  templen = getData(tempdata);
  if (!setLc(templen+4)) {
    return;
  }
  setData(tempdata, 0, templen);
  memcpy(&dummy, &param, 4);
  data[templen] = dummy >> 24;
  data[templen+1] = (dummy >> 16) & 0xff;
  data[templen+2] = (dummy >> 8) & 0xff;
  data[templen+3] = dummy & 0xff;
}

void BCCommandAPDU::appendStringN(const BYTE* param, const BYTE n) {
  BYTE tempdata[300];
  BYTE templen;

  templen = getData(tempdata);
  if (!setLc(templen+n)) {
    return;
  }
  setData(tempdata, 0, templen);
  memcpy(data+templen, param, n);
}

void BCCommandAPDU::appendString(const CHAR* param) {
  BYTE tempdata[300];
  BYTE templen;
  BYTE n = strlen(param) & 0xff;

  templen = getData(tempdata);
  if (!setLc(templen+n)) {
    return;
  }
  setData(tempdata, 0, templen);
  memcpy(data+templen, param, n);
}
