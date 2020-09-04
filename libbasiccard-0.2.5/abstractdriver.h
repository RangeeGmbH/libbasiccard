/***************************************************************************
                          abstractdriver.h  -  description
                             -------------------
    begin                : Fr Mär 26 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef ABSTRACTDRIVER_H
#define ABSTRACTDRIVER_H

#include <zccri.h>
#include "abstractreader.h"
/**
  *@author Christian Zietz
  */

class AbstractDriver {
public: 
	AbstractDriver() {};
  virtual ~AbstractDriver() {};
  virtual ULONG getReaderCount() = 0;
  virtual AbstractReader *getReader(ULONG i) = 0;
  virtual DWORD getOptions() = 0;
  virtual ZCCRIRET setOptions(DWORD options) = 0;  
};

#endif
