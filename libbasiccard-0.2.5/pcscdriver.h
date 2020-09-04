/***************************************************************************
                          pcscdriver.h  -  description
                             -------------------
    begin                : Fr Mär 26 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef PCSCDRIVER_H
#define PCSCDRIVER_H

#include "abstractdriver.h"
#include <winscard.h>

/**
  *@author Christian Zietz
  */

class PCSCDriver : public virtual AbstractDriver  {
private:
  SCARDCONTEXT hContext;
  ULONG numReaders;
public:
	PCSCDriver();
	virtual ~PCSCDriver();
  virtual ULONG getReaderCount();
  virtual AbstractReader *getReader(ULONG i);
  virtual DWORD getOptions();
  virtual ZCCRIRET setOptions(DWORD options);
};

#endif

