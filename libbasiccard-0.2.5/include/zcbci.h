/******************************************************************************/
/* Copyright      : ZeitControl cardsystems GmbH 1999                         */
/* Author         : Michael Petig                                             */
/* Modified by    : Christian Zietz for libbasiccard                          */      
/******************************************************************************/
/* Project        : BasicCard Command Interface                               */
/* Description    : Global Header                                             */
/* History        : 19990219 MP initial version                               */
/*                  19990420 MP add enhance support                           */
/*                  19990906 MP add ZCBCIOCXSTRING support                    */
/*                  20000118 MP add ZCBCISINGLE                               */
/*                  20000128 MP add 0x7FFFFFFF value to enums force use of 32 */
/*                              bit (solve problem with borland c builder)    */
/*                  20000614 MP add support for creating import libraries     */
/*                  20011203 MP add support for Professional BasicCard ZC4.1  */
/******************************************************************************/

#ifndef __ZCBCI_H__
#define __ZCBCI_H__

#include <zccri.h>

#ifndef ZCBCI_CARDVER
#define ZCBCI_CARDVER 401
#endif

#define ZCBCI_COMPACT
#if (ZCBCI_CARDVER>=200)
#define ZCBCI_ENHANCED
#endif
#if (ZCBCI_CARDVER>=400)
#define ZCBCI_PROFESSIONAL
#endif

#ifndef ZCBCI_NOINCL_ALL
#define ZCBCI_INCL_ALL
#endif

#ifdef ZCBCI_INCL_ALL
#define ZCBCI_INCL_FA
#define ZCBCI_INCL_SW
#define ZCBCI_INCL_FE
#define ZCBCI_INCL_LI
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* typedefs */
#pragma pack(1)

#ifndef INT
  typedef int INT;
  typedef float FLOAT;
#endif

typedef DWORD ZCBCIRET;
typedef ZCBCIRET *PZCBCIRET;
typedef DWORD ZCBCIHANDLE;
typedef ZCBCIHANDLE *PZCBCIHANDLE;
typedef ZCBCIHANDLE ZCBCICARD;
typedef ZCBCICARD *PZCBCICARD;
typedef enum _ZCBCIPARMTYPE
{
	ZCBCIBYTE,
	ZCBCIINT,
	ZCBCILONG,
	ZCBCISTRING,
	ZCBCISTRINGN,
	ZCBCIUSER,
	ZCBCIRAW,       /* if used must be the one and only parameter */
	ZCBCIOCXSTRING, /* used by basiccard activeX control */
    ZCBCINULL,      /* NULL/empty parameter */
    ZCBCISINGLE,
	ZCBCIBINSTRING,
	ZCBCIBINSTRINGN,
	ZCBCIDUMMY=0x7FFFFFFF /* make it use 32 bits */
} ZCBCIPARMTYPE;
typedef BYTE  ZCBCIPARMBYTE;
typedef ZCBCIPARMBYTE *PZCBCIPARMBYTE;
typedef INT   ZCBCIPARMINT;
typedef ZCBCIPARMINT *PZCBCIPARMINT;
typedef LONG  ZCBCIPARMLONG;
typedef ZCBCIPARMLONG *PZCBCIPARMLONG;
typedef FLOAT ZCBCIPARMSINGLE;
typedef ZCBCIPARMSINGLE *PZCBCIPARMSINGLE;
typedef struct _ZCBCIPARMSTRING
{
	BYTE cbString;  /* BYTE size pString is able to hold (including '\0') */
	CHAR *pString;  /* '\0' terminated string */
} ZCBCIPARMSTRING;
typedef ZCBCIPARMSTRING *PZCBCIPARMSTRING;
typedef struct _ZCBCIPARMSTRINGN
{
	BYTE bLength;   /* Length of string equals N */
	CHAR *pString;  /* '\0' terminated string */
} ZCBCIPARMSTRINGN;
typedef ZCBCIPARMSTRINGN *PZCBCIPARMSTRINGN;
typedef struct _ZCBCIPARMUSER
{
	BYTE bSize;     /* sizeof user type data */
	BYTE *pData;    /* user type data */
} ZCBCIPARMUSER;
typedef ZCBCIPARMUSER *PZCBCIPARMUSER;
typedef struct _ZCBCIPARMRAW
{
	BYTE bSize;     /* sizeof raw data buffer */
	BYTE bLen;      /* (in) length of send data, (out) length of receive data */
	BYTE *pData;    /* raw data buffer */
} ZCBCIPARMRAW;
typedef struct _ZCBCIPARMOCXSTRING
{
	BYTE bLength;   /* (in/out) Length of string equals N */
	BYTE *pString;  /* Pointer to byte array of at least 255 bytes */
} ZCBCIPARMOCXSTRING;
typedef struct _ZCBCIPARMBINSTRING
{
	BYTE bSize;     /* sizeof data buffer pData */
	BYTE bLength;   /* (in) length of send data, (out) length of received data */
	BYTE *pData;    /* pointer to data buffer (not '\0' terminated) */
} ZCBCIPARMBINSTRING;
typedef struct _ZCBCIPARMBINSTRINGN
{
	BYTE bLength;   /* length (N) of binary string */
	BYTE *pData;    /* pointer to data buffer (not '\0' terminated) */
} ZCBCIPARMBINSTRINGN;
typedef ZCBCIPARMRAW *PZCBCIPARMRAW;
typedef BYTE ZCBCIPARMDUMMY[12];
typedef union _ZCBCIPARMDATA
{
	ZCBCIPARMBYTE    valbyte;
	ZCBCIPARMINT     valint;
	ZCBCIPARMLONG    vallong;
	ZCBCIPARMSTRING  valstring;
	ZCBCIPARMSTRINGN valstringn;
	ZCBCIPARMUSER    valuser;
	ZCBCIPARMRAW     valraw;  
	ZCBCIPARMOCXSTRING valocxstring;
	ZCBCIPARMDUMMY   donotuse;
    ZCBCIPARMSINGLE  valsingle;
	ZCBCIPARMBINSTRING  valbinstring;
	ZCBCIPARMBINSTRINGN valbinstringn;
} ZCBCIPARMDATA;
typedef struct _ZCBCIPARM
{
	ZCBCIPARMTYPE parmtype;
	ZCBCIPARMDATA data;
} ZCBCIPARM;
typedef ZCBCIPARM *PZCBCIPARM;
typedef struct _ZCBCILOCKINFO
{
  BYTE ReadLock;   /* liAllowed, liKeyed1, liKeyed2, or liForbidden */
  BYTE WriteLock;  /* liAllowed, liKeyed1, liKeyed2, or liForbidden */
  BYTE CustomLock; /* liAllowed, liUnlocked, or liLocked */
  BYTE ReadKey[2]; /* Key number(s) for ReadLock */
  BYTE WriteKey[2];/* Key number(s) for WriteLock */
} ZCBCILOCKINFO;
typedef ZCBCILOCKINFO *PZCBCILOCKINFO;
typedef enum _ZCBCILOCK
{
	ZCBCI_LOCK_CUSTOM=0x0,
	ZCBCI_LOCK_READ  =0x4,
	ZCBCI_LOCK_WRITE =0x8,
	ZCBCI_LOCK_READWRITE=0xC,
    ZCBCI_LOCK_DUMMY =0x7FFFFFFF /* make it use 32 bits */
} ZCBCILOCK;
typedef ZCBCILOCK *PZCBCILOCK;
#pragma pack()

