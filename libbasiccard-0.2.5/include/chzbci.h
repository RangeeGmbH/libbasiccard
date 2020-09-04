/***************************************************************************
                          chzbci  -  description
                             -------------------
    begin                : Sa Sep 18 2004
    copyright            : (C) 2004 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/

#ifndef __CHZBCI_H__
#define __CHZBCI_H__

#define CHZBCI_VER 0x025

#include <zcbci.h>

#ifdef __cplusplus
extern "C" {
#endif

/* (new) SW1SW2 values */
#ifdef ZCBCI_INCL_SW
#define swBadSignature              0x66C6
#define swBadAuthenticate           0x66C7
#define swBadComponentName          0x69C0
#define swComponentNotFound         0x69C1
#define swAccessDenied              0x69C2
#define swComponentAlreadyExists    0x69C3
#define swBadComponentChain         0x69C4
#define swNameTooLong               0x69C5
#define swOutOfMemory               0x69C6
#define swInvalidACR                0x69C7
#define swBadComponentType          0x69C8
#define swKeyUsage                  0x69CD
#define swKeyAlgorithm              0x69CE
#define swTooManyTempFlags          0x69D0
#define swExecutableAcrDenied       0x69D1
#define swApplicationNotFound       0x69D2
#define swACRDepth                  0x69D3
#define swBadComponentAttr          0x69D4
#define swBadComponentData          0x69D5
#define swBadAppFile                0x69D6
#define swLoadSequenceActive        0x69D7
#define swLoadSequenceNotActive     0x69D8
#define swLoadSequencePhase         0x69D9
#define swBadEaxTag                 0x69DC
#define swSecureTransportActive     0x69DD
#define swSecureTransportInactive   0x69DE
#define swComponentReferenced       0x69DF
#define swFileNotContiguous         0x69E0
#define swAppFileOpen               0x69E1
#endif

/* encryption algorithms */
#define CHZBCI_AES128 0x31
#define CHZBCI_AES192 0x32
#define CHZBCI_AES256 0x33

#define CHZBCI_EAX_AES128 0x41
#define CHZBCI_EAX_AES192 0x42
#define CHZBCI_EAX_AES256 0x43

#define CHZBCI_OMAC_AES128 0x81
#define CHZBCI_OMAC_AES192 0x82
#define CHZBCI_OMAC_AES256 0x83

#define CHZBCI_EC167 0xc1

/* component types */
#define CHZBCI_COMP_FILE 0x10
#define CHZBCI_COMP_ACR 0x20
#define CHZBCI_COMP_PRIVILEGE 0x30
#define CHZBCI_COMP_FLAG 0x40
#define CHZBCI_COMP_KEY 0x70

/* key usage and algorithm specification */
#define CHZBCI_KU_VERIFY_MASK 0x0001
#define CHZBCI_KU_EXTAUTH_MASK 0x0002
#define CHZBCI_KU_SMENC_MASK 0x0004
#define CHZBCI_KU_SMMAC_MASK 0x0008
#define CHZBCI_KU_SIGN_MASK 0x0010
#define CHZBCI_KU_INTAUTH_MASK 0x0020
#define CHZBCI_KU_SECTRANS_MASK 0x0040

#define CHZBCI_DES_CRC_MASK 0x0001
#define CHZBCI_3DES_CRC_MASK 0x0002
#define CHZBCI_AES128_MASK 0x0004
#define CHZBCI_AES192_MASK 0x0008
#define CHZBCI_AES256_MASK 0x0010
#define CHZBCI_EAX_AES128_MASK 0x0020
#define CHZBCI_EAX_AES192_MASK 0x0040
#define CHZBCI_EAX_AES256_MASK 0x0080
#define CHZBCI_OMAC_AES128_MASK 0x0100
#define CHZBCI_OMAC_AES192_MASK 0x0200
#define CHZBCI_OMAC_AES256_MASK 0x0400
#define CHZBCI_EC167_MASK 0x0800

/* misc. constants */
#define CHZBCI_LOADSEQUENCE_START 1
#define CHZBCI_LOADSEQUENCE_END   2
#define CHZBCI_LOADSEQUENCE_ABORT 3

#define CHZBCI_FLAG_PERMANENT 0x02
#define CHZBCI_FLAG_CLEAR_NEW_APP 0x04
#define CHZBCI_FLAG_CLEAR_COMMAND 0x08

/* component attributes and easy access macros */

#define bci_getword(x) ((x##_HI << 8) | (x##_LO))
#define bci_setword(x,y) {x##_HI = (y >> 8) & 0xff; x##_LO = y & 0xff; }

#pragma pack(1)

typedef struct _CHZBCI_DIRECTORY_CREATE_ATTR {
  BYTE acrcid_HI, acrcid_LO;
  BYTE attributes;
} CHZBCI_DIRECTORY_CREATE_ATTR;

typedef struct _CHZBCI_DATAFILE_CREATE_ATTR {
  BYTE acrcid_HI, acrcid_LO;
  BYTE attributes;
  BYTE blocklen_HI, blocklen_LO;
} CHZBCI_DATAFILE_CREATE_ATTR;

