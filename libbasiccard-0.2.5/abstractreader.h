/***************************************************************************
                          abstractreader.h  -  description
                             -------------------
    begin                : Fr Mär 26 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef ABSTRACTREADER_H
#define ABSTRACTREADER_H

#include <zccri.h>
#include <winscard.h>
#include <stdio.h>
#include "commandapdu.h"
#include "responseapdu.h"

/**
  *@author Christian Zietz
  */

class AbstractReader {
protected:
  FILE *logFile;
  virtual void logData(const char *trailer, BYTE *data, BYTE datalen);
public: 
	AbstractReader();
  virtual ~AbstractReader() {};
  virtual char * getName() = 0;
  virtual ZCCRIRET waitCard(LONG timeout) = 0;
  virtual ZCCRIRET connect(PZCCRIATR pAtr) = 0;
  virtual ZCCRIRET reconnect(PZCCRIATR pAtr) = 0;
  virtual ZCCRIRET disconnect() = 0;
  virtual ZCCRIRET transaction(CommandAPDU *cmd, ResponseAPDU *res) = 0;
  virtual ZCCRIRET setLogFile(char *pFile);
  virtual DWORD getFeatures() = 0;
  virtual ZCCRIRET cardInReader(PZCCRIATR pAtr) = 0;
  virtual DWORD getOptions() = 0;
  virtual ZCCRIRET setOptions(DWORD options) = 0;
};

#endif