/* error codes */
#define ZCBCI_NOERROR                0UL
#define ZCBCI_ERROR_BASE             0x02000000UL
#define ZCBCI_ERROR_PARM             (ZCBCI_ERROR_BASE + 0x0000UL)
#define ZCBCI_ERROR_MISC             (ZCBCI_ERROR_BASE + 0x0001UL)
#define ZCBCI_ERROR_MEM              (ZCBCI_ERROR_BASE + 0x0002UL)
#define ZCBCI_ERROR_OVERFLOW         (ZCBCI_ERROR_BASE + 0x0003UL)
#define ZCBCI_ERROR_TRANS_FAILED     (ZCBCI_ERROR_BASE + 0x0004UL)
#define ZCBCI_ERROR_MISSING_PARM     (ZCBCI_ERROR_BASE + 0x0005UL)
#define ZCBCI_ERROR_INVALID_FILE     (ZCBCI_ERROR_BASE + 0x0006UL)

/* sw error codes */
#define ZCBCI_ERROR_BASE_SW           0x04000000UL
#define ZCBCI_SW2BCI(sw)              (((ZCBCIRET) ((WORD) (sw))) + ZCBCI_ERROR_BASE_SW)
#define ZCBCI_IS_SW(e)                (((e) >= ZCBCI_ERROR_BASE_SW) && ((e) <= (ZCBCI_ERROR_BASE_SW+0xFFFF))
#define ZCBCI_BCI2SW(e)               ((WORD) ((e) & 0xFFFF))

/* card states */
#define	ZCBCI_STATE_NEW    0
#define ZCBCI_STATE_LOAD   1
#define ZCBCI_STATE_TEST   2
#define ZCBCI_STATE_RUN    3

/* professional basiccard states */
#ifdef ZCBCI_PROFESSIONAL
#define	ZCBCI_PROSTATE_NEW    0
#define ZCBCI_PROSTATE_LOAD   1
#define ZCBCI_PROSTATE_PERS   2
#define ZCBCI_PROSTATE_TEST   3
#define ZCBCI_PROSTATE_RUN    4
#endif

/* encryption algorithms */
#define ZCBCI_DEFAULT_ENC  0
#define ZCBCI_SG_LFSR      0x11
#define ZCBCI_SG_LFSR_CRC  0x12
#ifdef ZCBCI_ENHANCED
#define ZCBCI_DES          0x21
#define ZCBCI_3DES         0x22
#endif
#ifdef ZCBCI_PROFESSIONAL
#define ZCBCI_DES_CRC      0x23
#define ZCBCI_3DES_CRC     0x24
#define ZCBCI_AES          0x31
#endif

/* special transaction values */
#define ZCBCI_DEFAULT_LC   0xFFFF
#define ZCBCI_DEFAULT_LE   0xFFFF
#define ZCBCI_DISABLE_LE   0xFFFE

