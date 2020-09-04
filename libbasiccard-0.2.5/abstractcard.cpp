/***************************************************************************
                          abstractcard.cpp  -  description
                             -------------------
    begin                : Sat Mar 27 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#include "winport.h"
#include "abstractcard.h"
#include <zcbci.h>

AbstractCard::AbstractCard(AbstractReader *r){
  reader = r;
}

AbstractCard::~AbstractCard(){
}

ZCCRIRET AbstractCard::transaction(CommandAPDU *cmd, ResponseAPDU *res){
  BYTE databuf[300];
  BYTE P3 = 0;

  if (reader==NULL) {
    return ZCCRI_ERROR_PARM;
  }

  ZCCRIRET rv = reader->transaction(cmd, res);

  // Le not accepted ?
  if ((res->getSW1SW2() & 0xff00) == 0x6c00) {
    cmd->setLe(res->getSW1SW2() & 0xff);
    cmd->setLeEnabled(true);
    reader->transaction(cmd, res);
  }

  // need to call GET RESPONSE ?
  if ((res->getSW1SW2() == 0x9000) && (res->getData(NULL)==0) && (cmd->isLeEnabled())) {
    P3 = cmd->getLe();
  }
  if (((res->getSW1SW2() & 0xff00) == 0x6100)
      && (res->getData(NULL)==0)) {
    P3 = res->getSW1SW2() & 0xff;
  }
  if (P3 > 0) {
    CommandAPDU *c = new CommandAPDU(0x00, 0xc0, 0x00, 0x00, 0x00, NULL, P3, true);
    ResponseAPDU *r = new ResponseAPDU;
    reader->transaction(c, r);
    r->getData(databuf);
    res->setData(databuf, r->getData(NULL));
  }

  SW1SW2 = res->getSW1SW2();
  return rv==ZCCRI_NOERROR?(isNoError()?ZCBCI_NOERROR:ZCBCI_ERROR_TRANS_FAILED):rv;
}

AbstractReader *AbstractCard::getReader(){
  return reader;
}

void AbstractCard::setReader(AbstractReader *param){
  reader = param;
}
