/***************************************************************************
                          basiccard.h  -  description
                             -------------------
    begin                : Sat Mar 27 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/


#ifndef BASICCARD_H
#define BASICCARD_H

#include "abstractcard.h"
#include "abstractcrypt.h"
#include <zcbci.h>
#include <chzbci.h>

/**
  *@author Christian Zietz
  */

class BasicCard : public virtual AbstractCard  {
protected:
  AbstractCrypt *crypt;
  BYTE keydata[256][32];
  BYTE keylens[256];
  BYTE algo;
  BYTE fileErr;
public: 
  BasicCard(AbstractReader *r);
  virtual ~BasicCard();

  virtual BYTE getAlgo() { return algo; };
  virtual BYTE getFileErr() { return fileErr; };
  
  virtual  ZCCRIRET setKey(BYTE key, BYTE *keydata, BYTE keylen);
  virtual  ZCCRIRET startEncryption(BYTE algo, BYTE key, WORD keyCID);
  virtual ZCCRIRET endEncryption();
  
  virtual  ZCCRIRET transaction(CommandAPDU *cmd, ResponseAPDU *res);

  // predefined commands
  virtual ZCCRIRET getState(BYTE *state, BYTE *version);
  virtual ZCCRIRET eepromSize(WORD *pStart, WORD *pLength);
  virtual ZCCRIRET clearEeprom(WORD start, WORD length);
  virtual ZCCRIRET writeEeprom(WORD start, BYTE *pData, BYTE cbData);
  virtual ZCCRIRET readEeprom(WORD start, BYTE *pData, BYTE cbData);
  virtual ZCCRIRET eepromCRC(WORD start, WORD length, WORD *pCRC);
  virtual ZCCRIRET setState(BYTE state);
  virtual ZCCRIRET getApplicationID(BYTE index, BYTE type, CHAR *pszID, BYTE cbID);
  virtual ZCCRIRET echo(BYTE inc, CHAR *pszEcho);
  
  virtual ZCCRIRET getKey(BYTE key, BYTE *keydata, BYTE *pkeylen);
  
  // MultiApplication commands
  virtual ZCCRIRET findComponent(BYTE type, const CHAR *pszPath, WORD *pCID);
  virtual ZCCRIRET selectApplication(const CHAR *pszPath);
  virtual ZCCRIRET externalAuthenticate(BYTE algo, BYTE key, WORD keyCID);
  virtual ZCCRIRET verify(WORD keyCID, const CHAR *pszPassword);
  virtual ZCCRIRET grantPrivilege(WORD prvCID, const CHAR *pszPath);
  virtual ZCCRIRET internalAuthenticate(BYTE algo, BYTE key, WORD keyCID);
  virtual ZCCRIRET getFreeMemory(WORD *pTotal, WORD *pBlock);
  virtual ZCCRIRET componentName(WORD cmpCID, CHAR *pszName, BYTE cbName);
  
  virtual ZCCRIRET createComponent(BYTE type, const CHAR *pszPath, const void *pAttr, BYTE cbAttr,
                                    const void *pData, BYTE cbData, WORD *pCID);
  virtual ZCCRIRET deleteComponent(WORD cmpCID);
  virtual ZCCRIRET writeComponentAttr(WORD cmpCID, const void *pAttr, BYTE cbAttr);
  virtual ZCCRIRET readComponentAttr(WORD cmpCID, void *pAttr, BYTE *pcbAttr);
  virtual ZCCRIRET writeComponentData(WORD cmpCID, const void *pData, BYTE cbData);
  virtual ZCCRIRET readComponentData(WORD cmpCID, void *pData, BYTE *pcbData);  
  virtual ZCCRIRET authenticateFile(WORD keyCID, BYTE algo, const CHAR *pszPath, const void *pSignature);
  virtual ZCCRIRET readRightsList(const CHAR *pszPath, WORD *pList, BYTE *pcbList);
  
  virtual ZCCRIRET loadSequence(BYTE phase);
  virtual ZCCRIRET secureTransport(WORD keyCID, BYTE algo, const void *pNonce, BYTE cbNonce);

  // file I/O commands
  virtual ZCCRIRET mkDir(const CHAR *pszPath);
  virtual ZCCRIRET rmDir(const CHAR *pszPath);
  virtual ZCCRIRET chDir(const CHAR *pszPath);
  virtual ZCCRIRET curDir(CHAR *pszPath, BYTE cbPath);
  virtual ZCCRIRET rename(const CHAR *pszOldPath, const CHAR *pszNewPath);
  virtual ZCCRIRET dirCount(const CHAR *pszFileSpec, WORD *pCount);
  virtual ZCCRIRET dirFile(const CHAR *pszFileSpec, WORD num, CHAR *pszFile, BYTE cbFile);
  virtual ZCCRIRET getAttr(const CHAR *pszPath, WORD *pAttrib);
  virtual ZCCRIRET eraseFile(const CHAR *pszPath);
  virtual ZCCRIRET openFile(const CHAR *pszPath, BYTE mode, WORD recordlen, WORD *pFileNum);
  virtual ZCCRIRET closeFile(WORD FileNum);
  virtual ZCCRIRET closeAllFiles();
  virtual ZCCRIRET fileLength(WORD FileNum, DWORD *pFileLength);
  virtual ZCCRIRET getFilepos(WORD FileNum, DWORD *pFilePos);
  virtual ZCCRIRET setFilepos(WORD FileNum, DWORD FilePos);
  virtual ZCCRIRET queryEof(WORD FileNum, BOOL *pfEof);
  virtual ZCCRIRET fileGet(WORD FileNum, BYTE *pData, BYTE cbData);
  virtual ZCCRIRET fileGetPos(WORD FileNum, DWORD pos, BYTE *pData, BYTE cbData);
  virtual ZCCRIRET filePut(WORD FileNum, const BYTE *pData, BYTE cbData);
  virtual ZCCRIRET filePutPos(WORD FileNum, DWORD pos, const BYTE *pData, BYTE cbData);
  virtual ZCCRIRET fileRead(WORD FileNum, BYTE *pData, BYTE cbData);
  virtual ZCCRIRET fileReadLong(WORD FileNum, LONG *plVar);
  virtual ZCCRIRET fileReadSingle(WORD FileNum, float *pflVar);
  virtual ZCCRIRET fileReadString(WORD FileNum, CHAR *pszVar, BYTE cbVar);
  virtual ZCCRIRET fileReadUser(WORD FileNum, BYTE *pData, BYTE *pcbData);
  virtual ZCCRIRET fileReadUser2(WORD FileNum, BYTE *pData, BYTE cbData);
  virtual ZCCRIRET fileReadLine(WORD FileNum, CHAR *pszLine, BYTE cbLine);
  virtual ZCCRIRET fileWriteLong(WORD FileNum, LONG lVal);
  virtual ZCCRIRET fileWriteSingle(WORD FileNum, float flVal);
  virtual ZCCRIRET fileWriteUser(WORD FileNum, const BYTE *pData, BYTE cbData);
  virtual ZCCRIRET fileWriteString(WORD FileNum, const CHAR *pszVal);
  virtual ZCCRIRET fileWrite(WORD FileNum, const BYTE *pData, BYTE cbData);
  virtual ZCCRIRET filePrint(WORD FileNum, const CHAR *pszVal);
  virtual ZCCRIRET getLockInfo(const CHAR *pszPath, PZCBCILOCKINFO pInfo);
  virtual ZCCRIRET setFileLock(const CHAR *pszPath, ZCBCILOCK type, BOOL fLock, BYTE KeyCount, BYTE Key1, BYTE Key2);

};

#endif