/* fileio modes */
#define ZCBCI_MODE_APPEND 0x60
#define ZCBCI_MODE_BINARY 0x20
#define ZCBCI_MODE_INPUT  0x30
#define ZCBCI_MODE_OUTPUT 0x40
#define ZCBCI_MODE_RANDOM 0x50
/* filio acess */
#define ZCBCI_ACCESS_READ  0x04
#define ZCBCI_ACCESS_WRITE 0x08
#define ZCBCI_ACCESS_READWRITE (ZCBCI_ACCESS_READ | ZCBCI_ACCESS_WRITE)
/* fileio sharing */
#define ZCBCI_SHARE_SHARED        0x03
#define ZCBCI_SHARE_LOCKREAD      0x02
#define ZCBCI_SHARE_LOCKWRITE     0x01
#define ZCBCI_SHARE_LOCKREADWRITE 0x00
/* Note: do not conncat ZCBCI_SHARE_LOCKREAD and ZCBCI_SHARE_LOCKWRITE by | */
/*       use ZCBCI_SHARE_LOCKREADWRITE instead                              */                

/* SW1SW2 CODES */
#ifdef ZCBCI_INCL_SW
#define	swCommandOK           0x9000
#define	swRetriesRemaining    0x63C0
#define	swEepromWriteError    0x6581
#define swBadEepromHeap       0x6582
#define swBadFileChain        0x6583
#define	swKeyNotFound         0x6611
#define	swPolyNotFound        0x6612
#define	swKeyTooShort         0x6613
#define	swKeyDisabled         0x6614
#define	swUnknownAlgorithm    0x6615
#define	swAlreadyEncrypting   0x66C0
#define	swNotEncrypting       0x66C1
#define	swBadCommandCRC       0x66C2
#define	swDesCheckError       0x66C3
#define swCoprocessorError    0x66C4
#define swAesCheckError       0x66C5
#define	swLcLeError           0x6700
#define	swCommandTooLong      0x6781
#define swResponseTooLong     0x6782
#define	swInvalidState        0x6985
#define	swCardUnconfigured    0x6986
#define	swNewStateError       0x6987
#define	swP1P2Error           0x6A00
#define	swOutsideEeprom       0x6A02
#define	swDataNotFound        0x6A88
#define	swINSNotFound         0x6D00
#define swReservedINS         0x6D80
#define	swCLANotFound         0x6E00
#define swReservedCLA         0x6E80
#define swInternalError       0x6F00
#define	swNoCardReader        0x6790
#define	swCardReaderError     0x6791
#define	swNoCardInReader      0x6792
#define	swCardPulled          0x6793
#define	swT1Error             0x6794
#define	swCardError           0x6795
#define	swCardNotReset        0x6796
#define	swKeyNotLoaded        0x6797
#define	swPolyNotLoaded       0x6798
#define	swBadResponseCRC      0x6799
#define	swCardTimedOut        0x679A
#define	swTermOutOfMemory     0x679B
#define	swBadDesResponse      0x679C
#define	swInvalidComPort      0x679D
#define	swComPortNotSupported 0x679E
#define swNoPcscDriver        0x679F
#define swPcscReaderBusy      0x67A0
#define swPcscError           0x67A1
#define swComPortBusy         0x67A2
#define swBadATR              0x67A3
#define swT0Error             0x67A4
#define swPTSError            0x67A7
#define swDataOverrun         0x67A8
#define swBadAesResponse      0x67A9
#define	swWriteVerifyError    0x67F0
#endif

#ifdef ZCBCI_INCL_FE
#define feInvalidDrive        1
#define feBadFilename         2
#define feBadFilenum          3
#define feFileNotFound        4
#define feFileNotOpen         5
#define feOpenError           6
#define feSeekError           7
#define feReadError           8
#define feWriteError          9
#define feCloseError          10
#define feInvalidMode         11
#define feInvalidAccess       12
#define feRenameError         13
#define feAccessDenied        14
#define feSharingViolation    15
#define feFileAlreadyExists   16
#define feNotDataFile         17
#define feNotDirectory        18
#define feDirNotEmpty         19
#define feBadFileChain        20
#define feFileOpen            21
#define feNameTooLong         22
#define feRecordTooLong       23
#define feTooManyOpenFiles    24
#define feTooManyCardFiles    25
#define feCommsError          26
#define feRemoteFile          27
#define feNotRemoteFile       28
#define feRecursiveRename     29
#define feInvalidFromKeyboard 30
#define feBadParameter        31
#define feOutOfMemory         32
#define feNoFileSystem        33
#define feUnexpectedError     34
#define feNotImplemented      35
#define feTooManyCustomLocks  36
#endif

#ifdef ZCBCI_INCL_FA
#define faDirectory  0x0010
#define faCardFile   0x0040
#define faReadOnly   0x0001
#define faHiddenFile 0x0002
#define faSystemFile 0x0004
#define faArchived   0x0020
#define faNormal     0x0080
#define faTemporary  0x0100
#endif

#ifdef ZCBCI_INCL_LI
#define liAllowed   0
#define liKeyed1    1
#define liKeyed2    2
#define liForbidden 3
#define liUnlocked  1
#define liLocked    2
#endif


/* linkage conventions */
#ifdef _WIN32
#define ZCBCILINK APIENTRY
#else
#define ZCBCILINK
#endif

/* ZeitControl internal use for creating import libraries */
#ifdef ZCMAKEDLLBCI
#define ZCDLLEXP __declspec(dllexport)
#define ZCEXPD ;
#endif

#ifdef ZCMAKEIMPBCI
#define ZCDLLEXP __declspec(dllexport)
#define ZCEXPD { return 0; }
#endif

#ifndef ZCEXPD
#define ZCDLLEXP
#define ZCEXPD ;
#endif


