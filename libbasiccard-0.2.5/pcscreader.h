/***************************************************************************
                          pcscreader.h  -  description
                             -------------------
    begin                : Fr Mär 26 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef PCSCREADER_H
#define PCSCREADER_H

#include "winport.h"
#include "abstractreader.h"
#include "commandapdu.h"
#include "responseapdu.h"
#include <winscard.h>

/**
  *@author Christian Zietz
  */

class PCSCReader : public virtual AbstractReader  {
private:
  SCARDCONTEXT hContext;
  char *name;
  char *extname;
  DWORD activeProtocol;
  SCARDHANDLE hCard;
public:
	PCSCReader(SCARDCONTEXT c, char *n);
	virtual ~PCSCReader();
  virtual char * getName();
  virtual ZCCRIRET waitCard(LONG timeout);
  virtual ZCCRIRET connect(PZCCRIATR pAtr);
  virtual ZCCRIRET reconnect(PZCCRIATR pAtr);
  virtual ZCCRIRET disconnect();
  virtual ZCCRIRET transaction(CommandAPDU *cmd, ResponseAPDU *res);
  virtual DWORD getFeatures();
  virtual ZCCRIRET cardInReader(PZCCRIATR pAtr);
  virtual DWORD getOptions();
  virtual ZCCRIRET setOptions(DWORD options);
};

#endif

