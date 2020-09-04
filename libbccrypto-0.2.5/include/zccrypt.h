/******************************************************************************/
/* Copyright      : ZeitControl cardsystems GmbH 2000                         */
/* Author         : Michael Petig                                             */
/*                  based on crypto functions from Tony Guilfoyle             */
/* Modified by    : Christian Zietz for libbasiccard / libbccrypto            */
/******************************************************************************/
/* Project        : ZeitControl Crypto API                                    */
/* Description    : Crypto support API including ECC, DES and SHA             */
/* History        : 20000128 MP initial version                               */
/*                  20030120 MP add support for ECC167 and AES                */
/******************************************************************************/

#ifndef __ZCCRYPT_H__
#define __ZCCRYPT_H__

#include <chzcrwt.h>

#ifdef __cplusplus
extern "C" {
#endif

/* API version */
#define ZCCRYPT_VER 0x102
#define ZCCRYPT_VER_1_0
#define ZCCRYPT_VER_1_1
#define ZCCRYPT_VER_1_2

/* linkage conventions */
#ifdef _WIN32
#define ZCCRYPTLINK APIENTRY
#else
#define ZCCRYPTLINK
#endif

/* internal use for creating import libraries */
#ifdef ZCMAKEDLLCRYPT
#define ZCDLLEXP __declspec(dllexport)
#define ZCEXPD ;
#endif

#ifdef ZCMAKEIMPCRYPT
#define ZCDLLEXP __declspec(dllexport)
#define ZCEXPD { return 0; }
#endif

#ifndef ZCEXPD
#define ZCDLLEXP
#define ZCEXPD ;
#endif

#ifndef ECSTUB
#define ECSTUB
#endif

/* error codes */
#define ZCCRYPT_NOERROR                0UL
#define ZCCRYPT_ERROR_BASE             0x03000000UL
#define ZCCRYPT_ERROR_PARM             (ZCCRYPT_ERROR_BASE + 0x0000UL)
#define ZCCRYPT_ERROR_MISC             (ZCCRYPT_ERROR_BASE + 0x0001UL)
#define ZCCRYPT_ERROR_MEM              (ZCCRYPT_ERROR_BASE + 0x0002UL)
#define ZCCRYPT_ERROR_NOTALLOWED       (ZCCRYPT_ERROR_BASE + 0x0003UL)
#define ZCCRYPT_ERROR_MISSINGPARM      (ZCCRYPT_ERROR_BASE + 0x0004UL)
/* special error codes EC161 and EC167 */
#define ZCCRYPT_ERROR_EC_BASE          (ZCCRYPT_ERROR_BASE + 0x1000UL)
#define ZCCRYPT_ERROR_EC_BADPARAMS     (ZCCRYPT_ERROR_EC_BASE + 0x0000UL)
#define ZCCRYPT_ERROR_EC_BADSIGNATURE  (ZCCRYPT_ERROR_EC_BASE + 0x00001L)
#define ZCCRYPT_ERROR_EC161_BASE          ZCCRYPT_ERROR_EC_BASE
#define ZCCRYPT_ERROR_EC161_BADPARAMS     ZCCRYPT_ERROR_EC_BADPARAMS
#define ZCCRYPT_ERROR_EC161_BADSIGNATURE  ZCCRYPT_ERROR_EC_BADSIGNATURE
#define ZCCRYPT_ERROR_EC167_BASE          ZCCRYPT_ERROR_EC_BASE
#define ZCCRYPT_ERROR_EC167_BADPARAMS     ZCCRYPT_ERROR_EC_BADPARAMS
#define ZCCRYPT_ERROR_EC167_BADSIGNATURE  ZCCRYPT_ERROR_EC_BADSIGNATURE
/* special error codes random */
#define ZCCRYPT_ERROR_RND_BASE         (ZCCRYPT_ERROR_BASE + 0x2000UL)
#define ZCCRYPT_ERROR_RND_NOTINIT      (ZCCRYPT_ERROR_RND_BASE + 0x0000UL)
#define ZCCRYPT_ERROR_RND_ALREADYINIT  (ZCCRYPT_ERROR_RND_BASE + 0x0001UL)

/* type definitions */
#pragma pack(1)
/* API return type */
typedef DWORD ZCCRYPTRET;
typedef ZCCRYPTRET *PZCCRYPTRET;
/* AES crypt context */
typedef void *ZCCRYPT_CONTEXT_AES;
typedef ZCCRYPT_CONTEXT_AES *PZCCRYPT_CONTEXT_AES;
/* DES crypt context */
typedef void *ZCCRYPT_CONTEXT_DES;
typedef ZCCRYPT_CONTEXT_DES *PZCCRYPT_CONTEXT_DES;
/* DES certificate context */
typedef void *ZCCRYPT_CONTEXT_DES_CERT;
typedef ZCCRYPT_CONTEXT_DES_CERT *PZCCRYPT_CONTEXT_DES_CERT;
/* SHA context */
typedef void *ZCCRYPT_CONTEXT_SHA;
typedef ZCCRYPT_CONTEXT_SHA *PZCCRYPT_CONTEXT_SHA;
/* encryption algorithm enum */
typedef enum _ZCCRYPT_ALGO {
    ZCCRYPT_ALGO_DES=0x21,
    ZCCRYPT_ALGO_3DES=0x22,
    ZCCRYPT_ALGO_AES128=0x31,
    ZCCRYPT_ALGO_AES192=0x32,
    ZCCRYPT_ALGO_AES256=0x33,
    ZCCRYPT_ALGO_DUMMY=0x7FFFFFFF  /* force enum to use 32 bit values */
} ZCCRYPT_ALGO;
typedef ZCCRYPT_ALGO *PZCCRYPT_ALGO;
/* encryption mode enum */
typedef enum _ZCCRYPT_MODE {
    ZCCRYPT_MODE_ECB=1,
    ZCCRYPT_MODE_CBC=2,
#ifdef 	ALL_MODES
	/* use on you own risk, this modes are not verified and tested */
    ZCCRYPT_MODE_CFB=3,
    ZCCRYPT_MODE_OFB=4,
#endif
    ZCCRYPT_MODE_DUMMY=0x7FFFFFFF  /* force enum to use 32 bit values */
} ZCCRYPT_MODE;
/* ECC 161 */
typedef struct _ZCCRYPT_EC161DOMAINPARAMS
  {
  BYTE a ;
  BYTE b[21] ;
  BYTE r[21] ;
  BYTE k ;
  BYTE Gx[21] ;
  BYTE Gy ;
  } ZCCRYPT_EC161DOMAINPARAMS;
typedef ZCCRYPT_EC161DOMAINPARAMS *PZCCRYPT_EC161DOMAINPARAMS;
typedef BYTE ZCCRYPT_EC161PRIVATEKEY[21];
typedef BYTE *PZCCRYPT_EC161PRIVATEKEY;
typedef BYTE ZCCRYPT_EC161PUBLICKEY[22];
typedef BYTE *PZCCRYPT_EC161PUBLICKEY;
typedef BYTE ZCCRYPT_EC161SIGNATURE[42];
typedef BYTE *PZCCRYPT_EC161SIGNATURE;
typedef BYTE ZCCRYPT_EC161SHAREDSECRET[21];
typedef BYTE *PZCCRYPT_EC161SHAREDSECRET;
typedef BYTE ZCCRYPT_EC161SESSIONKEY[20];
typedef BYTE *PZCCRYPT_EC161SESSIONKEY;
typedef void *ZCCRYPT_CONTEXT_EC161;
typedef ZCCRYPT_CONTEXT_EC161 *PZCCRYPT_CONTEXT_EC161;
/* ECC167 */
typedef struct _ZCCRYPT_EC167DOMAINPARAMS
  {
  BYTE a ;
  BYTE b[21] ;
  BYTE r[21] ;
  BYTE G[21] ;
  } ZCCRYPT_EC167DOMAINPARAMS;
typedef ZCCRYPT_EC167DOMAINPARAMS *PZCCRYPT_EC167DOMAINPARAMS;
typedef BYTE ZCCRYPT_EC167PRIVATEKEY[21];
typedef BYTE *PZCCRYPT_EC167PRIVATEKEY;
typedef BYTE ZCCRYPT_EC167PUBLICKEY[21];
typedef BYTE *PZCCRYPT_EC167PUBLICKEY;
typedef BYTE ZCCRYPT_EC167SIGNATURE[42];
typedef BYTE *PZCCRYPT_EC167SIGNATURE;
typedef BYTE ZCCRYPT_EC167SHAREDSECRET[21];
typedef BYTE *PZCCRYPT_EC167SHAREDSECRET;
typedef BYTE ZCCRYPT_EC167SESSIONKEY[20];
typedef BYTE *PZCCRYPT_EC167SESSIONKEY;
typedef void *ZCCRYPT_CONTEXT_EC167;
typedef ZCCRYPT_CONTEXT_EC167 *PZCCRYPT_CONTEXT_EC167;
#pragma pack()

/* function prototypes */
#ifdef ZCCRYPT_VER_1_0
/******************************************************************************/
/* Function       : ZCCryptGetVer                                             */
/* Description    : Get version of this API                                   */
/* Parameter      : (OUT) *pVer  version number e.g. 0x100 for V1.0           */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/******************************************************************************/
/* History        : 20000128 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptGetVer(DWORD *pVer) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptDesCreateContext                                   */
/* Description    : Create encrytion context for use with DES/3DES            */
/* Parameter      : (OUT) *pContext  encryption context to use with further   */
/*                                   DES function calls                       */
/*                  (IN) algo  encryption algorithm either ZCCRYPT_ALGO_DES   */
/*                             ZCCRYPT_ALGO_3DES                              */
/*                  (IN) mode  encryption chaining mode (ZCCRYPT_MODE_...)    */
/*                             either .._ECB, .._CBC, .._CFB or .._OFB        */
/*                  (IN) *pIV  8 byte initial vector or NULL for {0, .., 0}   */
/*                  (IN) *pKey 8 byte key for .._DES, 16 byte key for .._3DES */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter (pKey==NULL)        */
/******************************************************************************/
/* History        : 20000128 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesCreateContext(PZCCRYPT_CONTEXT_DES pContext,
                                               ZCCRYPT_ALGO algo,
                                               ZCCRYPT_MODE mode,
                                               const BYTE*        pIV,
                                               const BYTE*        pKey) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptDesSetIV                                           */
/* Description    : Set IV, maybe used to reset context to start IV           */
/* Parameter      : (IN) Context  encryption context return by                */
/*                                ZCCryptDesCreateContext                     */
/*                  (IN) *pIV  8 byte initial vector or NULL for {0, .., 0}   */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000131 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesSetIV(ZCCRYPT_CONTEXT_DES Context,
                                       const BYTE *pIV) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptDesGetIV                                           */
/* Description    : Get current active IV                                     */
/* Parameter      : (IN) Context  encryption context return by                */
/*                                ZCCryptDesCreateContext                     */
/*                  (OUT) *pIV  8 byte initial vector                         */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000131 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesGetIV(ZCCRYPT_CONTEXT_DES Context,
                                       BYTE *pIV) ZCEXPD


/******************************************************************************/
/* Function       : ZCCryptDesDestroyContext                                  */
/* Description    : Destroy encrytion context created by                      */
/*                  ZCCryptDesCreateContext this also destroy/clears all keys */
/* Parameter      : (IN) *pContext   encryption context to destroy            */
/*                                   DES function calls                       */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter (pKey==NULL)        */
/******************************************************************************/
/* History        : 20000131 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesDestroyContext(PZCCRYPT_CONTEXT_DES pContext) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptDesEncrypt                                         */
/* Description    : Encrypt n (multiple of 8 bytes / 64 bits) bytes           */
/* Parameter      : (IN) Context  encryption context return by                */
/*                                ZCCryptDesCreateContext                     */
/*                  (IN) pData   (pointer to) data to encrypt                 */
/*                  (IN) cbData  length of data to encrypt (must be multiple  */
/*                               of 8)                                        */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000131 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesEncrypt(ZCCRYPT_CONTEXT_DES Context,
                                         void *pData,
                                         DWORD cbLen) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptDesDecrypt                                         */
/* Description    : Decrypt n (multiple of 8 bytes / 64 bits) bytes           */
/* Parameter      : (IN) Context  encryption context return by                */
/*                                ZCCryptDesCreateContext                     */
/*                  (IN) pData   (pointer to) data to decrypt                 */
/*                  (IN) cbData  length of data to encrypt (must be multiple  */
/*                               of 8)                                        */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000131 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesDecrypt(ZCCRYPT_CONTEXT_DES Context,
                                         void *pData,
                                         DWORD cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptDesEncryptOnce                                     */
/* Description    : Encrypt n (multiple of 8 bytes / 64 bits) bytes           */
/* Parameter      : (IN) algo  encryption algorithm either ZCCRYPT_ALGO_DES   */
/*                             ZCCRYPT_ALGO_3DES                              */
/*                  (IN) mode  encryption chaining mode (ZCCRYPT_MODE_...)    */
/*                             either .._ECB, .._CBC, .._CFB or .._OFB        */
/*                  (IN) *pIV  8 byte initial vector or NULL for {0, .., 0}   */
/*                  (IN) *pKey 8 byte key for .._DES, 16 byte key for .._3DES */
/*                  (IN) pData   (pointer to) data to encrypt                 */
/*                  (IN) cbData  length of data to encrypt (must be multiple  */
/*                               of 8)                                        */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000131 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesEncryptOnce(ZCCRYPT_ALGO algo,
                                             ZCCRYPT_MODE mode,
                                             const BYTE*        pIV,
                                             const BYTE*        pKey,
                                             void *pData,
                                             DWORD cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptDesDecryptOnce                                     */
/* Description    : Decrypt n (multiple of 8 bytes / 64 bits) bytes           */
/* Parameter      : (IN) algo  encryption algorithm either ZCCRYPT_ALGO_DES   */
/*                             ZCCRYPT_ALGO_3DES                              */
/*                  (IN) mode  encryption chaining mode (ZCCRYPT_MODE_...)    */
/*                             either .._ECB, .._CBC, .._CFB or .._OFB        */
/*                  (IN) *pIV  8 byte initial vector or NULL for {0, .., 0}   */
/*                  (IN) *pKey 8 byte key for .._DES, 16 byte key for .._3DES */
/*                  (IN) pData   (pointer to) data to decrypt                 */
/*                  (IN) cbData  length of data to decrypt (must be multiple  */
/*                               of 8)                                        */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000131 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesDecryptOnce(ZCCRYPT_ALGO algo,
                                             ZCCRYPT_MODE mode,
                                             const BYTE*        pIV,
                                             const BYTE*        pKey,
                                             void *pData,
                                             DWORD cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptDesStartCertificate                                */
/* Description    : Start certificate                                         */
/* Parameter      : (OUT) *pCert     encryption context to use with further   */
/*                                   DES certificate function calls           */
/*                  (IN) algo  encryption algorithm either ZCCRYPT_ALGO_DES   */
/*                             ZCCRYPT_ALGO_3DES                              */
/*                  (IN) *pKey 8 byte key for .._DES, 16 byte key for .._3DES */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter (pKey==NULL)        */
/******************************************************************************/
/* History        : 20000128 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesStartCertificate(PZCCRYPT_CONTEXT_DES_CERT pCert,
                                                  ZCCRYPT_ALGO algo,
                                                  const BYTE*        pKey) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptDesUpdateCertificate                               */
/* Description    : Extend certificate by append (encrypt) pData              */
/* Parameter      : (IN) Cert  Certificate context created by                 */
/*                             ZCCryptDesStartCertificate                     */
/*                  (IN) pData   (pointer to) data to use for update          */
/*                  (IN) cbData   length of data                              */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000131 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesUpdateCertificate(ZCCRYPT_CONTEXT_DES_CERT Cert,
                                                   const void *pData,
                                                   DWORD cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptDesEndCertificate                                  */
/* Description    : End certificate and get resulting certificate             */
/* Parameter      : (IN) pCert  Certificate context created by                */
/*                               ZCCryptDesStartCertificate                   */
/*                  (OUT) pResult resulting certificate (8 bytes), caller     */
/*                                must pass valid pointer to 8 byte buffer    */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000131 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesEndCertificate(PZCCRYPT_CONTEXT_DES_CERT pCert,
                                               void *pResult) ZCEXPD


/******************************************************************************/
/* Function       : ZCCryptDesCertificateOnce                                 */
/* Description    : Create certificate of data (*pData) in one step           */
/* Parameter      : (IN) algo  encryption algorithm either ZCCRYPT_ALGO_DES   */
/*                             ZCCRYPT_ALGO_3DES                              */
/*                  (IN) *pKey 8 byte key for .._DES, 16 byte key for .._3DES */
/*                  (IN) pData   (pointer to) data to certificate             */
/*                  (IN) cbData  length of data to certificate                */
/*                  (OUT) pResult resulting certificate (8 bytes), caller     */
/*                                must pass valid pointer to 8 byte buffer    */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/*                  20000204 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesCertificateOnce(ZCCRYPT_ALGO algo,
                                             const BYTE*        pKey,
                                             const void *pData,
                                             DWORD cbData,
											 void *pResult) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptShaStart                                           */
/* Description    : Start SHA use in conjunction with ..ShaAppend, ..ShaEnd   */
/* Parameter      : (OUT) *pSHAContext SHA context to use with further        */
/*                                     function calls                         */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000204 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptShaStart(PZCCRYPT_CONTEXT_SHA pSHAContext) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptShaAppend                                          */
/* Description    : Append data to hash                                       */
/* Parameter      : (IN) SHAContext  context returned by ..ShaStart           */
/*                  (IN) pData       (pointer to) data to add                 */
/*                  (IN) cbData      length of data                           */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000204 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptShaAppend(ZCCRYPT_CONTEXT_SHA SHAContext,
										const void *pData,
										DWORD cbData) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptShaEnd                                             */
/* Description    : Close hash and get result (20 bytes)                      */
/* Parameter      : (IN) pSHAContext pointer to context returned by ..ShaStart*/
/*                  (OUT) pHash      resulting hash (20 bytes), caller must   */
/*                                   pass valid pointer to 20 byte buffer     */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000204 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptShaEnd(PZCCRYPT_CONTEXT_SHA pSHAContext,
								     void *pHash) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptSha                                                */
/* Description    : Do secure hash                                            */
/* Parameter      : (IN) pData       (pointer to) data to add                 */
/*                  (IN) cbData      length of data                           */
/*                  (OUT) pHash      resulting hash (20 bytes), caller must   */
/*                                   pass valid pointer to 20 byte buffer     */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000204 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptShaOnce(const void *pData,
									  DWORD cbData,
									  void *pHash) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptRandomInit                                         */
/* Description    : Initialize random generator                               */
/* Parameter      : none                                                      */
/* Returnvalue    : ZCCRYPT_NOERROR   no error                                */
/*                  ZCCRYPT_ERROR_MISC  unknown error occured                 */
/*                  ZCCRYPT_ERROR_RND_ALREADYINIT already initialized         */
/******************************************************************************/
/* History        : 20000210 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptRandomInit(void) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptRandomTerm                                         */
/* Description    : Terminate random generator (required if ZCCryptRandomInit */
/*                  was called)                                               */
/* Parameter      : none                                                      */
/* Returnvalue    : ZCCRYPT_NOERROR   no error                                */
/*                  ZCCRYPT_ERROR_RND_NOTINIT random not initialized          */
/*                  ZCCRYPT_ERROR_MISC  unknown error occured                 */
/******************************************************************************/
/* History        : 20000210 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptRandomTerm(void) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptRandomSeed                                         */
/* Description    : Seed random generator                                     */
/* Parameter      : (IN) *pSeed   random data to seed random generator        */
/*                  (IN) cbSeed   size  of seed in bytes                      */
/* Returnvalue    : ZCCRYPT_NOERROR   no error                                */
/*                  ZCCRYPT_ERROR_RND_NOTINIT random not initialized          */
/*                  ZCCRYPT_ERROR_MISC  unknown error occured                 */
/*                  ZCCRYPT_ERROR_PARM invalid pointer pSeed                  */
/******************************************************************************/
/* History        : 20000210 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptRandomSeed(const void *pSeed,
										 DWORD cbSeed) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptRandom                                             */
/* Description    : Get random data                                           */
/* Parameter      : (OUT) *pRandom   random data requested                    */
/*                  (IN)  cbRandom   number of requested bytes (max 40 byte)  */
/* Returnvalue    : ZCCRYPT_NOERROR   no error                                */
/*                  ZCCRYPT_ERROR_PARM to much random data requested or       */
/*                                     invalid pointer pRandom                */
/*                  ZCCRYPT_ERROR_RND_NOTINIT random not initialized          */
/*                  ZCCRYPT_ERROR_MISC  unknown error occured                 */
/******************************************************************************/
/* History        : 20000210 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptRandom(void *pRandom, DWORD cbRandom) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC161CreateContext                                 */
/* Description    : Create EC161 context for use with further EC calls from   */
/*                  number (1-3) of ZeitControl ECC curves                    */
/* Parameter      : (OUT) *pEC161  Elliptic Curve context to use with further */
/*                                 EC161 function calls                       */
/*                  (IN) iCurve number (1 to 3) of predefined curve           */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_MEM   out of memory                         */
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161CreateContext(PZCCRYPT_CONTEXT_EC161 pEC161,
												 int iCurve) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC161CreateContextFromParams                       */
/* Description    : Create EC161 context for use with further EC calls from   */
/*                  domain params                                             */
/* Parameter      : (OUT) *pEC161  Elliptic Curve context to use with further */
/*                                 EC161 function calls                       */
/*                  (IN) *pParams domain params                               */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_MEM   out of memory                         */
/*                  ZCCRYPT_ERROR_EC161_BADPARAMS bad ecc params (*pParams)   */
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161CreateContextFromParams
                                                (PZCCRYPT_CONTEXT_EC161 pEC161,
												 const ZCCRYPT_EC161DOMAINPARAMS *pParams) ECSTUB ZCEXPD


/******************************************************************************/
/* Function       : ZCCryptEC161DestroyContext                                */
/* Description    : Destroy EC161 context returned from                       */
/*                              ZCCryptEC161CreateContext or                  */
/*                              ZCCryptEC161CreateContextFromParams           */
/* Parameter      : (OUT) *pEC161  Elliptic Curve context to destroy          */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161DestroyContext(PZCCRYPT_CONTEXT_EC161 pEC161) ECSTUB ZCEXPD


/******************************************************************************/
/* Function       : ZCCryptEC161GetPublicKey                                  */
/* Description    : Get public key from private key                           */
/* Parameter      : (IN) EC161  context returned from                         */
/*                              ZCCryptEC161CreateContext or                  */
/*                              ZCCryptEC161CreateContextFromParams           */
/*                  (IN) *pPrivateKey 21 byte private key                     */
/*                  (OUT) *pPrivateKey 21 byte corrected private key          */
/*                  (OUT) *pPublicKey 22 byte public key                      */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC161_BADPARAMS invalid private key         */
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161GetPublicKey(ZCCRYPT_CONTEXT_EC161 Ec161,
												PZCCRYPT_EC161PRIVATEKEY pPrivateKey,
												PZCCRYPT_EC161PUBLICKEY pPublicKey) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC161SetupPrivateKey                               */
/* Description    : Make valid privat key from binary data                    */
/* Parameter      : (IN) EC161  context returned from                         */
/*                              ZCCryptEC161CreateContext or                  */
/*                              ZCCryptEC161CreateContextFromParams           */
/*                  (OUT) *pPrivateKey 21 byte private key                    */
/*                  (IN) *pKeyData     Key data to be used                    */
/*                  (IN) cbKeyData     Length of Key data                     */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC161_BADPARAMS invalid private key         */
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161SetupPrivateKey(ZCCRYPT_CONTEXT_EC161 Ec161,
											    	PZCCRYPT_EC161PRIVATEKEY pPrivateKey,
												    void *pKeyData,
												    DWORD cbKeyData) ECSTUB ZCEXPD


/******************************************************************************/
/* Function       : ZCCryptEC161GenerateKeyPair                               */
/* Description    : Get public key from private key                           */
/*                  You must have called ZCCryptRandomInit before             */
/* Parameter      : (IN) EC161  context returned from                         */
/*                              ZCCryptEC161CreateContext or                  */
/*                              ZCCryptEC161CreateContextFromParams           */
/*                  (OUT) *pPrivateKey 21 byte private key                    */
/*                  (OUT) *pPublicKey 22 byte public key                      */
/*                  (IN) *pSeed  seed for random generator or NULL            */
/*                               Note: It is strongly recommended to give some*/
/*                                     random seed                            */
/*                  (IN) cbSeed  size of seed in bytes                        */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_RND_NOTINIT random generator not initialized*/
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161GenerateKeyPair(ZCCRYPT_CONTEXT_EC161 Ec161,
												PZCCRYPT_EC161PRIVATEKEY pPrivateKey,
												PZCCRYPT_EC161PUBLICKEY pPublicKey,
												void *pSeed,
												DWORD cbSeed) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC161Verify                                        */
/* Description    : Verify EC161 signature                                    */
/* Parameter      : (IN) EC161  context returned from                         */
/*                              ZCCryptEC161CreateContext or                  */
/*                              ZCCryptEC161CreateContextFromParams           */
/*                  (IN) *pSignature 42 byte signature                        */
/*                  (IN) *pPublicKey signer's 22 byte public key              */
/*                  (IN) *pMessage 20 byte message to be verified             */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error, signature valid             */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC161_BADPARAMS invalid public key          */
/*                  ZCCRYPT_ERROR_EC161_BADSIGNATURE invalid signature        */
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161Verify(ZCCRYPT_CONTEXT_EC161 Ec161,
										  const ZCCRYPT_EC161SIGNATURE *pSignature,
										  const ZCCRYPT_EC161PUBLICKEY *pPublicKey,
										  const void *pMessage) ECSTUB ZCEXPD


/******************************************************************************/
/* Function       : ZCCryptEC161HashAndVerify                                 */
/* Description    : Hash message and verify EC161 signature                   */
/* Parameter      : (IN) EC161  context returned from                         */
/*                              ZCCryptEC161CreateContext or                  */
/*                              ZCCryptEC161CreateContextFromParams           */
/*                  (IN) *pSignature 42 byte signature                        */
/*                  (IN) *pPublicKey signer's 22 byte public key              */
/*                  (IN) *pMessage   message to be verified                   */
/*                  (IN) cbMessage   size of message in bytes                 */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error, signature valid             */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC161_BADPARAMS invalid public key          */
/*                  ZCCRYPT_ERROR_EC161_BADSIGNATURE invalid signature        */
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161HashAndVerify(ZCCRYPT_CONTEXT_EC161 Ec161,
										         const ZCCRYPT_EC161SIGNATURE *pSignature,
										         const ZCCRYPT_EC161PUBLICKEY *pPublicKey,
										         const void *pMessage,
										         DWORD cbMessage) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC161SharedSecret                                  */
/* Description    : Calculate shared secret                                   */
/* Parameter      : (IN) EC161  context returned from                         */
/*                              ZCCryptEC161CreateContext or                  */
/*                              ZCCryptEC161CreateContextFromParams           */
/*                  (IN) *pPublicKey other party's 22-byte public key         */
/*                  (IN) *pPrivateKey own 21 byte private key                 */
/*                  (OUT) *pSecret 20 byte shared secret                      */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error, signature valid             */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC161_BADPARAMS invalid key                 */
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161SharedSecret(ZCCRYPT_CONTEXT_EC161 Ec161,
												const ZCCRYPT_EC161PRIVATEKEY *pPrivateKey,
												const ZCCRYPT_EC161PUBLICKEY *pPublicKey,
												PZCCRYPT_EC161SHAREDSECRET pSecret) ECSTUB ZCEXPD


/******************************************************************************/
/* Function       : ZCCryptEC161SessionKey                                    */
/* Description    : Calculate a session key                                   */
/* Parameter      : (IN) EC161  context returned from                         */
/*                              ZCCryptEC161CreateContext or                  */
/*                              ZCCryptEC161CreateContextFromParams           */
/*                  (IN) *pPublicKey other party's 22-byte public key         */
/*                  (IN) *pPrivateKey own 21 byte private key                 */
/*                  (IN) *pKdp   key derivation parameter (binary data or     */
/*                               string of cbKdp byte length                  */
/*                  (IN) cbKdp   length of *pKdp                              */
/*                  (OUT) *pKey 20 byte session key                           */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error, signature valid             */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC161_BADPARAMS invalid key                 */
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161SessionKey(ZCCRYPT_CONTEXT_EC161 Ec161,
											  const ZCCRYPT_EC161PRIVATEKEY *pPrivateKey,
											  const ZCCRYPT_EC161PUBLICKEY *pPublicKey,
											  const void *pKdp,
											  DWORD cbKdp,
											  PZCCRYPT_EC161SESSIONKEY pKey) ECSTUB ZCEXPD


#endif /*  ZCCRYPT_VER_1_0 */

#ifdef ZCCRYPT_VER_1_2

/******************************************************************************/
/* Function       : ZCCryptAesCreateContext                                   */
/* Description    : Create encrytion context for use with AES128, AES192,     */
/*                  AES256                                                    */
/* Parameter      : (OUT) *pContext  encryption context to use with further   */
/*                                   AES function calls                       */
/*                  (IN) algo  encryption algorithm either                    */
/*                             ZCCRYPT_ALGO_AES128, ZCCRYPT_ALGO_AES192   or  */
/*                             ZCCRYPT_ALGO_AES256                            */
/*                  (IN) mode  encryption chaining mode (ZCCRYPT_MODE_...)    */
/*                             either .._ECB, .._CBC, .._CFB or .._OFB        */
/*                  (IN) *pIV  16 byte initial vector or NULL for {0, .., 0}  */
/*                  (IN) *pKey 16 byte key for .._AES128,                     */
/*                             24 byte key for .._AES192 or                   */
/*                             32 byte key for .._AES256                      */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter (pKey==NULL)        */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesCreateContext(PZCCRYPT_CONTEXT_AES pContext,
                                               ZCCRYPT_ALGO algo,
                                               ZCCRYPT_MODE mode,
                                               const BYTE*        pIV,
                                               const BYTE*        pKey) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptAesDestroyContext                                  */
/* Description    : Destroy encrytion context created by                      */
/*                  ZCCryptAesCreateContext this also destroy/clears all keys */
/* Parameter      : (IN) *pContext   encryption context to destroy            */
/*                                   AES function calls                       */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter (pKey==NULL)        */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesDestroyContext(PZCCRYPT_CONTEXT_AES pContext) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptAesEncrypt                                         */
/* Description    : Encrypt n (multiple of 16 bytes / 128 bits) bytes         */
/* Parameter      : (IN) Context  encryption context return by                */
/*                                ZCCryptAesCreateContext                     */
/*                  (IN) pData   (pointer to) data to encrypt                 */
/*                  (IN) dwLen   length of data to encrypt (must be multiple  */
/*                               of 16)                                       */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesEncrypt(ZCCRYPT_CONTEXT_AES Context,
                                         void *pData,
                                         DWORD dwLen) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptAesDecrypt                                         */
/* Description    : Decrypt n (multiple of 16 bytes / 128 bits) bytes         */
/* Parameter      : (IN) Context  encryption context return by                */
/*                                ZCCryptAesCreateContext                     */
/*                  (IN) pData   (pointer to) data to decrypt                 */
/*                  (IN) dwLen   length of data to encrypt (must be multiple  */
/*                               of 16)                                       */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesDecrypt(ZCCRYPT_CONTEXT_AES Context,
                                         void *pData,
                                         DWORD dwLen) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptAesSetIV                                           */
/* Description    : Set IV, maybe used to reset context to start IV           */
/* Parameter      : (IN) Context  encryption context return by                */
/*                                ZCCryptDesCreateContext                     */
/*                  (IN) *pIV  16 byte initial vector or NULL for {0, .., 0}  */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesSetIV(ZCCRYPT_CONTEXT_AES Context,
                                       const BYTE *pIV) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptAesGetIV                                           */
/* Description    : Get current active IV                                     */
/* Parameter      : (IN) Context  encryption context return by                */
/*                                ZCCryptAesCreateContext                     */
/*                  (OUT) *pIV  16 byte initial vector                         */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesGetIV(ZCCRYPT_CONTEXT_AES Context,
                                       BYTE *pIV) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptAesEncryptOnce                                     */
/* Description    : Encrypt n (multiple of 16 bytes / 128 bits) bytes         */
/*                  (IN) *pIV  16 byte initial vector or NULL for {0, .., 0}  */
/*                  (IN) *pKey 16 byte key for .._AES128,                     */
/*                             24 byte key for .._AES192 or                   */
/*                             32 byte key for .._AES256                      */
/* Parameter      : (IN) algo  encryption algorithm either                    */
/*                             ZCCRYPT_ALGO_AES128, ZCCRYPT_ALGO_AES192   or  */
/*                             ZCCRYPT_ALGO_AES256                            */
/*                  (IN) mode  encryption chaining mode (ZCCRYPT_MODE_...)    */
/*                             either .._ECB, .._CBC, .._CFB or .._OFB        */
/*                  (IN) *pIV  16 byte initial vector or NULL for {0, .., 0}  */
/*                  (IN) *pKey 16 byte key for .._AES128,                     */
/*                             24 byte key for .._AES192 or                   */
/*                             32 byte key for .._AES256                      */
/*                  (IN) pData   (pointer to) data to encrypt                 */
/*                  (IN) dwLen   length of data to encrypt (must be multiple  */
/*                               of 16)                                        */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20010120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesEncryptOnce(ZCCRYPT_ALGO algo,
                                             ZCCRYPT_MODE mode,
                                             const BYTE*        pIV,
                                             const BYTE*        pKey,
                                             void *pData,
                                             DWORD dwLen) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptAesDecryptOnce                                     */
/* Description    : Encrypt n (multiple of 16 bytes / 128 bits) bytes         */
/*                  (IN) *pIV  16 byte initial vector or NULL for {0, .., 0}  */
/*                  (IN) *pKey 16 byte key for .._AES128,                     */
/*                             24 byte key for .._AES192 or                   */
/*                             32 byte key for .._AES256                      */
/* Parameter      : (IN) algo  encryption algorithm either                    */
/*                             ZCCRYPT_ALGO_AES128, ZCCRYPT_ALGO_AES192   or  */
/*                             ZCCRYPT_ALGO_AES256                            */
/*                  (IN) mode  encryption chaining mode (ZCCRYPT_MODE_...)    */
/*                             either .._ECB, .._CBC, .._CFB or .._OFB        */
/*                  (IN) *pIV  16 byte initial vector or NULL for {0, .., 0}  */
/*                  (IN) *pKey 16 byte key for .._AES128,                     */
/*                             24 byte key for .._AES192 or                   */
/*                             32 byte key for .._AES256                      */
/*                  (IN) pData   (pointer to) data to decrypt                 */
/*                  (IN) dwLen   length of data to decrypt (must be multiple  */
/*                               of 16)                                        */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20010120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesDecryptOnce(ZCCRYPT_ALGO algo,
                                             ZCCRYPT_MODE mode,
                                             const BYTE*        pIV,
                                             const BYTE*        pKey,
                                             void *pData,
                                             DWORD dwLen) ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC167CreateContext                                 */
/* Description    : Create EC167 context for use with further EC calls from   */
/*                  number (1-5) of ZeitControl ECC167 curves                 */
/* Parameter      : (OUT) *pEC167  Elliptic Curve context to use with further */
/*                                 EC167 function calls                       */
/*                  (IN) iCurve number (1 to 5) of predefined curve           */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167CreateContext(PZCCRYPT_CONTEXT_EC167 pEC167,
												 int iCurve) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC167CreateContextFromParams                       */
/* Description    : Create EC167 context for use with further EC calls from   */
/*                  domain params                                             */
/* Parameter      : (OUT) *pEC167  Elliptic Curve context to use with further */
/*                                 EC167 function calls                       */
/*                  (IN) *pParams domain params                               */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_MEM   out of memory                         */
/*                  ZCCRYPT_ERROR_EC_BADPARAMS bad ecc params (*pParams)      */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167CreateContextFromParams
                                                (PZCCRYPT_CONTEXT_EC167 pEC167,
                                                 const ZCCRYPT_EC167DOMAINPARAMS *pParams) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC167DestroyContext                                */
/* Description    : Destroy EC167 context returned from                       */
/*                              ZCCryptEC167CreateContext or                  */
/*                              ZCCryptEC167CreateContextFromParams           */
/* Parameter      : (OUT) *pEC167  Elliptic Curve context to destroy          */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167DestroyContext(PZCCRYPT_CONTEXT_EC167 pEC167) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC167GetPublicKey                                  */
/* Description    : Get public key from private key                           */
/* Parameter      : (IN) EC167  context returned from                         */
/*                              ZCCryptEC167CreateContext or                  */
/*                              ZCCryptEC167CreateContextFromParams           */
/*                  (IN) *pPrivateKey 21 byte private key                     */
/*                  (OUT) *pPrivateKey 21 byte corrected private key          */
/*                  (OUT) *pPublicKey 21 byte public key                      */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC_BADPARAMS invalid private key            */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167GetPublicKey(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                PZCCRYPT_EC167PRIVATEKEY pPrivateKey,
                                                PZCCRYPT_EC167PUBLICKEY pPublicKey) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC167SetupPrivateKey                               */
/* Description    : Make valid privat key from binary data                    */
/* Parameter      : (IN) EC167  context returned from                         */
/*                              ZCCryptEC167CreateContext or                  */
/*                              ZCCryptEC167CreateContextFromParams           */
/*                  (OUT) *pPrivateKey 21 byte private key                    */
/*                  (IN) *pKeyData     Key data to be used                    */
/*                  (IN) cbKeyData     Length of Key data                     */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC_BADPARAMS invalid private key            */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167SetupPrivateKey(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                   PZCCRYPT_EC167PRIVATEKEY pPrivateKey,
                                                   void *pKeyData,
                                                   DWORD cbKeyData) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC167GenerateKeyPair                               */
/* Description    : Get public key from private key                           */
/*                  You must have called ZCCryptRandomInit before             */
/* Parameter      : (IN) EC167  context returned from                         */
/*                              ZCCryptEC167CreateContext or                  */
/*                              ZCCryptEC167CreateContextFromParams           */
/*                  (OUT) *pPrivateKey 21 byte private key                    */
/*                  (OUT) *pPublicKey 21 byte public key                      */
/*                  (IN) *pSeed  seed for random generator or NULL            */
/*                               Note: It is strongly recommended to give some*/
/*                                     random seed                            */
/*                  (IN) cbSeed  size of seed in bytes                        */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error                              */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_RND_NOTINIT random generator not initialized*/
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167GenerateKeyPair(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                   PZCCRYPT_EC167PRIVATEKEY pPrivateKey,
                                                   PZCCRYPT_EC167PUBLICKEY pPublicKey,
                                                   void *pSeed,
                                                   DWORD cbSeed) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC167Verify                                        */
/* Description    : Verify EC167 signature                                    */
/* Parameter      : (IN) EC167  context returned from                         */
/*                              ZCCryptEC167CreateContext or                  */
/*                              ZCCryptEC167CreateContextFromParams           */
/*                  (IN) *pSignature 42 byte signature                        */
/*                  (IN) *pPublicKey signer's 21 byte public key              */
/*                  (IN) *pMessage 20 byte message to be verified             */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error, signature valid             */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC_BADPARAMS invalid public key             */
/*                  ZCCRYPT_ERROR_EC_BADSIGNATURE invalid signature           */
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167Verify(ZCCRYPT_CONTEXT_EC167 Ec167,
                                          const ZCCRYPT_EC167SIGNATURE *pSignature,
                                          const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                          const void *pMessage) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC167HashAndVerify                                 */
/* Description    : Hash message and verify EC167 signature                   */
/* Parameter      : (IN) EC167  context returned from                         */
/*                              ZCCryptEC167CreateContext or                  */
/*                              ZCCryptEC167CreateContextFromParams           */
/*                  (IN) *pSignature 42 byte signature                        */
/*                  (IN) *pPublicKey signer's 21 byte public key              */
/*                  (IN) *pMessage   message to be verified                   */
/*                  (IN) cbMessage   size of message in bytes                 */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error, signature valid             */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC_BADPARAMS invalid public key             */
/*                  ZCCRYPT_ERROR_EC_BADSIGNATURE invalid signature           */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167HashAndVerify(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167SIGNATURE *pSignature,
                                                 const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC167SharedSecret                                  */
/* Description    : Calculate shared secret                                   */
/* Parameter      : (IN) EC167  context returned from                         */
/*                              ZCCryptEC167CreateContext or                  */
/*                              ZCCryptEC167CreateContextFromParams           */
/*                  (IN) *pPublicKey other party's 21-byte public key         */
/*                  (IN) *pPrivateKey own 21 byte private key                 */
/*                  (OUT) *pSecret 20 byte shared secret                      */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error, signature valid             */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC_BADPARAMS invalid key                    */
/******************************************************************************/
/* History        : 20000208 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167SharedSecret(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                                const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                                PZCCRYPT_EC167SHAREDSECRET pSecret) ECSTUB ZCEXPD

/******************************************************************************/
/* Function       : ZCCryptEC167SessionKey                                    */
/* Description    : Calculate a session key                                   */
/* Parameter      : (IN) EC167  context returned from                         */
/*                              ZCCryptEC167CreateContext or                  */
/*                              ZCCryptEC167CreateContextFromParams           */
/*                  (IN) *pPublicKey other party's 21-byte public key         */
/*                  (IN) *pPrivateKey own 21 byte private key                 */
/*                  (IN) *pKdp   key derivation parameter (binary data or     */
/*                               string of cbKdp byte length                  */
/*                  (IN) cbKdp   length of *pKdp                              */
/*                  (OUT) *pKey 20 byte session key                           */
/* Returnvalue    : ZCCRYPT_NOERROR     no_error, signature valid             */
/*                  ZCCRYPT_ERROR_PARM  invalid parameter                     */
/*                  ZCCRYPT_ERROR_EC_BADPARAMS invalid key                    */
/******************************************************************************/
/* History        : 20030120 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167SessionKey(ZCCRYPT_CONTEXT_EC167 Ec167,
                                              const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                              const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                              const void *pKdp,
                                              DWORD cbKdp,
                                              PZCCRYPT_EC167SESSIONKEY pKey) ECSTUB ZCEXPD

#endif /*  ZCCRYPT_VER_1_1 */


#ifdef __cplusplus
}
#endif

#endif
