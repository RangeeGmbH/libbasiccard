/***************************************************************************
                          zccri.cpp  -  description
                             -------------------
    begin                : So Mär 28 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/

#include "winport.h"
#include <zccri.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pcscdriver.h"
#include "pcscreader.h"
#include "commandapdu.h"
#include "responseapdu.h"

// FIXME: hard-coded maximum number of readers: 8

PCSCDriver d;

#ifndef USE_DEBUGREADER
AbstractReader *r[8] = {d.getReader(0), d.getReader(1), d.getReader(2), d.getReader(3),
                        d.getReader(4), d.getReader(5), d.getReader(6), d.getReader(7)};
unsigned int n = (d.getReaderCount()<8)?d.getReaderCount():8;
#else

#include "debugdriver.h"

DebugDriver deb;
AbstractReader *debr = deb.getReader(0);
#define ifnull(x) ((x==NULL)?debr:x)

AbstractReader *r[9] = {ifnull(d.getReader(0)), ifnull(d.getReader(1)), ifnull(d.getReader(2)), ifnull(d.getReader(3)),
                        ifnull(d.getReader(4)), ifnull(d.getReader(5)), ifnull(d.getReader(6)), ifnull(d.getReader(7)),
                        debr};
unsigned int n = ((d.getReaderCount()<8)?d.getReaderCount():8) + ((deb.getReaderCount())?1:0);
#endif

// it's really "UNKOWN"
#define Check(x) (hReader < n)?(x):(ZCCRI_ERROR_UNKOWN_READER)

#ifndef ENVVAR
#define ENVVAR "CRIREADER"
#endif

#ifdef _WIN32

#define PERMSAVE_READER_EXISTS
#define GUI_DIALOG_EXISTS

#include "zccri_inc_win32.cpp"
  
#else // ifndef _WIN32

#ifndef CRI_CONFIG_PATH
#define CRI_CONFIG_PATH "/etc"
#endif

#ifndef CRI_CONFIG_FILE
#define CRI_CONFIG_FILE "crireader"
#endif

#define PERMSAVE_READER_EXISTS

#include "zccri_inc_unix.cpp"

#endif

unsigned int ZCCriIntReaderCount() {
  return n;
}

AbstractReader *ZCCriIntGetReader(ZCCRIREADER hReader) {
  return r[hReader];
}

int ZCCriIntFindReader(PZCCRIREADERNAME pName) {
  ULONG j; 
  ZCCRIREADERNAME d;
  
  if (pName!=NULL) {
    for (j=0; j<n; j++) {
      if (strcmp(pName, r[j]->getName())==0)
        break;
    }
    
    if (j==n) { // no readers found
      return -1;
    } else {
      return j;
    }
  } else if (ZCCriIntGetDefaultReader(d)) { // get default reader name
    return ZCCriIntFindReader(d); // check if it exists
  } else {
    return (n>0)?0:-1; // no default reader specified, first one (if existing) is default
  }
}    

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetReaderCount(PULONG pCount) {
  *pCount = n;
  return ZCCRI_NOERROR;
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetReaderName(ULONG ulNum, PZCCRIREADERNAME pName) {

  if (ulNum>=ZCCriIntReaderCount())
    return ZCCRI_ERROR_UNKOWN_READER;

  strncpy(pName, r[ulNum]->getName(), ZCCRI_MAX_READER_NAME);
  return ZCCRI_NOERROR;
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriOpenReader(PZCCRIREADER phReader, PZCCRIREADERNAME pName) {
  int j;

  j = ZCCriIntFindReader(pName);
  *phReader = (ZCCRIREADER)j;
  return (j==-1)?ZCCRI_ERROR_UNKOWN_READER:ZCCRI_NOERROR;
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriCloseReader(ZCCRIREADER hReader) {
  return Check(ZCCRI_NOERROR);
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriWaitCard(ZCCRIREADER hReader, LONG lTimeout) {
  return Check(r[hReader]->waitCard(lTimeout));
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriConnect(ZCCRIREADER hReader, PZCCRIATR pAtr) {
  return Check(r[hReader]->connect(pAtr));
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriReconnect(ZCCRIREADER hReader, PZCCRIATR pAtr) {
  return Check(r[hReader]->reconnect(pAtr));
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriDisconnect(ZCCRIREADER hReader) {
  return Check(r[hReader]->disconnect());
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriTransaction(ZCCRIREADER hReader, BYTE *pDataIn, DWORD cbDataIn, BYTE *pReply, DWORD *pcbReply) {
  ZCCRIRET rv;

  if (hReader>=n) {
    return ZCCRI_ERROR_UNKOWN_READER;
  }

  CommandAPDU cmd;
  ResponseAPDU res;

  cmd.unpackAPDU(pDataIn, cbDataIn);
  rv = r[hReader]->transaction(&cmd, &res);

  if (res.packAPDU(NULL)> *pcbReply) {
    return ZCCRI_ERROR_OVERFLOW;
  } else {
    *pcbReply = res.packAPDU(pReply);
    return rv;
  }
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriTransaction2(ZCCRIREADER hReader, BYTE Cla, BYTE Ins,
                                              BYTE P1, BYTE P2, BYTE Lc, BYTE *pData,
                                              SHORT Le, BYTE *pReply, SHORT *pcbReply, WORD *pSW1SW2) {
  ZCCRIRET rv;

  if (hReader>=n) {
    return ZCCRI_ERROR_UNKOWN_READER;
  }

  CommandAPDU cmd(Cla, Ins, P1, P2, Lc, pData, (BYTE)Le, (Le==-1)?false:true);
  ResponseAPDU res;

  rv = r[hReader]->transaction(&cmd, &res);

  if (res.getData(NULL)> *pcbReply) {
    return ZCCRI_ERROR_OVERFLOW;
  } else {
    *pcbReply = res.getData(pReply);
    *pSW1SW2 = res.getSW1SW2();
    return rv;
  }
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriSetLogFile(ZCCRIREADER hReader, CHAR *pszFile) {
  return Check(r[hReader]->setLogFile(pszFile));
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriRegisterService(CHAR *pszService, CHAR *pszDLLName) {
  return ZCCRI_ERROR_NOT_SUPPORTED; // not supported
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriDeregisterService(CHAR *pszService) {
  return ZCCRI_ERROR_NOT_SUPPORTED; // not supported
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriSetDefaultReader(PZCCRIREADERNAME pName) {
  // FIXME: this is only stored permanently if ZCCriIntPermSaveDefaultReader
  //        exists for current platform
  if (ZCCriIntFindReader(pName) != -1) {
    setenv(ENVVAR, pName, 1);
    #ifdef PERMSAVE_READER_EXISTS
    ZCCriIntPermSaveDefaultReader(pName);
    #endif  
    return ZCCRI_NOERROR;
  } else {
    return ZCCRI_ERROR_UNKOWN_READER;
  }    
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetDefaultReader(PZCCRIREADERNAME pName) {
  int j;

  j = ZCCriIntFindReader(NULL);
  if (j!=-1) {
    strncpy(pName, r[j]->getName(), ZCCRI_MAX_READER_NAME);
    return ZCCRI_NOERROR;
  } else {
    return ZCCRI_ERROR_NO_DEFAULT;
  }
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriSelectReaderDialog(PZCCRIREADERNAME pName) {
  // FIXME: the text mode menu is ugly
  
  #if (defined(GUI_DIALOG_EXISTS) && !defined(USE_TEXTMENU))
  
  // display nice graphical selection dialog (currently only for Windows)
  return ZCCriIntGuiSelectReaderDialog(pName);
  
  #else
  
  unsigned int i;

  printf("\nSelect smartcard reader:\n");
  for (i=0; i<n; i++) {
    printf("%3d: %.40s\n", i+1, r[i]->getName());
  }
  printf("%3d: cancel\n", 0);

  do {
    i = getchar()-'0';
  } while ((i<0)||(i>n));

  if (i!=0) {
    strncpy(pName, r[i-1]->getName(), ZCCRI_MAX_READER_NAME);
    return ZCCRI_NOERROR;
  } else {
    return ZCCRI_CANCEL;
  }
  
  #endif
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetFeatures(PZCCRIREADERNAME pName, DWORD *pdwFeatures) {
  int j;

  j = ZCCriIntFindReader(pName);
  if (j!=-1) {
    *pdwFeatures = r[j]->getFeatures();
    return ZCCRI_NOERROR;
  } else {
    return ZCCRI_ERROR_UNKOWN_READER;
  }
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriCardInReader(PZCCRIREADERNAME pName, PZCCRIATR pAtr) {
  int j;

  j = ZCCriIntFindReader(pName);
  if (j!=-1) {
    return r[j]->cardInReader(pAtr);
  } else {
    return ZCCRI_ERROR_UNKOWN_READER;
  }
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriSetOptions(ZCCRIREADER hReader, DWORD dwOptions) {
  return Check(r[hReader]->setOptions(dwOptions));
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetOptions(ZCCRIREADER hReader, DWORD *pdwOptions) {
  if (hReader<n) {
    *pdwOptions = r[hReader]->getOptions();
    return ZCCRI_NOERROR;
  } else {
    return ZCCRI_ERROR_UNKOWN_READER;
  }
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriSetOptions2(PZCCRISERVICENAME pServiceName, DWORD dwOptions) {
  return d.setOptions(dwOptions);
}

ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetOptions2(PZCCRISERVICENAME pServiceName, DWORD *pdwOptions) {
  *pdwOptions = d.getOptions();
  return ZCCRI_NOERROR;
}    
