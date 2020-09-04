/***************************************************************************
                          pcscdriver.cpp  -  description
                             -------------------
    begin                : Fr Mär 26 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#include "winport.h"
#include "pcscdriver.h"
#include "pcscreader.h"

#include <stdlib.h>
#include <winscard.h>

PCSCDriver::PCSCDriver(){
  unsigned long rv, len;
  LPSTR i, j;

  numReaders = 0;
  hContext = 0;

  rv = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
  if (rv!=SCARD_S_SUCCESS)
    return;

  rv = SCardListReaders(hContext, NULL, NULL, &len);
  if (rv!=SCARD_S_SUCCESS)
    return;

  i = (LPSTR)new char[len];
  j = i;
  rv = SCardListReaders(hContext, NULL, i, &len);
  if (rv!=SCARD_S_SUCCESS)
    return;

  while (*i != 0) {
    numReaders++;
    while (*i != 0) {
      i++;
    }
    i++;
  }

  delete []j;
}

PCSCDriver::~PCSCDriver(){
  SCardReleaseContext(hContext);
}

ULONG PCSCDriver::getReaderCount(){
  return numReaders;
}

AbstractReader *PCSCDriver::getReader(ULONG i){
  LPSTR j, l;
  unsigned long rv, k;
  unsigned long len;

  if (i<getReaderCount()) {
    rv = SCardListReaders(hContext, NULL, NULL, &len);
    if (rv!=SCARD_S_SUCCESS)
      return NULL;

    j = (LPSTR)new char[len];
    l = j;
    rv = SCardListReaders(hContext, NULL, j, &len);
    if (rv!=SCARD_S_SUCCESS)
      return NULL;

    for(k=0; k<i; k++) {
      while (*j != 0) {
        j++;
      }
      j++;
    }

    PCSCReader *r = new PCSCReader(hContext, j);

    delete []l;

    return r;
  } else {
    return NULL;
  }
}

DWORD PCSCDriver::getOptions(){
  return 0;
}

ZCCRIRET PCSCDriver::setOptions(DWORD options){
  return ZCCRI_ERROR_NOT_SUPPORTED;
}