/* function prototypes */
/******************************************************************************/
/* Function       : ZCBciAttach                                               */
/* Description    : Attach to basiccard                                       */
/*                  Note: First connect to card reader and card by            */
/*                        ZCCriConnect, then attach to BCI by using           */
/*                        ZCCRIREADER returned by ZCCriConnect.               */
/* Parameter      : (in) reader  reader returned by ZCCriConnect              */
/*                  (out) *pCard handle to attached context                   */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/******************************************************************************/
/* History        : 19990223 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciAttach(ZCCRIREADER reader, 
										PZCBCICARD pCard) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciDetach                                               */
/* Description    : Detach from basiccard (close context)                     */
/* Parameter      : (in) card handle to attached context                      */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/******************************************************************************/
/* History        : 19990223 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciDetach(ZCBCICARD card) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciCreateHandle                                         */
/* Description    : Create (limited) handle to basiccard                      */
/*                  Note: Returned handle can be used for key managment       */
/*                        functions only. To get full functional handle       */
/*                        at once use ZCBciAttach                             */
/*                        Use ZCBciDetach to destroy handle                   */
/*                        Use ZCBciAttachHandle to get full functional handle */
/* Parameter      : (out) *pCard handle to attached context                   */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/******************************************************************************/
/* History        : 19990223 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciCreateHandle(PZCBCICARD pCard) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciAttachHandle                                         */
/* Description    : Attach to basiccard                                       */
/*                  Note: First connect to card reader and card by            */
/*                        ZCCriConnect, then attach to BCI by using           */
/*                        ZCCRIREADER returned by ZCCriConnect.               */
/* Parameter      : (in) reader  reader returned by ZCCriConnect              */
/*                  (in) card handle to card context received by              */
/*                            ZCBciCreateHandle                               */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/******************************************************************************/
/* History        : 19990223 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciAttachHandle(ZCCRIREADER reader, ZCBCICARD card) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciSetKey                                               */
/* Description    : Set encryption key (must match card declared key)         */
/* Parameter      : (in) card handle to attached context                      */
/*                  (in)  keynum number of key (same as declared in card      */
/*                               file                                         */
/*                  (in)  *pKey  key bytes (same key and byte order as        */
/*                               declared in card basic file                  */
/*                  (in)  cbKey  sizeof key (8 or 16 bytes)                   */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciSetKey(ZCBCICARD card,
						    BYTE keynum, BYTE *pKey, BYTE cbKey) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciSetPoly                                              */
/* Description    : Set encryption polynom (must match card declared polynom) */
/* Parameter      : (in) card handle to attached context                      */
/*                  (in)  polynomA first polynom maybe 0 if no SG_LFSR will   */
/*                                 used (otherwise same as ...)               */
/*                  (in)  polynomS second polynom maybe 0 if no SG_LFSR will  */
/*                                 used (otherwise same as ...)               */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciSetPoly(ZCBCICARD card,
						     DWORD polynomA, DWORD polynomS) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciReadKeyFile                                          */
/* Description    : Load encryption keys from file (as generated by keygen)   */
/* Parameter      : (in) card handle to attached context                      */
/*                  (in) pszFile name of key file                             */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_INVALID_FILE invalid key file                 */
/******************************************************************************/
/* History        : 19990304 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciReadKeyFile(ZCBCICARD card, 
											 CHAR *pszFile) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciTransaction                                          */
/* Description    : Do low level transaction                                  */
/* Parameter      : (in) card handle to attached context                      */
/*                  (in) Cla class byte                                       */
/*                  (in) Ins instruction byte                                 */
/*                  (in/out) pData pointer to array of BasicCard parameters   */
/*                  (in) bdataCount number of BasicCard parameters            */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/* History        : 19990223 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciTransaction(ZCBCICARD card, BYTE Cla, BYTE Ins, 
									PZCBCIPARM pData, BYTE bDataCount,
									WORD *pSW1SW2) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciTransaction2                                         */
/* Description    : transaction for special cases not supported by            */
/*                  ZCBciTransaction                                          */
/* Parameter      : (in) card handle to attached context                      */
/*                  (in) Cla class byte                                       */
/*                  (in) Ins instruction byte                                 */
/*                  (in) P1  parameter 1                                      */
/*                  (in) P2  parameter 2                                      */
/*                  (in) Lc  Lc (0..255) or ZCBCI_DEFAULT_LC                  */
/*                  (in) Le  Le (0..255) ZCBCI_DISABLE_LE or ZCBCI_DEFAULT_LE */
/*                  (in/out) pData pointer to array of BasicCard parameters   */
/*                  (in) bdataCount number of BasicCard parameters            */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/* History        : 19990223 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciTransaction2(ZCBCICARD card, BYTE Cla, BYTE Ins, 
									BYTE P1, BYTE P2, WORD Lc, WORD Le,
									PZCBCIPARM pData, BYTE bDataCount,
									WORD *pSW1SW2) ZCEXPD

#ifdef ZCBCI_COMPACT
/* Compact BasicCard */
/******************************************************************************/
/* Function       : ZCBciGetState                                             */
/* Description    : BasicCard GET STATE command (Cla C0, Ins 00)              */
/*                  return card state (For Compact and Enhanced BasicCard:    */
/*                  ZCBCI_STATE_NEW, ZCBCI_STATE_LOAD, ZCBCI_STATE_TEST or    */
/*                  ZCBCI_STATE_RUN; For Professional BasicCard:              */
/*                  ZCBCI_PROSTATE_NEW, ZCBCI_PROSTATE_LOAD,                  */
/*                  ZCBCI_PROSTATE_PERS,ZCBCI_PROSTATE_TEST or                */
/*                  ZCBCI_PROSTATE_RUN;)                                      */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pState  card state                                 */
/*                  (out) *pVersion BasicCard version                         */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/*                  20011204 MP support for Professional BasicCard added      */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetState(ZCBCICARD card, WORD *pSW1SW2,
								 BYTE *pState, WORD *pVersion) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciGetStateVersion                                      */
