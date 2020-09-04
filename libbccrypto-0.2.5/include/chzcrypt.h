#ifndef __CHZCRYPT_H__
#define __CHZCRYPT_H__

#include "zccrypt.h"

#define CHZCRYPT_VER 0x025
#define CHZCRYPT_VER_MASK 0xffff
#define CHZCRYPT_EC_SUPPORT 0x10000

#ifdef __cplusplus
extern "C" {
#endif
	
#define CHZCRYPT_ERROR_EC211_BASE          ZCCRYPT_ERROR_EC_BASE
#define CHZCRYPT_ERROR_EC211_BADPARAMS     ZCCRYPT_ERROR_EC_BADPARAMS
#define CHZCRYPT_ERROR_EC211_BADSIGNATURE  ZCCRYPT_ERROR_EC_BADSIGNATURE

#pragma pack(1)
typedef enum _CHZCRYPT_ALGO {
    CHZCRYPT_ALGO_EAXAES128=0x41,
    CHZCRYPT_ALGO_EAXAES192=0x42,
    CHZCRYPT_ALGO_EAXAES256=0x43,
    CHZCRYPT_ALGO_OMACAES128=0x81,
    CHZCRYPT_ALGO_OMACAES192=0x82,
    CHZCRYPT_ALGO_OMACAES256=0x83,
    CHZCRYPT_ALGO_ECC_NR=0x100,
    CHZCRYPT_ALGO_ECC_DSA=0x101,
    CHZCRYPT_ALGO_DUMMY=0x7FFFFFFF  /* force enum to use 32 bit values */
} CHZCRYPT_ALGO;

typedef void *CHZCRYPT_CONTEXT_OMAC;
typedef CHZCRYPT_CONTEXT_OMAC *PCHZCRYPT_CONTEXT_OMAC;

typedef void *CHZCRYPT_CONTEXT_EAX;
typedef CHZCRYPT_CONTEXT_EAX *PCHZCRYPT_CONTEXT_EAX;

typedef struct _CHZCRYPT_EC211DOMAINPARAMS
  {
  BYTE a ;
  BYTE b[27] ;
  BYTE r[27] ;
  BYTE G[27] ;
  } CHZCRYPT_EC211DOMAINPARAMS;
typedef CHZCRYPT_EC211DOMAINPARAMS *PCHZCRYPT_EC211DOMAINPARAMS;
typedef BYTE CHZCRYPT_EC211PRIVATEKEY[27];
typedef BYTE *PCHZCRYPT_EC211PRIVATEKEY;
typedef BYTE CHZCRYPT_EC211PUBLICKEY[27];
typedef BYTE *PCHZCRYPT_EC211PUBLICKEY;
typedef BYTE CHZCRYPT_EC211SIGNATURE[54];
typedef BYTE *PCHZCRYPT_EC211SIGNATURE;
typedef BYTE CHZCRYPT_EC211SHAREDSECRET[27];
typedef BYTE *PCHZCRYPT_EC211SHAREDSECRET;
typedef BYTE CHZCRYPT_EC211SESSIONKEY[32];
typedef BYTE *PCHZCRYPT_EC211SESSIONKEY;
typedef void *CHZCRYPT_CONTEXT_EC211;
typedef CHZCRYPT_CONTEXT_EC211 *PCHZCRYPT_CONTEXT_EC211;

#pragma pack()

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptGetVer(DWORD *pVer) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptSha256Start(PZCCRYPT_CONTEXT_SHA pSHAContext) ZCEXPD


ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptSha256Append(ZCCRYPT_CONTEXT_SHA SHAContext,
                                                     const void *pData,
                                                     DWORD cbData) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptSha256End(PZCCRYPT_CONTEXT_SHA pSHAContext,
                                                  void *pHash) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptSha256Once(const void *pData,
                                                   DWORD cbData,
                                                   void *pHash) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptOMACStart(PCHZCRYPT_CONTEXT_OMAC pOmac,
                                                  CHZCRYPT_ALGO algo, const BYTE* pKey) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptOMACUpdate(CHZCRYPT_CONTEXT_OMAC Omac,
                                                   const void* pData, DWORD cbData) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptOMACEnd(PCHZCRYPT_CONTEXT_OMAC pOmac, void* pTag) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptOMACOnce(CHZCRYPT_ALGO algo, const BYTE* pKey,
                                                 const void* pData, DWORD cbData,
                                                 void* pTag) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXStart(PCHZCRYPT_CONTEXT_EAX pEax,
                                                  CHZCRYPT_ALGO algo, const BYTE* pKey,
                                                  const void* pNonce, DWORD cbNonce,
                                                  const void* pHeader, DWORD cbHeader) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXAddHeader(CHZCRYPT_CONTEXT_EAX Eax,
                                                  const void* pHeader, DWORD cbHeader) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXEncrypt(CHZCRYPT_CONTEXT_EAX Eax,
                                                 void* pData, DWORD cbData) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXDecrypt(CHZCRYPT_CONTEXT_EAX Eax,
                                                 void* pData, DWORD cbData) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXEnd(PCHZCRYPT_CONTEXT_EAX pEax, void* pTag) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXEncryptOnce(CHZCRYPT_ALGO algo, const BYTE* pKey,
                                                 const void* pNonce, DWORD cbNonce,
                                                 const void* pHeader, DWORD cbHeader,
                                                 void* pData, DWORD cbData,
                                                 void* pTag) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXDecryptOnce(CHZCRYPT_ALGO algo, const BYTE* pKey,
                                                 const void* pNonce, DWORD cbNonce,
                                                 const void* pHeader, DWORD cbHeader,
                                                 void* pData, DWORD cbData,
                                                 void* pTag) ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167VerifyAlg(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167SIGNATURE *pSignature,
                                                 const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                                 const void *pMessage, CHZCRYPT_ALGO algo) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167HashAndVerifyAlg(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167SIGNATURE *pSignature,
                                                 const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage,  CHZCRYPT_ALGO algo) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167Sign(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 ZCCRYPT_EC167SIGNATURE *pSignature) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167SignAlg(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 ZCCRYPT_EC167SIGNATURE *pSignature, CHZCRYPT_ALGO algo) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC161Sign(ZCCRYPT_CONTEXT_EC161 Ec161,
                                                 const ZCCRYPT_EC161PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 ZCCRYPT_EC161SIGNATURE *pSignature) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167HashAndSign(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage,
                                                 ZCCRYPT_EC167SIGNATURE *pSignature) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167HashAndSignAlg(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage,
                                                 ZCCRYPT_EC167SIGNATURE *pSignature, CHZCRYPT_ALGO algo) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC161HashAndSign(ZCCRYPT_CONTEXT_EC161 Ec161,
                                                 const ZCCRYPT_EC161PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage,
                                                 ZCCRYPT_EC161SIGNATURE *pSignature) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211CreateContext(PCHZCRYPT_CONTEXT_EC211 pEC211,
                                                           int iCurve) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211CreateContextFromParams
                                                (PCHZCRYPT_CONTEXT_EC211 pEC211,
                                                 const CHZCRYPT_EC211DOMAINPARAMS *pParams) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211DestroyContext(PCHZCRYPT_CONTEXT_EC211 pEC211) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211GetPublicKey(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                PCHZCRYPT_EC211PRIVATEKEY pPrivateKey,
                                                PCHZCRYPT_EC211PUBLICKEY pPublicKey) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211SetupPrivateKey(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                   PCHZCRYPT_EC211PRIVATEKEY pPrivateKey,
                                                   void *pKeyData,
                                                   DWORD cbKeyData) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211GenerateKeyPair(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                            PCHZCRYPT_EC211PRIVATEKEY pPrivateKey,
                                                            PCHZCRYPT_EC211PUBLICKEY pPublicKey,
                                                            void *pSeed,
                                                            DWORD cbSeed) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211VerifyAlg(CHZCRYPT_CONTEXT_EC211 Ec211,
                                          const CHZCRYPT_EC211SIGNATURE *pSignature,
                                          const CHZCRYPT_EC211PUBLICKEY *pPublicKey,
                                          const void *pMessage, CHZCRYPT_ALGO algo) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211HashAndVerifyAlg(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                 const CHZCRYPT_EC211SIGNATURE *pSignature,
                                                 const CHZCRYPT_EC211PUBLICKEY *pPublicKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage, CHZCRYPT_ALGO algo) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211SignAlg(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                 const CHZCRYPT_EC211PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 CHZCRYPT_EC211SIGNATURE *pSignature, CHZCRYPT_ALGO algo) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211HashAndSignAlg(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                 const CHZCRYPT_EC211PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage,
                                                 CHZCRYPT_EC211SIGNATURE *pSignature, CHZCRYPT_ALGO algo) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211SharedSecret(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                const CHZCRYPT_EC211PRIVATEKEY *pPrivateKey,
                                                const CHZCRYPT_EC211PUBLICKEY *pPublicKey,
                                                PCHZCRYPT_EC211SHAREDSECRET pSecret) ECSTUB ZCEXPD

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211SessionKey(CHZCRYPT_CONTEXT_EC211 Ec211,
                                              const CHZCRYPT_EC211PRIVATEKEY *pPrivateKey,
                                              const CHZCRYPT_EC211PUBLICKEY *pPublicKey,
                                              const void *pKdp,
                                              DWORD cbKdp,
                                              PCHZCRYPT_EC211SESSIONKEY pKey) ECSTUB ZCEXPD


#ifdef __cplusplus
}
#endif

#endif
