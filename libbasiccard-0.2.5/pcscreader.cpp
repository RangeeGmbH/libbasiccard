/***************************************************************************
                          pcscreader.cpp  -  description
                             -------------------
    begin                : Fr Mär 26 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#include "winport.h"
#include "pcscreader.h"
#include <cstring>

PCSCReader::PCSCReader(SCARDCONTEXT c, char *n) : AbstractReader(){
  hContext = c;
  hCard = 0;
  name = new char[strlen(n)+1];
  extname = new char[strlen(n)+6];
  strcpy(name, n);
  strcpy(extname, "PCSC:");
  strcpy(extname+5, n);
}

PCSCReader::~PCSCReader(){
  disconnect();
  delete []name;
  delete []extname;
}

char * PCSCReader::getName() {
#ifndef USE_OLDNAMES
  return extname;
#else
  return name;
#endif
}

ZCCRIRET PCSCReader::waitCard(LONG timeout) {
/* now including fix for nasty bug using SCR reader under Windows */
  ZCCRIRET rv;
  SCARD_READERSTATE_A readerstate[1];

  if (timeout==0)
    timeout=1; // 0 is none for CRI but possibly infinite for PC/SC
  if (timeout==-1)
    timeout=INFINITE; // -1 is infinite for CRI

  readerstate[0].szReader = name;
  readerstate[0].dwCurrentState = SCARD_STATE_UNAWARE;
  SCardGetStatusChange(hContext, 1, readerstate, 1);

  readerstate[0].szReader = name;
  readerstate[0].dwCurrentState = readerstate[0].dwEventState | SCARD_STATE_EMPTY;
  rv = SCardGetStatusChange(hContext, timeout, readerstate, 1);

  if (rv == SCARD_E_TIMEOUT) {
    return ZCCRI_ERROR_NO_CARD;
  } else if (rv != SCARD_S_SUCCESS) {
    return ZCCRI_ERROR_PARM;
  } else {
    return (readerstate[0].dwEventState & SCARD_STATE_EMPTY)?ZCCRI_ERROR_NO_CARD:ZCCRI_NOERROR;
  }
}

ZCCRIRET PCSCReader::connect(PZCCRIATR pAtr) {

  ZCCRIRET rv;
  DWORD dlen, dprot, state, atrlen;
  ZCCRIATR atrbuf;
  ZCCRIREADERNAME dummy;

  rv = SCardConnect(hContext, name, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_ANY, &hCard, &activeProtocol);
  if (rv == SCARD_E_NO_SMARTCARD) {
    return ZCCRI_ERROR_NO_CARD;
  } else if (rv != SCARD_S_SUCCESS) {
    return ZCCRI_ERROR_READER_BUSY;
  } else {
    dlen = sizeof(dummy);
    dprot = SCARD_PROTOCOL_ANY;
    atrlen = sizeof(atrbuf.abAtr);
    SCardStatus(hCard, dummy, &dlen, &state, &dprot, (LPBYTE)&(atrbuf.abAtr), &atrlen);
    atrbuf.bLength = (BYTE)atrlen;
    atrbuf.abAtr[atrbuf.bLength] = 0;
    if (pAtr != NULL)
      memcpy(pAtr, &atrbuf, sizeof(atrbuf));

    logData("ATR:",atrbuf.abAtr,(BYTE)atrlen);

    return ((activeProtocol>SCARD_PROTOCOL_RAW)||(activeProtocol<SCARD_PROTOCOL_T0))?ZCCRI_ERROR_NO_CARD:ZCCRI_NOERROR;
  }
}