/* Description    : BasicCard GET STATE command (Cla C0, Ins 00)              */
/*                  return card state (For Compact and Enhanced BasicCard:    */
/*                  ZCBCI_STATE_NEW, ZCBCI_STATE_LOAD, ZCBCI_STATE_TEST or    */
/*                  ZCBCI_STATE_RUN; For Professional BasicCard:              */
/*                  ZCBCI_PROSTATE_NEW, ZCBCI_PROSTATE_LOAD,                  */
/*                  ZCBCI_PROSTATE_PERS,ZCBCI_PROSTATE_TEST or                */
/*                  ZCBCI_PROSTATE_RUN;)                                      */
/*                  and version string                                        */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pState  card state                                 */
/*                  (out) *pVersion BasicCard version                         */
/*                  (out) *pVersionStr BasicCard version string (if pointer   */
/*                                     is not NULL                            */
/*                  (in/out) *pVersionStrLen length of buffer pVersionStr on  */
/*                                           input, length of version string  */
/*                                           on output                        */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/*                  20011204 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetStateVersion(ZCBCICARD card, WORD *pSW1SW2,
								 BYTE *pState, WORD *pVersion,
								 CHAR *pVersionStr, DWORD *pVersionStrLen) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciEepromSize                                           */
/* Description    : BasicCard EEPROM SIZE command (Cla C0, Ins 02)            */
/*                  return start and length of card eeprom                    */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pStart  start of card eeprom                       */
/*                  (out) *pLength length of card eeprom                      */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciEepromSize(ZCBCICARD card, WORD *pSW1SW2,
								   WORD *pStart, WORD *pLength) ZCEXPD

/******************************************************************************/
/* Function       : ZCClearEeprom                                             */
/* Description    : BasicCard CLEAR EEPROM command (Cla C0, Ins 04)           */
/*                  clear (set to FF) length bytes of eeprom starting at start*/
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (in)  start  start of card eeprom to clear                */
/*                  (in)  length length of card eeprom to clear               */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciClearEeprom(ZCBCICARD card, WORD *pSW1SW2,
								   WORD start, WORD length) ZCEXPD