typedef struct _CHZBCI_DIRECTORY_WRITE_ATTR {
  BYTE acrcid_HI, acrcid_LO;
} CHZBCI_DIRECTORY_WRITE_ATTR;

typedef CHZBCI_DIRECTORY_WRITE_ATTR CHZBCI_DATAFILE_WRITE_ATTR;

typedef CHZBCI_DIRECTORY_CREATE_ATTR CHZBCI_DIRECTORY_READ_ATTR;

typedef struct _CHZBCI_DATAFILE_READ_ATTR {
  BYTE acrcid_HI, acrcid_LO;
  BYTE attributes;
  BYTE blocklen_HI, blocklen_LO;
  BYTE filelen_HI, filelen_LO;
} CHZBCI_DATAFILE_READ_ATTR;

typedef struct _CHZBCI_ACR_CREATE_ATTR {
  BYTE acrcid_HI, acrcid_LO;
  BYTE acrtype;
} CHZBCI_ACR_CREATE_ATTR;

typedef CHZBCI_DIRECTORY_WRITE_ATTR CHZBCI_ACR_WRITE_ATTR;

typedef CHZBCI_ACR_CREATE_ATTR CHZBCI_ACR_READ_ATTR;

typedef CHZBCI_DIRECTORY_WRITE_ATTR CHZBCI_PRIVILEGE_ATTR;

typedef CHZBCI_DIRECTORY_CREATE_ATTR CHZBCI_FLAG_ATTR;

typedef struct _CHZBCI_KEY_ATTR {
  BYTE acrcid_HI, acrcid_LO;
  BYTE usagemask_HI, usagemask_LO;
  BYTE algorithmmask_HI, algorithmmask_LO;
  BYTE errorcounter;
  BYTE ecresetvalue;
} CHZBCI_KEY_ATTR;

#pragma pack()

/* functions */

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciStartMultiAppEncryption(ZCBCICARD card, WORD *pSW1SW2, BYTE *pAlgo, BYTE key,
                                                          WORD keyCID) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciFindComponent(ZCBCICARD card, WORD *pSW1SW2, BYTE type, const CHAR *pszPath,
                                                WORD *pCID) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciSelectApplication(ZCBCICARD card, WORD *pSW1SW2, const CHAR *pszPath) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciExternalAuthenticate(ZCBCICARD card, WORD *pSW1SW2, BYTE algo, BYTE key,
                                                       WORD keyCID) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciInternalAuthenticate(ZCBCICARD card, WORD *pSW1SW2, BYTE algo, BYTE key,
                                                      WORD keyCID) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciVerify(ZCBCICARD card, WORD *pSW1SW2, WORD keyCID, const CHAR *pszPassword) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciGrantPrivilege(ZCBCICARD card, WORD *pSW1SW2, WORD prvCID, const CHAR *pszPath) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciGetExtendedID(ZCBCICARD card, WORD *pSW1SW2,
                                                BYTE index, BYTE type, CHAR *pszID, BYTE cbID) ZCEXPD

/* new functions since libbasiccard 0.2.0 */

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciGetFreeMemory(ZCBCICARD card, WORD *pSW1SW2, WORD *pTotal, WORD *pBlock) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciComponentName(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID, CHAR *pszName, BYTE cbName) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciCreateComponent(ZCBCICARD card, WORD *pSW1SW2, BYTE type, const CHAR *pszPath,
                                                  const void *pAttr, BYTE cbAttr,
                                                  const void *pData, BYTE cbData, WORD *pCID) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciDeleteComponent(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciWriteComponentAttr(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID,
                                                     const void *pAttr, BYTE cbAttr) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciReadComponentAttr(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID,
                                                    void *pAttr, BYTE *pcbAttr) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciWriteComponentData(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID,
                                                     const void *pData, BYTE cbData) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciReadComponentData(ZCBCICARD card, WORD *pSW1SW2, WORD cmpCID,
                                                    void *pData, BYTE *pcbData) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciAuthenticateFile(ZCBCICARD card, WORD *pSW1SW2, WORD keyCID,
                                                   BYTE algo, const CHAR *pszPath, const void *pSignature) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciReadRightsList(ZCBCICARD card, WORD *pSW1SW2, const CHAR *pszPath,
                                                 WORD *pList, BYTE *pcbList) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciLoadSequence(ZCBCICARD card, WORD *pSW1SW2, BYTE phase) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciSecureTransport(ZCBCICARD card, WORD *pSW1SW2, WORD keyCID,
                                                  BYTE algo, const void *pNonce, BYTE cbNonce) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciGetLibVersion(DWORD *pVer) ZCEXPD

ZCDLLEXP ZCBCIRET ZCBCILINK CHZBciGetKey(ZCBCICARD card, BYTE keynum, BYTE *pKey, BYTE *pcbKey) ZCEXPD

#ifdef __cplusplus
}
#endif

#endif