ZCCRIRET PCSCReader::reconnect(PZCCRIATR pAtr) {
  ZCCRIRET rv;
  DWORD dlen, dprot, state, atrlen;
  ZCCRIATR atrbuf;
  ZCCRIREADERNAME dummy;

  if (hCard==0)
    return ZCCRI_ERROR_PARM;

  rv = SCardReconnect(hCard, SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_ANY, SCARD_RESET_CARD, &activeProtocol);

  if (rv == SCARD_E_NO_SMARTCARD) {
    return ZCCRI_ERROR_NO_CARD;
  } else if (rv != SCARD_S_SUCCESS) {
    return ZCCRI_ERROR_READER_BUSY;
  } else {
    dlen = sizeof(dummy);
    dprot = SCARD_PROTOCOL_ANY;
    atrlen = sizeof(atrbuf.abAtr);    
    SCardStatus(hCard, dummy, &dlen, &state, &dprot, (LPBYTE)&(atrbuf.abAtr), &atrlen);
    atrbuf.bLength = (BYTE)atrlen;
    atrbuf.abAtr[atrbuf.bLength] = 0;
    if (pAtr != NULL)
      memcpy(pAtr, atrbuf.abAtr, sizeof(atrbuf));

    logData("ATR:",(BYTE *)&atrbuf,(BYTE)atrlen);

    return ((activeProtocol>SCARD_PROTOCOL_RAW)||(activeProtocol<SCARD_PROTOCOL_T0))?ZCCRI_ERROR_NO_CARD:ZCCRI_NOERROR;
  }
}

ZCCRIRET PCSCReader::disconnect() {
  LONG rv;

  if (hCard==0)
    return ZCCRI_ERROR_PARM;

  rv = SCardDisconnect(hCard, SCARD_UNPOWER_CARD);
  hCard = 0;

  return ZCCRI_NOERROR;
}

DWORD PCSCReader::getFeatures() {
  return 0;
}

ZCCRIRET PCSCReader::cardInReader(PZCCRIATR pAtr) {
  return ZCCRI_ERROR_NOT_SUPPORTED;
}

DWORD PCSCReader::getOptions() {
  return 0;
}

ZCCRIRET PCSCReader::setOptions(DWORD options) {
  return ZCCRI_ERROR_NOT_SUPPORTED;
}

ZCCRIRET PCSCReader::transaction(CommandAPDU *cmd, ResponseAPDU *res) {
  BYTE inlen;
  BYTE *inbuf;
  DWORD outlen;
  BYTE *outbuf;
  LONG rv;

  inlen = cmd->packAPDU(NULL);
  inbuf = new BYTE[inlen];
  cmd->packAPDU(inbuf);
  
  if ((activeProtocol==SCARD_PROTOCOL_T0) && (cmd->getLc()) && (cmd->isLeEnabled())) {
    // with the T=0 protocol either Lc or Le can be transmitted; so skip Le, if Lc is being sent
    inlen--; 
  }

  logData("->",inbuf,inlen);

  outlen = 512;
  outbuf = new BYTE[outlen];

  if (hCard==0)
    return ZCCRI_ERROR_PARM;

  rv = SCardTransmit(hCard, (activeProtocol==SCARD_PROTOCOL_T0)?SCARD_PCI_T0:SCARD_PCI_T1, inbuf, inlen, NULL, outbuf, &outlen);
  delete []inbuf;

  switch (rv) {
  case SCARD_S_SUCCESS:
    res->unpackAPDU(outbuf, outlen);
    logData("<-",outbuf,outlen);
    delete []outbuf;
    return ZCCRI_NOERROR;
    break;
  case SCARD_E_PROTO_MISMATCH:
    delete []outbuf;
    return ZCCRI_ERROR_PROTOCOL;
    break;
  case SCARD_W_REMOVED_CARD:
  case SCARD_E_NO_SMARTCARD:
    delete []outbuf;
    return ZCCRI_ERROR_NO_CARD;
    break;
  case SCARD_E_INSUFFICIENT_BUFFER:
    delete []outbuf;
    return ZCCRI_ERROR_OVERFLOW;
  default:
    delete []outbuf;
    return ZCCRI_ERROR_PARM;
    break;
  }
}