/******************************************************************************/
/* Function       : ZCWriteEeprom                                             */
/* Description    : BasicCard WRITE EEPROM command (Cla C0, Ins 06)           */
/*                  write cbData bytes to eeprom starting at start            */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (in)  start  start of card eeprom to clear                */
/*                  (in)  pData  data to write                                */
/*                  (in)  cbData length of data to write                      */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciWriteEeprom(ZCBCICARD card, WORD *pSW1SW2,
								    WORD start, BYTE *pData, BYTE cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCReadEeprom                                              */
/* Description    : BasicCard READ EEPROM command (Cla C0, Ins 08)            */
/*                  read cbData bytes from eeprom starting at start           */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (in)  start  start of card eeprom to clear                */
/*                  (out) *pData  data read                                   */
/*                  (in)  cbData length of data to read                       */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciReadEeprom(ZCBCICARD card, WORD *pSW1SW2,
								    WORD start, BYTE *pData, BYTE cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciEepromCRC                                            */
/* Description    : BasicCard EEPROM CRC command (Cla C0, Ins 0A)             */
/*                  calculate and return eeprom crc                           */
/*                  Note: Calling ZCBciEepromCRC on empty (no program loaded) */
/*                        Enhanced BasicCard will destroy card.               */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (in)  start  start address                                */
/*                  (in)  length to verify                                    */
/*                  (out) *pCRC  resulting crc                                */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciEepromCRC(ZCBCICARD card, WORD *pSW1SW2,
								  WORD start, WORD length, WORD *pCRC) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciSetState                                             */
/* Description    : BasicCard SET STATE command (Cla C0, Ins 0C)              */
/*                  set card state (For Compact and Enhanced BasicCard:       */
/*                  ZCBCI_STATE_NEW, ZCBCI_STATE_LOAD, ZCBCI_STATE_TEST or    */
/*                  ZCBCI_STATE_RUN; For Professional BasicCard:              */
/*                  ZCBCI_PROSTATE_NEW, ZCBCI_PROSTATE_LOAD,                  */
/*                  ZCBCI_PROSTATE_PERS,ZCBCI_PROSTATE_TEST or                */
/*                  ZCBCI_PROSTATE_RUN;)                                      */
/*                  Note: You need to reset card (ZCCriReconnect) before doing*/
/*                        any further action on this card.                    */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (in)  state  new card state                               */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciSetState(ZCBCICARD card, WORD *pSW1SW2,
								 BYTE state) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciGetApplicationID                                     */
/* Description    : BasicCard GET APPLICATION ID command (Cla C0, Ins 0E)     */
/*                  get application id                                        */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pszID  application id                              */
/*                  (in)  cbID  size of buffer to hold id                     */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetApplicationID(ZCBCICARD card, WORD *pSW1SW2,
								         CHAR *pszID, BYTE cbID) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciStartEncryption                                      */
/* Description    : BasicCard START ENCRYPTION command (Cla C0, Ins 10)       */
/*                  start transparent encryption                              */
/*                  Note: For AES encryption it is recommended to use         */
/*                        ZCBciStartProEncryption                             */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (in)  algo  algorithm to use either ZCBCI_SG_LFSR,        */
/*                              ZCBCI_SG_LFSR_CRC, ZCBCI_DES, ZCBCI_3DES      */
/*                              ZCBCI_DES_CRC, ZCBCI_3DES_CRC or ZCBCI_AES    */
/*                  (in)  key   key to use (see ZCBciSetKey also)             */
/*                  (in)  random input random used by encryption as part of IV*/
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/*                  20011204 MP add support for Professional BasicCard        */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciStartEncryption(ZCBCICARD card, WORD *pSW1SW2,
								        BYTE algo, BYTE key, DWORD random) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciStartProEncryption                                   */
/* Description    : BasicCard START ENCRYPTION command (Cla C0, Ins 10)       */
/*                  start transparent encryption                              */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (in/out)  *pAlgo  algorithm to use either ZCBCI_SG_LFSR,  */
/*                              ZCBCI_SG_LFSR_CRC, ZCBCI_DES, ZCBCI_3DES      */
/*                              ZCBCI_DES_CRC, ZCBCI_3DES_CRC or ZCBCI_AES    */
/*                  (in)  key   key to use (see ZCBciSetKey also)             */
/*                  (in)  random input random used by encryption as part of IV*/
/*                  (in)  random2 input random second part used by AES only   */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/*                  20011204 MP add support for Professional BasicCard        */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciStartProEncryption(ZCBCICARD card, WORD *pSW1SW2,
								        BYTE *pAlgo, BYTE key, 
										DWORD random, DWORD random2) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciEndEncryption                                        */
/* Description    : BasicCard END ENCRYPTION command (Cla C0, Ins 12)         */
/*                  stop transparent encryption                               */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciEndEncryption(ZCBCICARD card, 
											   WORD *pSW1SW2) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciEcho                                                 */
/* Description    : BasicCard ECHO command (Cla C0, Ins 04)                   */
/*                  echo data (for testing)                                   */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (in)  inc increment value                                 */
/*                  (in/out) *pszEcho '\0' terminated echo string             */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_OVERFLOW  to much data                        */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                  ZCBCI_ERROR_MISSING_PARM one or more return parameter is  */
/*                                           missing                          */
/******************************************************************************/
/* History        : 19990224 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciEcho(ZCBCICARD card, WORD *pSW1SW2,
						     BYTE inc, CHAR *pszEcho) ZCEXPD



#endif

#ifdef ZCBCI_ENHANCED 
/* Enhanced BasicCard (only) */
/******************************************************************************/
/* Function       : ZCBciMkDir                                                */
/* Description    : Make directory on card                                    */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) pszPath dircetory to create. Path may be relative    */
/*                               to current directory or absolut (starting    */
/*                               with "\"                                     */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciMkDir(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							  const CHAR *pszPath) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciRmDir                                                */
/* Description    : Remove directory on card                                  */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) pszPath dircetory to remove. Path may be relative    */
/*                               to current directory or absolut (starting    */
/*                               with "\"                                     */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciRmDir(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							  const CHAR *pszPath) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciChDir                                                */
/* Description    : Change current directory on card                          */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) pszPath dircetory to change. Path may be relative    */
/*                               to current directory or absolut (starting    */
/*                               with "\"                                     */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciChDir(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							  const CHAR *pszPath) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciCurDir                                               */
/* Description    : Return current directory                                  */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (out) pszPath current directory                           */
/*                  (in)  cbPath sizeof buffer pszPath                        */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/*                  ZCBCI_ERROR_OVERFLOW  directory name exceeds buffer size  */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciCurDir(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							   CHAR *pszPath, BYTE cbPath) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciRename                                               */
/* Description    : Rename or move a file or directory                        */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) pszOldPath current file/directory name               */
/*                  (in) pszNewPath new file/directory name                   */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciRename(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							   const CHAR *pszOldPath, const CHAR *pszNewPath) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciDirCount                                             */
/* Description    : Count files on card matching filespec                     */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) pszFileSpec filespec to search for                   */
/*                  (out) *pCount number of files match                       */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciDirCount(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							     const CHAR *pszFileSpec, WORD *pCount) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciDirFile                                              */
/* Description    : Return file on card matching filespec and number          */
/*                  ZCBciDirCount must be called before with same filespec    */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) pszFileSpec filespec to search for                   */
/*                  (in) num number of file to return (1<=num<=count)         */
/*                  (out) *pszFile name of matching file                      */
/*                  (in) cbFile size of buffer pszFile                        */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/*                  ZCBCI_ERROR_OVERFLOW  filename exceeds buffer size        */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciDirFile(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							    const CHAR *pszFileSpec, WORD num,
								CHAR *pszFile, BYTE cbFile) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciGetAttr                                              */
