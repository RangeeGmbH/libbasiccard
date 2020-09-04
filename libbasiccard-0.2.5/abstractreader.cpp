/***************************************************************************
                          abstractreader.cpp  -  description
                             -------------------
    begin                : Fr Mär 26 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#include "winport.h"
#include "abstractreader.h"
#include <stdlib.h>

AbstractReader::AbstractReader() {
  logFile = NULL;
}

void AbstractReader::logData(const char* trailer, BYTE *data, BYTE datalen){
  BYTE i;

  if (logFile!=NULL) {
    fprintf(logFile, "%s ", trailer);
    for (i=0; i<datalen; i++)
      fprintf(logFile, "%.2x ", data[i]);
    fprintf(logFile, "\n");
  }
}

ZCCRIRET AbstractReader::setLogFile(char *pFile){
  if (logFile!=NULL)
    fclose(logFile);

  if (pFile!=NULL) {
    logFile = fopen(pFile, "w");
    return (logFile!=NULL)?ZCCRI_NOERROR:ZCCRI_ERROR_MISC;
  } else {
    logFile = NULL;
    return ZCCRI_NOERROR;
  }
}