/* Description    : Get file or directory attribute                           */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) pszPath dircetory or file to change. Path may be     */
/*                               relative to current directory or absolut     */
/*                               (starting with "\")                          */
/*                  (out) *pAttrib returned attributes (see fa... for         */
/*                                 attributes)                                */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetAttr(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							    const CHAR *pszPath, WORD *pAttrib) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciEraseFile                                            */
/* Description    : Erase file                                                */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) pszPath file to erase                                */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciEraseFile(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							      const CHAR *pszPath) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciOpenFile                                             */
/* Description    : Open file on BasicCard                                    */
/*                  Note: There is a difference in using ZCBCiFileWriteUser   */
/*                        and ZCBciFilePut or ZCBciFilePutPos. Write functions*/
/*                        do not result in same binary file data as done by   */
/*                        put functions. You must use put/get functions if    */
/*                        file open mode is binary (ZCBCI_MODE_BINARY) or     */
/*                        random (ZCBCI_MODE_RANDOM), and you must use        */
/*                        read/write functions otherwise. See BasicCard       */
/*                        reference manual for more details.                  */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) pszPath file to open                                 */
/*                  (in) mode any valid combination of ZCBCI_MODE_..,         */
/*                       ZCBCI_ACCESS_.. and ZCBCI_SHARE_.. use '|' to concat */
/*                       values                                               */
/*                       Note: do not conncat ZCBCI_SHARE_LOCKREAD and        */
/*                             ZCBCI_SHARE_LOCKWRITE by '|' use               */
/*                             ZCBCI_SHARE_LOCKREADWRITE instead              */
/*                  (in) recordlen record/block size used by file             */
/*                  (out) *pFileNum handle to open file use for fileio        */
/*                        on this open file                                   */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciOpenFile(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							     const CHAR *pszPath, BYTE mode, WORD recordlen,
								 WORD *pFileNum) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciCloseFile                                            */
/* Description    : Close file on BasicCard                                   */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file to close                                */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciCloseFile(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							      WORD FileNum) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciCloseAllFiles                                        */
/* Description    : Close all files on BasicCard                              */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciCloseAllFiles(ZCBCICARD card, WORD *pSW1SW2, 
									  BYTE *pFileErr) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileLength                                           */
/* Description    : Return file length of open file                           */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *pFileLength length of file                         */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileLength(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							       WORD FileNum, DWORD *pFileLength) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciGetFilepos                                           */
/* Description    : Get file position                                         */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *pFilePos  position of file pointer                 */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetFilepos(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							       WORD FileNum, DWORD *pFilePos) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciSetFilepos                                           */
/* Description    : Set file position                                         */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (in) FilePos  position of file pointer                    */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciSetFilepos(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							       WORD FileNum, DWORD FilePos) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciQueryEof                                             */
/* Description    : Query if end of file is reached                           */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *pfEof  TRUE if EOF otherwise FALSE                 */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciQueryEof(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							     WORD FileNum, BOOL *pfEof) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileGet                                              */
/* Description    : Get content of open file at current file position         */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *pData  data read from file                         */
/*                  (in)  cbData  number of data bytes requested              */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/*                  ZCBCI_ERROR_MISSING_PARM not enough data received         */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileGet(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							    WORD FileNum, BYTE *pData, BYTE cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileGetPos                                           */
/* Description    : Get content of open file at specified file position       */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (in) pos  position to read from                           */
/*                  (out) *pData  data read from file                         */
/*                  (in)  cbData  number of data bytes requested              */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/*                  ZCBCI_ERROR_MISSING_PARM not enough data received         */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileGetPos(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							       WORD FileNum, DWORD pos, 
								   BYTE *pData, BYTE cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFilePut                                              */
/* Description    : Put content of pData to open file at current file position*/
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *pData  data to write to file                       */
/*                  (in)  cbData  number of data bytes to write               */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFilePut(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							    WORD FileNum, const BYTE *pData, BYTE cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFilePutPos                                           */
/* Description    : Put content of pData to open file at specified file       */
/*                  position                                                  */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (in) pos  position to write to                            */
/*                  (out) *pData  data to write to file                       */
/*                  (in)  cbData  number of data bytes to write               */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFilePutPos(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							       WORD FileNum, DWORD pos, 
								   const BYTE *pData, BYTE cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileRead                                             */
/* Description    : Read the content of file at current position to pData     */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *pData  data read from file                         */
/*                  (in)  cbData  number of data bytes to read                */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990325 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileRead(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							     WORD FileNum, BYTE *pData, BYTE cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileReadLong                                         */
/* Description    : Read long value from file at current position to plVar    */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *plVar long value read from file                    */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990325 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadLong(ZCBCICARD card, WORD *pSW1SW2, 
									 BYTE *pFileErr, WORD FileNum, 
									 LONG *plVar) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileReadSingle                                       */
/* Description    : Read single/float value from file at current position to  */
/*                  pflVar                                                    */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *pflVar float value read from file                  */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990325 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadSingle(ZCBCICARD card, WORD *pSW1SW2, 
									   BYTE *pFileErr, WORD FileNum, 
									   float *pflVar) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileReadString                                       */
/* Description    : Read string from file at current position to pszVar       */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *pszVar  string read from file                      */
/*                  (in)  cbVar  size of string buffer                        */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/*                  ZCBCI_ERROR_OVERFLOW  string exceeds buffer size          */
/******************************************************************************/
/* History        : 19990325 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadString(ZCBCICARD card, WORD *pSW1SW2, 
									   BYTE *pFileErr, WORD FileNum, 
									   CHAR *pszVar, BYTE cbVar) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileReadUser                                         */
/* Description    : Read user type data from file at current position to pData*/
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *pData  data read from file                         */
/*                  (in/out)  *pcbData length of data buffer/bytes read       */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/*                  ZCBCI_ERROR_OVERFLOW  data exceeds buffer size            */
/******************************************************************************/
/* History        : 19990325 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadUser(ZCBCICARD card, WORD *pSW1SW2, 
									 BYTE *pFileErr, WORD FileNum, 
									 BYTE *pData, BYTE *pcbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileReadUser2                                        */
/* Description    : Read block from file at current position to pData         */
/*                  Note: In opposition to ZCBciFileReadUser cbData must match*/
/*                        length of data object stored in file.               */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *pData  block read from file                        */
/*                  (in)  cbData  block length                                */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990325 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadUser2(ZCBCICARD card, WORD *pSW1SW2, 
									  BYTE *pFileErr, WORD FileNum, 
									  BYTE *pData, BYTE cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileReadLine                                         */
/* Description    : Read line from file at current position to pszLine        */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (out) *pszLine  line read from file                       */
/*                  (in)  cbLine  size of buffer pszLine                      */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/*                  ZCBCI_ERROR_OVERFLOW buffer pszLine to small              */
/******************************************************************************/
/* History        : 19990407 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileReadLine(ZCBCICARD card, WORD *pSW1SW2, 
									 BYTE *pFileErr, WORD FileNum, 
									 CHAR *pszLine, BYTE cbLine) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileWriteLong                                        */
/* Description    : Write long value at current position to file              */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (in) lVal  long value to write                            */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990407 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileWriteLong(ZCBCICARD card, WORD *pSW1SW2, 
									  BYTE *pFileErr, WORD FileNum, 
									  LONG lVal) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileWriteSingle                                      */
/* Description    : Write float (single) value at current position to file    */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (in) flVal  float (single) value to write                 */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990413 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileWriteSingle(ZCBCICARD card, WORD *pSW1SW2, 
									    BYTE *pFileErr, WORD FileNum, 
									    float flVal) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileWriteUser                                        */
/* Description    : Write user type data at current position to file          */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (in) *pbData  data to write                               */
/*                  (in) cbData  length of data to write                      */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990413 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileWriteUser(ZCBCICARD card, WORD *pSW1SW2, 
									  BYTE *pFileErr, WORD FileNum, 
									  const BYTE *pData, BYTE cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileWriteString                                      */
/* Description    : Write string data at current position to file             */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (in) *pszVal  string to write                             */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990413 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileWriteString(ZCBCICARD card, WORD *pSW1SW2, 
									    BYTE *pFileErr, WORD FileNum, 
									    const CHAR *pszVal) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFileWrite                                            */
/* Description    : Write data at current position to file.                   */
/*                  In difference to ZCBciFileWriteUser data is not prefixed  */
/*                  with type byte, before written to file.                   */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (in) *pbData  data to write                               */
/*                  (in) cbData  length of data to write                      */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990413 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFileWrite(ZCBCICARD card, WORD *pSW1SW2, 
								  BYTE *pFileErr, WORD FileNum, 
								  const BYTE *pData, BYTE cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciFilePrint                                            */
/* Description    : Print string at current position to file.                 */
/*                  This function just calls ZCBciFileWrite                   */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) FileNum file number returned by ZCBciOpenFile        */
/*                  (in) *pszVal  string to write                             */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990413 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciFilePrint(ZCBCICARD card, WORD *pSW1SW2, 
								  BYTE *pFileErr, WORD FileNum, 
								  const CHAR *pszVal) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciGetLockInfo                                          */
/* Description    : Get lock information of file or directory                 */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) *pszPath file/directory to query                     */
/*                  (out) *pInfo  lock information of file or directory       */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/*                  ZCBCI_ERROR_MISSING_PARM not enough data received         */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciGetLockInfo(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							        const CHAR *pszPath, PZCBCILOCKINFO pInfo) ZCEXPD

/******************************************************************************/
/* Function       : ZCBciSetFileLock                                          */
/* Description    : Set lock information of file or directory                 */
/* Parameter      : (in) card handle to attached context                      */
/*                  (out) *pSW1SW2 card return value SW1 SW2                  */
/*                  (out) *pFileErr file error as returned by card            */
/*                  (in) *pszPath file/directory to change                    */
/*                  (in) type lock type ZCBCI_LOCK_..                         */
/*                  (in) fLock TRUE lock file, FALSE unlock file              */
/*                  (in) KeyCount number of access keys (0..2)                */
/*                  (in) Key1 number of first key (ignored if KeyCount<1)     */
/*                  (in) Key2 number of second key (ignored if KeyCount<2)    */
/* Returnvalue    : ZCBCI_NOERROR    no error                                 */
/*                  ZCBCI_ERROR_PARM invalid parameter pCard                  */
/*                  ZCBCI_ERROR_TRANS_FAILED transaction failed (SW1SW2       */
/*                                           differs from 0x9000 or 0x61XX    */
/*                                           or file error occured (*pFileErr */
/*                                           !=0)                             */
/******************************************************************************/
/* History        : 19990317 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCBCIRET ZCBCILINK ZCBciSetFileLock(ZCBCICARD card, WORD *pSW1SW2, BYTE *pFileErr,
							        const CHAR *pszPath, ZCBCILOCK type, BOOL fLock,
									BYTE KeyCount, BYTE Key1, BYTE Key2) ZCEXPD
#endif


#ifdef __cplusplus
}
#endif

#endif
