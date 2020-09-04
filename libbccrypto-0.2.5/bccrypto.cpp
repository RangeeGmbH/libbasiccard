/***************************************************************************
                          bccrypto.cpp  -  description
                             -------------------
    begin                : Mi Mär 01 2006
    copyright            : (C) 2006 by Christian Zietz
    email                : czietz@gmx.net
 ***************************************************************************/

#include <string.h> 
#include <time.h>

// for data type definitions
#ifdef _WIN32
#include <windows.h>
#else
#include <chzcrwt.h>
#endif
 
#include <tomcrypt.h>

#ifdef ECC_BASICCARD
#include "ecc.h"
#else
#define ECSTUB { return ZCCRYPT_ERROR_MISC; }
#endif

#define ALL_MODES
#include "zccrypt.h"
#include "chzcrypt.h"

// entropy to seed PRNG with (if PRNG is used)
#ifdef USE_PRNG
#ifndef BITS_OF_ENTROPY
#define BITS_OF_ENTROPY 512
#endif
#endif

#define CONTEXT_MAGIC 0xe4031945
#define CHECK_CONTEXT(c) { if (c->magic != CONTEXT_MAGIC) return ZCCRYPT_ERROR_PARM; }

struct cry_int_context {
  unsigned long magic;
  ZCCRYPT_ALGO algo;
  ZCCRYPT_MODE mode;
  int cipher;
  union {
    symmetric_ECB ecb;
    symmetric_CBC cbc;
    symmetric_CFB cfb;
    symmetric_OFB ofb;
    omac_state omac;
    eax_state eax;
  } data;
  unsigned char certbuffer[8];
  unsigned char certdata[8];
  int certlen;
};

struct cry_int_sha_context {
  hash_state data;
  unsigned long magic;
};

prng_state cry_int_prng;
int cry_int_prng_started = 0;

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptGetVer(DWORD *pVer) {
  *pVer = ZCCRYPT_VER;
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptGetVer(DWORD *pVer) {
  *pVer = CHZCRYPT_VER;
  #ifdef ECC_BASICCARD
  *pVer |= CHZCRYPT_EC_SUPPORT;
  #endif
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesCreateContext(PZCCRYPT_CONTEXT_DES pContext,
                                               ZCCRYPT_ALGO algo, 
                                               ZCCRYPT_MODE mode,
                                               const BYTE* pIV,
                                               const BYTE* pKey) {
  
  int cipher, keylen, res;
  unsigned char nullIV[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  unsigned char keydata[32];
  
  register_cipher(&des_desc);
  register_cipher(&des3_desc);
  register_cipher(&aes_desc);

  if (pKey == NULL) {
    return ZCCRYPT_ERROR_PARM;
  } else {
    memcpy(keydata, pKey, sizeof(keydata));
  }
  
  switch (algo) {
    case ZCCRYPT_ALGO_3DES:
      cipher = find_cipher("3des");
      keylen = 24;
      // different definitions of a 3DES key in ZCCRYPT and libtomcrypt
      memcpy(keydata+16, pKey, 8);
      break;
    case ZCCRYPT_ALGO_DES:
      cipher = find_cipher("des");
      keylen = 8;
      break;
    case ZCCRYPT_ALGO_AES128:
      cipher = find_cipher("aes");
      keylen = 16;
      break;
    case ZCCRYPT_ALGO_AES192:
      cipher = find_cipher("aes");
      keylen = 24;
      break;
    case ZCCRYPT_ALGO_AES256:
      cipher = find_cipher("aes");
      keylen = 32;
      break;      
    default:
      return ZCCRYPT_ERROR_PARM;
      break;
  } 
  
  cry_int_context* context = new cry_int_context;
  context->algo = algo;
  context->mode = mode;
  context->cipher = cipher;
  context->magic = CONTEXT_MAGIC;
  
  switch (mode) {
    case ZCCRYPT_MODE_ECB:
      res = ecb_start(cipher, keydata, keylen, 0, &context->data.ecb);
      break;
    case ZCCRYPT_MODE_CBC:
      res = cbc_start(cipher, pIV==NULL?nullIV:pIV, keydata, keylen, 0, &context->data.cbc); 
      break;
    case ZCCRYPT_MODE_CFB:
      res = cfb_start(cipher, pIV==NULL?nullIV:pIV, keydata, keylen, 0, &context->data.cfb); 
      break;
    case ZCCRYPT_MODE_OFB: 
      res = ofb_start(cipher, pIV==NULL?nullIV:pIV, keydata, keylen, 0, &context->data.ofb); 
      break;
    default:
      res = -1;
      break;
  }
  
  *pContext = (ZCCRYPT_CONTEXT_DES)context;
  
  if (res!=CRYPT_OK) {
    delete context;
    return ZCCRYPT_ERROR_PARM;
  } else {  
    return ZCCRYPT_NOERROR;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesSetIV(ZCCRYPT_CONTEXT_DES Context,
                                                const BYTE *pIV) {
  unsigned char nullIV[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
  int res;                                               
  cry_int_context* context = (cry_int_context *)Context;
  CHECK_CONTEXT(context);

  res = CRYPT_OK;
  
  switch (context->mode) {
    case ZCCRYPT_MODE_CBC:
      res = cbc_setiv(pIV==NULL?nullIV:pIV, cipher_descriptor[context->cipher].block_length, &context->data.cbc);
      break;
    case ZCCRYPT_MODE_CFB:
      res = cfb_setiv(pIV==NULL?nullIV:pIV, cipher_descriptor[context->cipher].block_length, &context->data.cfb);
      break;
    case ZCCRYPT_MODE_OFB:
      res = ofb_setiv(pIV==NULL?nullIV:pIV, cipher_descriptor[context->cipher].block_length, &context->data.ofb);
      break;
  }
    
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
} 

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesGetIV(ZCCRYPT_CONTEXT_DES Context,
                                                BYTE *pIV) {
      
  cry_int_context* context = (cry_int_context *)Context;
  int res;
  CHECK_CONTEXT(context);  
  unsigned long dummy;
  
  res = CRYPT_OK;
  
  switch (context->mode) {
    case ZCCRYPT_MODE_CBC:
      res = cbc_getiv(pIV, &dummy, &context->data.cbc);
      break;
    case ZCCRYPT_MODE_CFB:
      res = cfb_getiv(pIV, &dummy, &context->data.cfb);
      break;
    case ZCCRYPT_MODE_OFB:
      res = ofb_getiv(pIV, &dummy, &context->data.ofb);
      break;
  }
    
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
} 

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesDestroyContext(PZCCRYPT_CONTEXT_DES pContext) {
  cry_int_context* context = (cry_int_context *)(*pContext);
  CHECK_CONTEXT(context);
 
  switch (context->mode) {
    case ZCCRYPT_MODE_ECB:
      ecb_done(&context->data.ecb);
      break;
    case ZCCRYPT_MODE_CBC:
      cbc_done(&context->data.cbc);
      break;
    case ZCCRYPT_MODE_CFB:
      cfb_done(&context->data.cfb);
      break;
    case ZCCRYPT_MODE_OFB: 
      ofb_done(&context->data.ofb); 
      break;
  }
  
  context->magic = 0; // invalidate context
  delete context;
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesEncrypt(ZCCRYPT_CONTEXT_DES Context,
                                                  void *pData,
                                                  DWORD cbLen) {
  cry_int_context* context = (cry_int_context *)Context;
  int res;
  CHECK_CONTEXT(context); 

  if (cbLen % cipher_descriptor[context->cipher].block_length != 0) {
    return ZCCRYPT_ERROR_PARM;
  }
  
  unsigned char *buffer = new unsigned char[cbLen];
  
  switch (context->mode) {
    case ZCCRYPT_MODE_ECB:
      res = ecb_encrypt((unsigned char *)pData, buffer, cbLen, &context->data.ecb); 
      break;
    case ZCCRYPT_MODE_CBC:
      res = cbc_encrypt((unsigned char *)pData, buffer, cbLen, &context->data.cbc);
      break;
    case ZCCRYPT_MODE_CFB:
      res = cfb_encrypt((unsigned char *)pData, buffer, cbLen, &context->data.cfb);
      break;
    case ZCCRYPT_MODE_OFB: 
      res = ofb_encrypt((unsigned char *)pData, buffer, cbLen, &context->data.ofb);
      break;
    default:
      res = -1;
      break;
  }

  memcpy(pData, buffer, cbLen);
  delete buffer;
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesDecrypt(ZCCRYPT_CONTEXT_DES Context,
                                                  void *pData,
                                                  DWORD cbLen) {
  cry_int_context* context = (cry_int_context *)Context;
  int res;
  CHECK_CONTEXT(context); 

  if (cbLen % cipher_descriptor[context->cipher].block_length != 0) {
    return ZCCRYPT_ERROR_PARM;
  }
  
  unsigned char *buffer = new unsigned char[cbLen];
  
  switch (context->mode) {
    case ZCCRYPT_MODE_ECB:
      res = ecb_decrypt((unsigned char *)pData, buffer, cbLen, &context->data.ecb); 
      break;
    case ZCCRYPT_MODE_CBC:
      res = cbc_decrypt((unsigned char *)pData, buffer, cbLen, &context->data.cbc);
      break;
    case ZCCRYPT_MODE_CFB:
      res = cfb_decrypt((unsigned char *)pData, buffer, cbLen, &context->data.cfb);
      break;
    case ZCCRYPT_MODE_OFB: 
      res = ofb_decrypt((unsigned char *)pData, buffer, cbLen, &context->data.ofb);
      break;
    default:
      res = -1;
      break;      
  }

  memcpy(pData, buffer, cbLen);
  delete buffer;
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
}


ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesEncryptOnce(ZCCRYPT_ALGO algo, 
                                                      ZCCRYPT_MODE mode,
                                                      const BYTE*        pIV,
                                                      const BYTE*        pKey,
                                                      void *pData,
                                                      DWORD cbData) {
  ZCCRYPTRET res;
  ZCCRYPT_CONTEXT_DES c;
  
  if ((res = ZCCryptDesCreateContext(&c, algo, mode, pIV, pKey))!=ZCCRYPT_NOERROR) {
    return res;
  }
  
  res = ZCCryptDesEncrypt(c, pData, cbData);
  ZCCryptDesDestroyContext(&c);
  return res;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesDecryptOnce(ZCCRYPT_ALGO algo, 
                                                      ZCCRYPT_MODE mode,
                                                      const BYTE*        pIV,
                                                      const BYTE*        pKey,
                                                      void *pData,
                                                      DWORD cbData) {
  ZCCRYPTRET res;
  ZCCRYPT_CONTEXT_DES c;
  
  if ((res = ZCCryptDesCreateContext(&c, algo, mode, pIV, pKey))!=ZCCRYPT_NOERROR) {
    return res;
  }
  
  res = ZCCryptDesDecrypt(c, pData, cbData);
  ZCCryptDesDestroyContext(&c);
  return res;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesStartCertificate(PZCCRYPT_CONTEXT_DES_CERT pCert,
                                                           ZCCRYPT_ALGO algo, 
                                                           const BYTE* pKey) {
  int i;   
  
  if (algo>ZCCRYPT_ALGO_3DES) {
    return ZCCRYPT_ERROR_PARM;
  }
                                                         
  ZCCRYPTRET res = ZCCryptDesCreateContext(pCert, algo, ZCCRYPT_MODE_ECB, NULL, pKey);
  if (res==ZCCRYPT_NOERROR) {
    for (i=0; i<8; i++) {
      ((cry_int_context *)(*pCert))->certdata[i] = 0;
    }
    ((cry_int_context *)(*pCert))->certlen = 0;
  }
  return res;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesUpdateCertificate(ZCCRYPT_CONTEXT_DES_CERT Cert,
                                                            const void *pData,
                                                            DWORD cbData) {
  cry_int_context* context = (cry_int_context *)Cert;
  ZCCRYPTRET res;
  int i;
  unsigned char *data = (unsigned char *)pData;
  
  CHECK_CONTEXT(context); 
  
  res = ZCCRYPT_NOERROR;
  while (cbData>0) {
    context->certbuffer[context->certlen] = *data;
    context->certlen++;
    data++;
    cbData--;
    if (context->certlen == 8) {
      for (i=0; i<8; i++) {
        context->certdata[i] ^= context->certbuffer[i];
      }
      res = ZCCryptDesEncrypt(Cert, context->certdata, context->certlen);
      if (res!=ZCCRYPT_NOERROR) {
        break;
      }
      context->certlen = 0;
    }  
  }
  
  return res;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesEndCertificate(PZCCRYPT_CONTEXT_DES_CERT pCert,
                                                         void *pResult) {
  cry_int_context* context = (cry_int_context *)(*pCert);
  ZCCRYPTRET res;
  int i;
  CHECK_CONTEXT(context); 
  
  context->certbuffer[context->certlen] = 0x80;
  for (i=context->certlen+1; i<8; i++) {
    context->certbuffer[i] = 0;
  }
  
  for (i=0; i<8; i++) {
    context->certdata[i] ^= context->certbuffer[i];
  }
  res = ZCCryptDesEncrypt(*pCert, context->certdata, 8);  
  
  memcpy(pResult, context->certdata, 8);
  ZCCryptDesDestroyContext(pCert);
  
  return res;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptDesCertificateOnce(ZCCRYPT_ALGO algo, 
                                                         const BYTE*        pKey,
                                                         const void *pData,
                                                         DWORD cbData,
                                                         void *pResult) {
  ZCCRYPT_CONTEXT_DES_CERT c;
  ZCCRYPTRET res;
  
  if ((res = ZCCryptDesStartCertificate(&c, algo, pKey)) != ZCCRYPT_NOERROR) {
    return res;
  }
  
  res = ZCCryptDesUpdateCertificate(c, pData, cbData);
  ZCCryptDesEndCertificate(&c, pResult);
  
  return res;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptShaStart(PZCCRYPT_CONTEXT_SHA pSHAContext) {
  cry_int_sha_context *context = new cry_int_sha_context;
  int res;
  
  register_hash(&sha1_desc);
  context->magic = CONTEXT_MAGIC;
  res = sha1_init(&context->data);
  
  *pSHAContext = (ZCCRYPT_CONTEXT_SHA)context;
  
  if (res!=CRYPT_OK) {
    delete context;
    return ZCCRYPT_ERROR_MISC;
  } else {
    return ZCCRYPT_NOERROR;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptShaAppend(ZCCRYPT_CONTEXT_SHA SHAContext,
                                                 const void *pData,
                                                 DWORD cbData) {
  cry_int_sha_context* context = (cry_int_sha_context *)SHAContext;
  int res;
  CHECK_CONTEXT(context);
  
  res = sha1_process(&context->data, (unsigned char *)pData, cbData);

  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptShaEnd(PZCCRYPT_CONTEXT_SHA pSHAContext,
                                              void *pHash) {
  cry_int_sha_context* context = (cry_int_sha_context *)(*pSHAContext);
  int res;
  CHECK_CONTEXT(context);
  
  res = sha1_done(&context->data, (unsigned char *)pHash);
  
  context->magic = 0; // invalidate context
  delete context;
  
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
} 

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptShaOnce(const void *pData,
                                               DWORD cbData,
                                               void *pHash) {
  ZCCRYPT_CONTEXT_SHA c;
  ZCCRYPTRET res;
  
  if ((res = ZCCryptShaStart(&c)) != ZCCRYPT_NOERROR) {
    return res;
  }
  
  res = ZCCryptShaAppend(c, pData, cbData);
  ZCCryptShaEnd(&c, pHash);
  
  return res;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptSha256Start(PZCCRYPT_CONTEXT_SHA pSHAContext) {
  cry_int_sha_context *context = new cry_int_sha_context;
  int res;
  
  register_hash(&sha256_desc);
  context->magic = CONTEXT_MAGIC;
  res = sha256_init(&context->data);
  
  *pSHAContext = (ZCCRYPT_CONTEXT_SHA)context;
  
  if (res!=CRYPT_OK) {
    delete context;
    return ZCCRYPT_ERROR_MISC;
  } else {
    return ZCCRYPT_NOERROR;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptSha256Append(ZCCRYPT_CONTEXT_SHA SHAContext,
                                                     const void *pData,
                                                     DWORD cbData) {
  cry_int_sha_context* context = (cry_int_sha_context *)SHAContext;
  int res;
  CHECK_CONTEXT(context);
  
  res = sha256_process(&context->data, (unsigned char *)pData, cbData);

  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptSha256End(PZCCRYPT_CONTEXT_SHA pSHAContext,
                                                  void *pHash) {
  cry_int_sha_context* context = (cry_int_sha_context *)(*pSHAContext);
  int res;
  CHECK_CONTEXT(context);
  
  res = sha256_done(&context->data, (unsigned char *)pHash);
  
  context->magic = 0; // invalidate context
  delete context;
  
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
} 

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptSha256Once(const void *pData,
                                                   DWORD cbData,
                                                   void *pHash) {
  ZCCRYPT_CONTEXT_SHA c;
  ZCCRYPTRET res;
  
  if ((res = CHZCryptSha256Start(&c)) != ZCCRYPT_NOERROR) {
    return res;
  }
  
  res = CHZCryptSha256Append(c, pData, cbData);
  CHZCryptSha256End(&c, pHash);
  
  return res;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptRandomInit(void) {
  int res;
  time_t ent;
  
  if (!cry_int_prng_started) {
    register_prng(&fortuna_desc);
    register_prng(&sprng_desc);
#ifdef USE_PRNG 
#if (BITS_OF_ENTROPY)   
    res = rng_make_prng(BITS_OF_ENTROPY, find_prng("fortuna"), &cry_int_prng, NULL);
#else
#warning Without real entropy the PRNG wont be secure unless you seed it
    fortuna_start(&cry_int_prng);
    ent = time(NULL);
    fortuna_add_entropy((unsigned char *)(&ent), sizeof(ent), &cry_int_prng);
    res = fortuna_ready(&cry_int_prng);
#endif    
#else
    res = CRYPT_OK;
#endif
    if (res != CRYPT_OK) {
      return ZCCRYPT_ERROR_MISC;
    }
  }
  
  // increment usage count
  cry_int_prng_started++;
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptRandomTerm(void) {
  int res;

  if (cry_int_prng_started) {
    // decrement usage count
    cry_int_prng_started--;
    if (cry_int_prng_started == 0) {
#ifdef USE_PRNG
      res = fortuna_done(&cry_int_prng);
#else
      res = CRYPT_OK;
#endif
    } else {
      res = CRYPT_OK;
    }
    return (res != CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
  } else {
    return ZCCRYPT_ERROR_RND_NOTINIT;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptRandomSeed(const void *pSeed,
                                                  DWORD cbSeed) {
  int res;
  unsigned char *s = (unsigned char *)pSeed;
  
  if (cry_int_prng_started) {
#ifdef USE_PRNG
    while (cbSeed>0) { 
      fortuna_add_entropy(s, cbSeed>32?32:cbSeed, &cry_int_prng);
      cbSeed -= 32;
      s += 32;
    }
    res = fortuna_ready(&cry_int_prng);
#else
    res = CRYPT_OK;
#endif 
    return (res != CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
  } else {
    return ZCCRYPT_ERROR_RND_NOTINIT;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptRandom(void *pRandom, DWORD cbRandom) {
  int res;
  
  if (cry_int_prng_started) {
#ifdef USE_PRNG  
    res = fortuna_read((unsigned char *)pRandom, cbRandom, &cry_int_prng);
#else
    res = sprng_read((unsigned char *)pRandom, cbRandom, &cry_int_prng);
#endif    
    return (res != (int)cbRandom)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
  } else {
    return ZCCRYPT_ERROR_RND_NOTINIT;
  }
} 

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesCreateContext(PZCCRYPT_CONTEXT_AES pContext,
                                                        ZCCRYPT_ALGO algo, 
                                                        ZCCRYPT_MODE mode,
                                                        const BYTE* pIV,
                                                        const BYTE* pKey) {
  return ZCCryptDesCreateContext((PZCCRYPT_CONTEXT_DES)pContext, algo, mode, pIV, pKey);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesDestroyContext(PZCCRYPT_CONTEXT_AES pContext) {
  return ZCCryptDesDestroyContext((PZCCRYPT_CONTEXT_DES)pContext);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesEncrypt(ZCCRYPT_CONTEXT_AES Context,
                                                  void *pData,
                                                  DWORD dwLen) {
  return ZCCryptDesEncrypt((ZCCRYPT_CONTEXT_DES)Context, pData, dwLen);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesDecrypt(ZCCRYPT_CONTEXT_AES Context,
                                                  void *pData,
                                                  DWORD dwLen) {
  return ZCCryptDesDecrypt((ZCCRYPT_CONTEXT_DES)Context, pData, dwLen);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesSetIV(ZCCRYPT_CONTEXT_AES Context,
                                                const BYTE *pIV) {
  return ZCCryptDesSetIV((ZCCRYPT_CONTEXT_DES)Context, pIV);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesGetIV(ZCCRYPT_CONTEXT_AES Context,
                                                BYTE *pIV) {
  return ZCCryptDesGetIV((ZCCRYPT_CONTEXT_DES)Context, pIV);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesEncryptOnce(ZCCRYPT_ALGO algo, 
                                                      ZCCRYPT_MODE mode,
                                                      const BYTE* pIV,
                                                      const BYTE* pKey,
                                                      void *pData,
                                                      DWORD dwLen) {
  return ZCCryptDesEncryptOnce(algo, mode, pIV, pKey, pData, dwLen);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptAesDecryptOnce(ZCCRYPT_ALGO algo, 
                                                      ZCCRYPT_MODE mode,
                                                      const BYTE* pIV,
                                                      const BYTE* pKey,
                                                      void *pData,
                                                      DWORD dwLen) {
  return ZCCryptDesDecryptOnce(algo, mode, pIV, pKey, pData, dwLen);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptOMACStart(PCHZCRYPT_CONTEXT_OMAC pOmac,
                                                  CHZCRYPT_ALGO algo, const BYTE* pKey) {
  int res, keylen;
  
  switch (algo) {
    case CHZCRYPT_ALGO_OMACAES128:
      keylen = 16;
      break;
    case CHZCRYPT_ALGO_OMACAES192:
      keylen = 24;
      break;
    case CHZCRYPT_ALGO_OMACAES256:
      keylen = 32;
      break;
    default:
      return ZCCRYPT_ERROR_PARM;
      break;
  }
  
  cry_int_context *context = new cry_int_context;
  
  register_cipher(&aes_desc);
  context->magic = CONTEXT_MAGIC;
  context->algo = (ZCCRYPT_ALGO)algo;
  context->mode = ZCCRYPT_MODE_DUMMY;
  
  res = omac_init(&context->data.omac, find_cipher("aes"), (unsigned char *)pKey, keylen);
  
  *pOmac = (CHZCRYPT_CONTEXT_OMAC)context;
  
  if (res!=CRYPT_OK) {
    delete context;
    return ZCCRYPT_ERROR_MISC;
  } else {
    return ZCCRYPT_NOERROR;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptOMACUpdate(CHZCRYPT_CONTEXT_OMAC Omac,
                                                   const void* pData, DWORD cbData) {
  cry_int_context* context = (cry_int_context *)Omac;
  int res;
  CHECK_CONTEXT(context);
  
  res = omac_process(&context->data.omac, (unsigned char *)pData, cbData);
  
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptOMACEnd(PCHZCRYPT_CONTEXT_OMAC pOmac, void* pTag) {
  cry_int_context* context = (cry_int_context *)(*pOmac);
  int res;
  unsigned long outlen = 16;
  
  CHECK_CONTEXT(context);
  
  res = omac_done(&context->data.omac, (unsigned char *)pTag, &outlen);
  context->magic = 0;
  
  delete context;
  
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptOMACOnce(CHZCRYPT_ALGO algo, const BYTE* pKey,
                                                 const void* pData, DWORD cbData,
                                                 void* pTag) {
  CHZCRYPT_CONTEXT_OMAC c;
  ZCCRYPTRET res;
  
  if ((res = CHZCryptOMACStart(&c, algo, pKey)) != ZCCRYPT_NOERROR) {
    return res;
  }
  
  res = CHZCryptOMACUpdate(c, pData, cbData);
  CHZCryptOMACEnd(&c, pTag);
  
  return res;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXStart(PCHZCRYPT_CONTEXT_EAX pEax,
                                                  CHZCRYPT_ALGO algo, const BYTE* pKey,
                                                  const void* pNonce, DWORD cbNonce,
                                                  const void* pHeader, DWORD cbHeader) {
  int res, keylen;
  
  switch (algo) {
    case CHZCRYPT_ALGO_EAXAES128:
      keylen = 16;
      break;
    case CHZCRYPT_ALGO_EAXAES192:
      keylen = 24;
      break;
    case CHZCRYPT_ALGO_EAXAES256:
      keylen = 32;
      break;
    default:
      return ZCCRYPT_ERROR_PARM;
      break;
  }
  
  cry_int_context *context = new cry_int_context;
  
  register_cipher(&aes_desc);
  context->magic = CONTEXT_MAGIC;
  context->algo = (ZCCRYPT_ALGO)algo;
  context->mode = ZCCRYPT_MODE_DUMMY;
  
  res = eax_init(&context->data.eax, find_cipher("aes"), (unsigned char *)pKey, keylen,
                 (unsigned char *)pNonce, cbNonce, (unsigned char *)pHeader, cbHeader);
  
  *pEax = (CHZCRYPT_CONTEXT_EAX)context;
  
  if (res!=CRYPT_OK) {
    delete context;
    return ZCCRYPT_ERROR_MISC;
  } else {
    return ZCCRYPT_NOERROR;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXAddHeader(CHZCRYPT_CONTEXT_EAX Eax,
                                                  const void* pHeader, DWORD cbHeader) {
  cry_int_context* context = (cry_int_context *)Eax;
  int res; 
  
  res = eax_addheader(&context->data.eax, (unsigned char *)pHeader, cbHeader);
  
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;  
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXEncrypt(CHZCRYPT_CONTEXT_EAX Eax,
                                                 void* pData, DWORD cbData) {
  cry_int_context* context = (cry_int_context *)Eax;
  int res; 
  
  res = eax_encrypt(&context->data.eax, (unsigned char *)pData, (unsigned char *)pData, cbData);
  
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;  
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXDecrypt(CHZCRYPT_CONTEXT_EAX Eax,
                                                 void* pData, DWORD cbData) {
  cry_int_context* context = (cry_int_context *)Eax;
  int res; 
  
  res = eax_decrypt(&context->data.eax, (unsigned char *)pData, (unsigned char *)pData, cbData);
  
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;  
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXEnd(PCHZCRYPT_CONTEXT_EAX pEax, void* pTag) {
  cry_int_context* context = (cry_int_context *)(*pEax);
  int res;
  unsigned long outlen = 16;
  
  CHECK_CONTEXT(context);
  
  res = eax_done(&context->data.eax, (unsigned char *)pTag, &outlen);
  context->magic = 0;
  
  delete context;
  
  return (res!=CRYPT_OK)?ZCCRYPT_ERROR_MISC:ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXEncryptOnce(CHZCRYPT_ALGO algo, const BYTE* pKey,
                                                 const void* pNonce, DWORD cbNonce,
                                                 const void* pHeader, DWORD cbHeader,
                                                 void* pData, DWORD cbData,
                                                 void* pTag) {
  CHZCRYPT_CONTEXT_EAX c;
  ZCCRYPTRET res;
  
  if ((res = CHZCryptEAXStart(&c, algo, pKey, pNonce, cbNonce, pHeader, cbHeader)) != ZCCRYPT_NOERROR) {
    return res;
  }
  
  res = CHZCryptEAXEncrypt(c, pData, cbData);
  CHZCryptEAXEnd(&c, pTag);
  
  return res;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEAXDecryptOnce(CHZCRYPT_ALGO algo, const BYTE* pKey,
                                                 const void* pNonce, DWORD cbNonce,
                                                 const void* pHeader, DWORD cbHeader,
                                                 void* pData, DWORD cbData,
                                                 void* pTag) {
  CHZCRYPT_CONTEXT_EAX c;
  ZCCRYPTRET res;
  
  if ((res = CHZCryptEAXStart(&c, algo, pKey, pNonce, cbNonce, pHeader, cbHeader)) != ZCCRYPT_NOERROR) {
    return res;
  }
  
  res = CHZCryptEAXDecrypt(c, pData, cbData);
  CHZCryptEAXEnd(&c, pTag);
  
  return res;
}

#ifdef ECC_BASICCARD

#define EC161 { F2M::setPentanomial(168,15,3,2,0); }
#define EC167 { F2M::setTrinomial(167, 6, 0); }
#define EC211 { F2M::setPentanomial(211,11,10,8,0); }

struct cry_int_ecc_context {
  DomainParameters dp;
  unsigned long magic;
};

Point cry_int_expand(cry_int_ecc_context *c, unsigned char *d) {
  F2M x;
  unsigned char gx[21];
  int gyc;

  memcpy(gx, d, sizeof(gx));
  gyc = gx[0] >> 7;
  gx[0] &= 0x7f;
  x.fromBinary(gx, 21);
  return c->dp.c.expandCoord(x, gyc);  
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167CreateContext(PZCCRYPT_CONTEXT_EC167 pEC167,
                                                          int iCurve) {
  EC167
  cry_int_ecc_context *c = new cry_int_ecc_context;
  
  c->magic = CONTEXT_MAGIC;
  
  switch (iCurve) {
    case 1:
      c->dp.c = Curve(F2M(1), F2M("780D32381A6FA719F2AA60D26653EAFFD6F72E2241"));
      c->dp.g = Point(F2M("4C88EC6F14E89715D2C4E12DD5961C233EFD43E4F"), F2M("7DBF0C8DB500312FDE0860A1F58CF0DD0E2D2FBEDB"));
      c->dp.r = "4000000000000000000004CA549CBF6D09FC17D3DF";
      c->dp.k = 2;
      break;
    case 2:
      c->dp.c = Curve(F2M(1), F2M("4D1C2B050BEE5BEE643B5C8082F7EFCCAF0945F92F"));
      c->dp.g = Point(F2M("5BE29DA612256087FF5B68D4C003868C159D77D97E"), F2M("236DF4A8A00457C3D101992DC2979C3A2C71DAE689"));
      c->dp.r = "40000000000000000000043B3C27AEB62DC581877B";
      c->dp.k = 2;
      break;
    case 3:
      c->dp.c = Curve(F2M(1), F2M("21C2968C28808743C304E518EAF36F31099B527C9A"));
      c->dp.g = Point(F2M("64ADC23A669986C19576ACD69DA15E3924887C1C7D"), F2M("422BDA4D593F684E6F11CEC12DAEE57595A932EB1F"));
      c->dp.r = "4000000000000000000001CBA0361F0382B6548471";
      c->dp.k = 2;
      break;
    case 4:
      c->dp.c = Curve(F2M(1), F2M("1A8B2B88D2DF30D337F058C6A865CE7D3807D97BEF"));
      c->dp.g = Point(F2M("4866C50702275C8758986B6BFA219D09EEBB5B2DBB"), F2M("7E3ABD517DEF373EADF676246027B3D66F351FE517"));
      c->dp.r = "40000000000000000000077F477C6B2E3093E5D5C9";
      c->dp.k = 2;
      break;
    case 5:
      c->dp.c = Curve(F2M(1), F2M("79964FD80B85AE569E4C044B048B00AF1FCD0BA382"));
      c->dp.g = Point(F2M("5A6612D9329241E0580AA36BFCA8C9E0409BC98A54"), F2M("3CDB4A7BC7E886C69534D4A57E9CEF3B376C3B460D"));
      c->dp.r = "4000000000000000000003B26EAC20B03B17FE0B51";
      c->dp.k = 2;
      break;
    default:
      delete c;
      return ZCCRYPT_ERROR_PARM;
      break;
  }

  if (!c->dp.valid()) {
    delete c;
    return ZCCRYPT_ERROR_EC167_BADPARAMS;
  } else {
    *pEC167 = (ZCCRYPT_CONTEXT_EC167)c;
    return ZCCRYPT_NOERROR;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167CreateContextFromParams
                                                (PZCCRYPT_CONTEXT_EC167 pEC167,
                                                 const ZCCRYPT_EC167DOMAINPARAMS *pParams) {
  EC167
  
  cry_int_ecc_context *c = new cry_int_ecc_context;
  
  c->magic = CONTEXT_MAGIC;
  c->dp.c.a = (int)pParams->a;
  c->dp.c.b.fromBinary((unsigned char*)pParams->b, 21);
  c->dp.r.fromBinary((unsigned char*)pParams->r, 21);
  c->dp.k = 2;
  
  c->dp.g = cry_int_expand(c, (unsigned char*)(pParams->G));
  
  if (!c->dp.valid()) {
    delete c;
    return ZCCRYPT_ERROR_EC167_BADPARAMS;
  } else {
    *pEC167 = (ZCCRYPT_CONTEXT_EC167)c;
    return ZCCRYPT_NOERROR;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167DestroyContext(PZCCRYPT_CONTEXT_EC167 pEC167) {
  cry_int_ecc_context* context = (cry_int_ecc_context *)(*pEC167);
  CHECK_CONTEXT(context);
  
  context->magic = 0;
  delete context;
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167GetPublicKey(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                PZCCRYPT_EC167PRIVATEKEY pPrivateKey,
                                                PZCCRYPT_EC167PUBLICKEY pPublicKey) {
  unsigned char pkout[21];
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec167);
  CHECK_CONTEXT(context);
  EC167
  
  BigNum s;
  
  s.fromBinary((unsigned char *)pPrivateKey, 21);
  ECPubKey pk = ECPubKey(context->dp, ECSecKey(context->dp, s));
  pk.q.x.toBinaryN(pkout, 21);
  pkout[0] |= context->dp.c.compressCoord(pk.q) << 7;
  memcpy(pPublicKey, pkout, 21);
  
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167SetupPrivateKey(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                   PZCCRYPT_EC167PRIVATEKEY pPrivateKey,
                                                   void *pKeyData,
                                                   DWORD cbKeyData) {
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec167);
  CHECK_CONTEXT(context);
  EC167

  BigNum s;
  s.fromBinary((unsigned char *)pKeyData, cbKeyData);
  s %= context->dp.r;
  s.toBinaryN((unsigned char *)pPrivateKey, 21);
  
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167GenerateKeyPair(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                            PZCCRYPT_EC167PRIVATEKEY pPrivateKey,
                                                            PZCCRYPT_EC167PUBLICKEY pPublicKey,
                                                            void *pSeed,
                                                            DWORD cbSeed) {
  ZCCRYPTRET res;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec167);
  CHECK_CONTEXT(context);
  EC167
  
  if (pSeed != NULL) {
    if ((res = ZCCryptRandomSeed(pSeed, cbSeed)) != ZCCRYPT_NOERROR) {
      return res;
    }
  }
  
  ECSecKey sk(context->dp);
  
  sk.s.toBinaryN((unsigned char *)pPrivateKey, 21);
  return ZCCryptEC167GetPublicKey(Ec167, pPrivateKey, pPublicKey);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167VerifyAlg(ZCCRYPT_CONTEXT_EC167 Ec167,
                                          const ZCCRYPT_EC167SIGNATURE *pSignature,
                                          const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                          const void *pMessage, CHZCRYPT_ALGO algo) {
  EC167
  
  ECPubKey pk;
  BigNum c, d, m;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec167);
  CHECK_CONTEXT(context);
  
  c.fromBinary(((unsigned char *)(pSignature))+00, 21);
  d.fromBinary(((unsigned char *)(pSignature))+21, 21);   
  m.fromBinary((unsigned char *)pMessage, 20);

  pk.dp = context->dp;
  pk.q = cry_int_expand(context, (unsigned char *)pPublicKey);
  
  return pk.verifySSA(c,d,m,algo==CHZCRYPT_ALGO_ECC_DSA)?ZCCRYPT_NOERROR:ZCCRYPT_ERROR_EC167_BADSIGNATURE;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167Verify(ZCCRYPT_CONTEXT_EC167 Ec167,
                                          const ZCCRYPT_EC167SIGNATURE *pSignature,
                                          const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                          const void *pMessage) {
  return CHZCryptEC167VerifyAlg(Ec167, pSignature, pPublicKey, pMessage, CHZCRYPT_ALGO_ECC_NR);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167HashAndVerifyAlg(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167SIGNATURE *pSignature,
                                                 const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage,  CHZCRYPT_ALGO algo) {
  EC167
  
  ECPubKey pk;  
  BigNum c, d;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec167);
  CHECK_CONTEXT(context);
  
  c.fromBinary(((unsigned char *)(pSignature))+00, 21);
  d.fromBinary(((unsigned char *)(pSignature))+21, 21);  

  pk.dp = context->dp;  
  pk.q = cry_int_expand(context, (unsigned char *)pPublicKey);
  
  return pk.verifySSA(c,d,(unsigned char *)pMessage,cbMessage, 0, algo==CHZCRYPT_ALGO_ECC_DSA)?ZCCRYPT_NOERROR:ZCCRYPT_ERROR_EC167_BADSIGNATURE;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167HashAndVerify(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167SIGNATURE *pSignature,
                                                 const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage) {
  return CHZCryptEC167HashAndVerifyAlg(Ec167, pSignature, pPublicKey, pMessage, cbMessage, CHZCRYPT_ALGO_ECC_NR);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167SignAlg(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 ZCCRYPT_EC167SIGNATURE *pSignature, CHZCRYPT_ALGO algo) {
  EC167
    
  ECSecKey sk;
  BigNum c, d, m;
  int res;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec167);
  CHECK_CONTEXT(context);  

  m.fromBinary((unsigned char *)pMessage, 20);  
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 21);
  
  res = sk.signSSA(m, c, d, algo==CHZCRYPT_ALGO_ECC_DSA);
  c.toBinaryN(((unsigned char *)(pSignature))+00, 21);
  d.toBinaryN(((unsigned char *)(pSignature))+21, 21);   
  
  return res?ZCCRYPT_NOERROR:ZCCRYPT_ERROR_MISC;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167Sign(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 ZCCRYPT_EC167SIGNATURE *pSignature) {
  return CHZCryptEC167SignAlg(Ec167, pPrivateKey, pMessage, pSignature, CHZCRYPT_ALGO_ECC_NR);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167HashAndSignAlg(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage,
                                                 ZCCRYPT_EC167SIGNATURE *pSignature, CHZCRYPT_ALGO algo) {
  EC167
    
  ECSecKey sk;
  BigNum c, d;
  int res;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec167);
  CHECK_CONTEXT(context);  
  
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 21);
  
  res = sk.signSSA((unsigned char *)pMessage, cbMessage, c, d, 0, algo==CHZCRYPT_ALGO_ECC_DSA);
  c.toBinaryN(((unsigned char *)(pSignature))+00, 21);
  d.toBinaryN(((unsigned char *)(pSignature))+21, 21);   
  
  return res?ZCCRYPT_NOERROR:ZCCRYPT_ERROR_MISC;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC167HashAndSign(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                 const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage,
                                                 ZCCRYPT_EC167SIGNATURE *pSignature) {
  return CHZCryptEC167HashAndSignAlg(Ec167, pPrivateKey, pMessage, cbMessage, pSignature, CHZCRYPT_ALGO_ECC_NR);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167SharedSecret(ZCCRYPT_CONTEXT_EC167 Ec167,
                                                const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                                const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                                PZCCRYPT_EC167SHAREDSECRET pSecret) {
  EC167
  
  ECPubKey pk;
  ECSecKey sk;
  F2M sec;
  
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec167);
  CHECK_CONTEXT(context);

  pk.dp = context->dp;
  pk.q = cry_int_expand(context, (unsigned char *)pPublicKey);
  
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 21);
  
  sk.sharedSecret(pk, sec);
  sec.toBinaryN(pSecret, 21);
  
  return ZCCRYPT_NOERROR;  
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC167SessionKey(ZCCRYPT_CONTEXT_EC167 Ec167,
                                              const ZCCRYPT_EC167PRIVATEKEY *pPrivateKey,
                                              const ZCCRYPT_EC167PUBLICKEY *pPublicKey,
                                              const void *pKdp,
                                              DWORD cbKdp,
                                              PZCCRYPT_EC167SESSIONKEY pKey) {
  EC167
  
  ECPubKey pk;
  ECSecKey sk;
  F2M sec;
  
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec167);
  CHECK_CONTEXT(context);

  pk.dp = context->dp;
  pk.q = cry_int_expand(context, (unsigned char *)pPublicKey);
  
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 21);
  
  sk.sharedSecret(pk, sec);
  sk.sessionKey(sec, 21, (unsigned char *)pKdp, cbKdp, (unsigned char *)pKey, 0);
  
  return ZCCRYPT_NOERROR;  
}

// -- EC161

Point cry_int_expand161(cry_int_ecc_context *c, unsigned char *dx, int y) {
  F2M x;

  x.convertEC161 = 1;
  x.fromBinary(dx, 21);
  return c->dp.c.expandCoord161(x, y);  
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161CreateContext(PZCCRYPT_CONTEXT_EC161 pEC161,
                                                          int iCurve) {
  EC161
  cry_int_ecc_context *c = new cry_int_ecc_context;
  
  c->magic = CONTEXT_MAGIC;
  
  switch (iCurve) {
    case 1:
      c->dp.c = Curve(F2M("831A2768FCDCC33E71210267B3003C719F8E22FC00"), F2M("10D365E4A7AF29A0E4601422F7BCA30F6E8E23B639"), 1);
      c->dp.g = Point(F2M("8F9632872FF9B0F234484BAD41BA8C3DF430642C5F"), F2M("5833F52929823461F65F3974186786BF8D36B39E2E"));
      c->dp.r = "1E9131ABF0B7672A07A44BE6AA1F7BD468B427B11";
      c->dp.k = 0x86;
      break;
    case 2:
      c->dp.c = Curve(F2M("3478516EFD9509528994AE0B59EC9FA661FFDF6122"), F2M("6259E4E4FD28AD5367D11956654FCD2FA0007D5D3F"), 1);
      c->dp.g = Point(F2M("DA8FBBF2CD6638DC801BE5B8BEBB5A1187075CD344"), F2M("909AF84D0A512B39AE50080B511933CB16279D0C14"));
      c->dp.r = "1702E05C0B81702E05C0BA2E5C7ECC5D1F3726983";
      c->dp.k = 0xb2;
      break;
    case 3:
      c->dp.c = Curve(F2M("6864FD8CC5D1022031E30AC8C7E121176B442F0103"), F2M("878563A904C7112689220CA05D6E670D3310D7213D"), 1);
      c->dp.g = Point(F2M("E23450934DBF89BCE79E8197F0C1019E28375C4612"), F2M("472A8E7AC9239638CFFA672DA053200BD04EEFFE19"));
      c->dp.r = "138138138138138138138151E9FA445E3B8CD5021";
      c->dp.k = 0xd2;
      break;
    default:
      delete c;
      return ZCCRYPT_ERROR_PARM;
      break;
  }

  if (!c->dp.valid()) {
    delete c;
    return ZCCRYPT_ERROR_EC161_BADPARAMS;
  } else {
    *pEC161 = (ZCCRYPT_CONTEXT_EC161)c;
    return ZCCRYPT_NOERROR;
  }
}  

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161CreateContextFromParams
                                                (PZCCRYPT_CONTEXT_EC161 pEC161,
                                                 const ZCCRYPT_EC161DOMAINPARAMS *pParams) {
  EC161
  unsigned char buffer[21];
  int i;
  
  cry_int_ecc_context *c = new cry_int_ecc_context;
  
  c->magic = CONTEXT_MAGIC;
  
  for (i=0; i<21; i++) {
    buffer[i] = 0;
  }
  buffer[11] = pParams->a;
  c->dp.c.a.convertEC161 = 1;
  c->dp.c.a.fromBinary(buffer, 21);
  
  c->dp.c.b.convertEC161 = 1;
  c->dp.c.b.fromBinary((unsigned char*)pParams->b, 21);
  c->dp.c.isEC161 = 1;
  
  c->dp.r.fromBinary((unsigned char*)pParams->r, 21);
  c->dp.k = pParams->k;
  
  c->dp.g = cry_int_expand161(c, (unsigned char*)(pParams->Gx), pParams->Gy);
  
  if (!c->dp.valid()) {
    delete c;
    return ZCCRYPT_ERROR_EC161_BADPARAMS;
  } else {
    *pEC161 = (ZCCRYPT_CONTEXT_EC161)c;
    return ZCCRYPT_NOERROR;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161DestroyContext(PZCCRYPT_CONTEXT_EC161 pEC161) {
  cry_int_ecc_context* context = (cry_int_ecc_context *)(*pEC161);
  CHECK_CONTEXT(context);
  
  context->magic = 0;
  delete context;
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161GetPublicKey(ZCCRYPT_CONTEXT_EC161 Ec161,
												                                 PZCCRYPT_EC161PRIVATEKEY pPrivateKey,
												                                 PZCCRYPT_EC161PUBLICKEY pPublicKey) {
  unsigned char pkout[22];
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec161);
  CHECK_CONTEXT(context);
  EC161
  
  BigNum s;
  
  s.fromBinary((unsigned char *)pPrivateKey, 21);
  ECPubKey pk = ECPubKey(context->dp, ECSecKey(context->dp, s));
  pk.q.x.convertEC161 = 1;
  pk.q.x.toBinaryN(pkout, 21);
  pkout[21] = context->dp.c.compressCoord161(pk.q);
  memcpy(pPublicKey, pkout, 22);
  
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161SetupPrivateKey(ZCCRYPT_CONTEXT_EC161 Ec161,
											    	                                PZCCRYPT_EC161PRIVATEKEY pPrivateKey,
												                                    void *pKeyData,
												                                    DWORD cbKeyData) {
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec161);
  CHECK_CONTEXT(context);
  EC161

  BigNum s;
  s.fromBinary((unsigned char *)pKeyData, cbKeyData);
  s %= context->dp.r;
  s.toBinaryN((unsigned char *)pPrivateKey, 21);
  
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161GenerateKeyPair(ZCCRYPT_CONTEXT_EC161 Ec161,
												                                    PZCCRYPT_EC161PRIVATEKEY pPrivateKey,
												                                    PZCCRYPT_EC161PUBLICKEY pPublicKey,
												                                    void *pSeed,
												                                    DWORD cbSeed) {
  ZCCRYPTRET res;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec161);
  CHECK_CONTEXT(context);
  EC161
  
  if (pSeed != NULL) {
    if ((res = ZCCryptRandomSeed(pSeed, cbSeed)) != ZCCRYPT_NOERROR) {
      return res;
    }
  }
  
  ECSecKey sk(context->dp);
  
  sk.s.toBinaryN((unsigned char *)pPrivateKey, 21);
  return ZCCryptEC161GetPublicKey(Ec161, pPrivateKey, pPublicKey);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161Verify(ZCCRYPT_CONTEXT_EC161 Ec161,
										                               const ZCCRYPT_EC161SIGNATURE *pSignature,
										                               const ZCCRYPT_EC161PUBLICKEY *pPublicKey,
										                               const void *pMessage) {
  EC161
  
  ECPubKey pk;
  BigNum c, d, m;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec161);
  CHECK_CONTEXT(context);
  
  c.fromBinary(((unsigned char *)(pSignature))+00, 21);
  d.fromBinary(((unsigned char *)(pSignature))+21, 21);   
  m.fromBinary((unsigned char *)pMessage, 20);

  pk.dp = context->dp;
  pk.q = cry_int_expand161(context, (unsigned char *)pPublicKey, *(((unsigned char*)pPublicKey)+21));
  
  return pk.verifySSA(c,d,m,0)?ZCCRYPT_NOERROR:ZCCRYPT_ERROR_EC161_BADSIGNATURE;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161HashAndVerify(ZCCRYPT_CONTEXT_EC161 Ec161,
										                                      const ZCCRYPT_EC161SIGNATURE *pSignature,
										                                      const ZCCRYPT_EC161PUBLICKEY *pPublicKey,
										                                      const void *pMessage,
										                                      DWORD cbMessage) {
  EC161
  
  ECPubKey pk;  
  BigNum c, d;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec161);
  CHECK_CONTEXT(context);
  
  c.fromBinary(((unsigned char *)(pSignature))+00, 21);
  d.fromBinary(((unsigned char *)(pSignature))+21, 21);  

  pk.dp = context->dp;  
  pk.q = cry_int_expand161(context, (unsigned char *)pPublicKey, *(((unsigned char*)pPublicKey)+21));
  
  return pk.verifySSA(c,d,(unsigned char *)pMessage,cbMessage, 0, 0)?ZCCRYPT_NOERROR:ZCCRYPT_ERROR_EC161_BADSIGNATURE;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC161Sign(ZCCRYPT_CONTEXT_EC161 Ec161,
                                                 const ZCCRYPT_EC161PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 ZCCRYPT_EC161SIGNATURE *pSignature) {
  EC161
    
  ECSecKey sk;
  BigNum c, d, m;
  int res;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec161);
  CHECK_CONTEXT(context);  

  m.fromBinary((unsigned char *)pMessage, 20);  
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 21);
  
  res = sk.signSSA(m, c, d, 0);
  c.toBinaryN(((unsigned char *)(pSignature))+00, 21);
  d.toBinaryN(((unsigned char *)(pSignature))+21, 21);   
  
  return res?ZCCRYPT_NOERROR:ZCCRYPT_ERROR_MISC;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC161HashAndSign(ZCCRYPT_CONTEXT_EC161 Ec161,
                                                 const ZCCRYPT_EC161PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage,
                                                 ZCCRYPT_EC161SIGNATURE *pSignature) {
  EC161
    
  ECSecKey sk;
  BigNum c, d;
  int res;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec161);
  CHECK_CONTEXT(context);  
  
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 21);
  
  res = sk.signSSA((unsigned char *)pMessage, cbMessage, c, d, 0, 0);
  c.toBinaryN(((unsigned char *)(pSignature))+00, 21);
  d.toBinaryN(((unsigned char *)(pSignature))+21, 21);   
  
  return res?ZCCRYPT_NOERROR:ZCCRYPT_ERROR_MISC;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161SharedSecret(ZCCRYPT_CONTEXT_EC161 Ec161,
												                                 const ZCCRYPT_EC161PRIVATEKEY *pPrivateKey,
												                                 const ZCCRYPT_EC161PUBLICKEY *pPublicKey,
												                                 PZCCRYPT_EC161SHAREDSECRET pSecret) {
  EC161
  
  ECPubKey pk;
  ECSecKey sk;
  F2M sec;
  
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec161);
  CHECK_CONTEXT(context);

  pk.dp = context->dp;
  pk.q = cry_int_expand161(context, (unsigned char *)pPublicKey, *(((unsigned char*)pPublicKey)+21));
  
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 21);
  
  sk.sharedSecret(pk, sec);
  sec.toBinaryN(pSecret, 21);
  
  return ZCCRYPT_NOERROR;  
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK ZCCryptEC161SessionKey(ZCCRYPT_CONTEXT_EC161 Ec161,
											                                 const ZCCRYPT_EC161PRIVATEKEY *pPrivateKey,
											                                 const ZCCRYPT_EC161PUBLICKEY *pPublicKey,
										                                   const void *pKdp,
											                                 DWORD cbKdp,
											                                 PZCCRYPT_EC161SESSIONKEY pKey) {
  EC161
  
  ECPubKey pk;
  ECSecKey sk;
  F2M sec;
  
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec161);
  CHECK_CONTEXT(context);

  pk.dp = context->dp;
  pk.q = cry_int_expand161(context, (unsigned char *)pPublicKey, *(((unsigned char*)pPublicKey)+21));
  
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 21);
  
  sk.sharedSecret(pk, sec);
  sk.sessionKey(sec, 21, (unsigned char *)pKdp, cbKdp, (unsigned char *)pKey, 0);
  
  return ZCCRYPT_NOERROR;  
}

// EC211

Point cry_int_expand211(cry_int_ecc_context *c, unsigned char *d) {
  F2M x;
  unsigned char gx[27];
  int gyc;

  memcpy(gx, d, sizeof(gx));
  gyc = gx[0] >> 7;
  gx[0] &= 0x7f;
  x.fromBinary(gx, 27);
  return c->dp.c.expandCoord(x, gyc);  
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211CreateContext(PCHZCRYPT_CONTEXT_EC211 pEC211,
                                                           int iCurve) {
  EC211
  cry_int_ecc_context *c = new cry_int_ecc_context;
  
  c->magic = CONTEXT_MAGIC;
  
  switch (iCurve) {
    case 1:
      c->dp.c = Curve(F2M(1), F2M("07FB2374C3862A05CF7DF33F74BF15781A7607FEDF0DAB31AB8F69"));
      c->dp.g = Point(F2M("406FF157FE7240D09C4473D33BD2551177CC779FFD7F74790A93E"), F2M("2A3251C2C43BF9DF37197BAF3E61450689412A1441FE1FC6FC141"));
      c->dp.r = "04000000000000000000000000016BA81D3542A634C1F2684188CB";
      c->dp.k = 2;
      break;
    case 2:
      c->dp.c = Curve(F2M(1), F2M("03E39CBE7EC2419D8310F7FD458F53B01CC55047716367100126E0"));
      c->dp.g = Point(F2M("1848BE4CAF7797DD0DA2BF2B01DCF317EBF4EF726F92DF6480615"), F2M("659FE8CB35ECA25971C1C6503B3A1CFC64F45544FA9FF081ECB69"));
      c->dp.r = "04000000000000000000000000006A28E6A0802A11AD34557A3A0B";
      c->dp.k = 2;
      break;
    case 3:
      c->dp.c = Curve(F2M(1), F2M("048ECBAE4EC7EA5CC609A39253FA1A020E68A23CCDA2785942E651"));
      c->dp.g = Point(F2M("369FB8C4097A6E6BB64A6599A3B841440757C7606E95D14C7EFCC"), F2M("1B0F5FE5EBE848B181712FE1ABC105AC96CA488316F9627CAC954"));
      c->dp.r = "04000000000000000000000000019D1EF0E57804803C8AA51BBE33";
      c->dp.k = 2;
      break;
    case 4:
      c->dp.c = Curve(F2M(1), F2M("07F6C3205D0DBA927678047884506DBCEB0B91913F1C4198BB157D"));
      c->dp.g = Point(F2M("1500214DBC99D141C3083EE24C0549A9FC3CB99C02577A4A68F5F"), F2M("6EED22AD4E558937EFB65155D7E9ED4D5D4FADBEC3B0E7BB40A48"));
      c->dp.r = "0400000000000000000000000000E76082C60A24F134FA76565B09";
      c->dp.k = 2;
      break;
    case 5:
      c->dp.c = Curve(F2M(1), F2M("0618C91C8573E926FDE10F8F0375E14D17F3A3A1B50C4C39D51F17"));
      c->dp.g = Point(F2M("075CDABA9F4DDEDE0937780EEB7FC5DC0CEA4C76B0D24733F86AF"), F2M("2EC0547F3C7D67D60093EF398571B4C59A2D15095DF0E6F878BB7"));  
      c->dp.r = "0400000000000000000000000002BAA1BD9EA46810A9028DA1BFC1";
      c->dp.k = 2;
      break;
    default:
      delete c;
      return ZCCRYPT_ERROR_PARM;
      break;
  }

  if (!c->dp.valid()) {
    delete c;
    return CHZCRYPT_ERROR_EC211_BADPARAMS;
  } else {
    *pEC211 = (CHZCRYPT_CONTEXT_EC211)c;
    return ZCCRYPT_NOERROR;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211CreateContextFromParams
                                                (PCHZCRYPT_CONTEXT_EC211 pEC211,
                                                 const CHZCRYPT_EC211DOMAINPARAMS *pParams) {
  EC211
  
  cry_int_ecc_context *c = new cry_int_ecc_context;
  
  c->magic = CONTEXT_MAGIC;
  c->dp.c.a = (int)pParams->a;
  c->dp.c.b.fromBinary((unsigned char*)pParams->b, 27);
  c->dp.r.fromBinary((unsigned char*)pParams->r, 27);
  c->dp.k = 2;
  
  c->dp.g = cry_int_expand211(c, (unsigned char*)(pParams->G));
  
  if (!c->dp.valid()) {
    delete c;
    return CHZCRYPT_ERROR_EC211_BADPARAMS;
  } else {
    *pEC211 = (CHZCRYPT_CONTEXT_EC211)c;
    return ZCCRYPT_NOERROR;
  }
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211DestroyContext(PCHZCRYPT_CONTEXT_EC211 pEC211) {
  cry_int_ecc_context* context = (cry_int_ecc_context *)(*pEC211);
  CHECK_CONTEXT(context);
  
  context->magic = 0;
  delete context;
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211GetPublicKey(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                PCHZCRYPT_EC211PRIVATEKEY pPrivateKey,
                                                PCHZCRYPT_EC211PUBLICKEY pPublicKey) {
  unsigned char pkout[27];
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec211);
  CHECK_CONTEXT(context);
  EC211
  
  BigNum s;
  
  s.fromBinary((unsigned char *)pPrivateKey, 27);
  ECPubKey pk = ECPubKey(context->dp, ECSecKey(context->dp, s));
  pk.q.x.toBinaryN(pkout, 27);
  pkout[0] |= context->dp.c.compressCoord(pk.q) << 7;
  memcpy(pPublicKey, pkout, 27);
  
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211SetupPrivateKey(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                   PCHZCRYPT_EC211PRIVATEKEY pPrivateKey,
                                                   void *pKeyData,
                                                   DWORD cbKeyData) {
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec211);
  CHECK_CONTEXT(context);
  EC211

  BigNum s;
  s.fromBinary((unsigned char *)pKeyData, cbKeyData);
  s %= context->dp.r;
  s.toBinaryN((unsigned char *)pPrivateKey, 27);
  
  return ZCCRYPT_NOERROR;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211GenerateKeyPair(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                            PCHZCRYPT_EC211PRIVATEKEY pPrivateKey,
                                                            PCHZCRYPT_EC211PUBLICKEY pPublicKey,
                                                            void *pSeed,
                                                            DWORD cbSeed) {
  ZCCRYPTRET res;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec211);
  CHECK_CONTEXT(context);
  EC211
  
  if (pSeed != NULL) {
    if ((res = ZCCryptRandomSeed(pSeed, cbSeed)) != ZCCRYPT_NOERROR) {
      return res;
    }
  }
  
  ECSecKey sk(context->dp);
  
  sk.s.toBinaryN((unsigned char *)pPrivateKey, 27);
  return CHZCryptEC211GetPublicKey(Ec211, pPrivateKey, pPublicKey);
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211VerifyAlg(CHZCRYPT_CONTEXT_EC211 Ec211,
                                          const CHZCRYPT_EC211SIGNATURE *pSignature,
                                          const CHZCRYPT_EC211PUBLICKEY *pPublicKey,
                                          const void *pMessage, CHZCRYPT_ALGO algo) {
  EC211
  
  ECPubKey pk;
  BigNum c, d, m;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec211);
  CHECK_CONTEXT(context);
  
  c.fromBinary(((unsigned char *)(pSignature))+00, 27);
  d.fromBinary(((unsigned char *)(pSignature))+27, 27);   
  m.fromBinary((unsigned char *)pMessage, 27); 
  m = m.rsh(6); // ensure m is exactly 210 bits long

  pk.dp = context->dp;
  pk.q = cry_int_expand211(context, (unsigned char *)pPublicKey);
  
  return pk.verifySSA(c,d,m,algo==CHZCRYPT_ALGO_ECC_DSA)?ZCCRYPT_NOERROR:CHZCRYPT_ERROR_EC211_BADSIGNATURE;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211HashAndVerifyAlg(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                 const CHZCRYPT_EC211SIGNATURE *pSignature,
                                                 const CHZCRYPT_EC211PUBLICKEY *pPublicKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage, CHZCRYPT_ALGO algo) {
  EC211
  
  ECPubKey pk;  
  BigNum c, d;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec211);
  CHECK_CONTEXT(context);
  
  c.fromBinary(((unsigned char *)(pSignature))+00, 27);
  d.fromBinary(((unsigned char *)(pSignature))+27, 27);  

  pk.dp = context->dp;  
  pk.q = cry_int_expand211(context, (unsigned char *)pPublicKey);

  return pk.verifySSA(c,d,(unsigned char *)pMessage,cbMessage, 1, algo==CHZCRYPT_ALGO_ECC_DSA)?ZCCRYPT_NOERROR:CHZCRYPT_ERROR_EC211_BADSIGNATURE;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211SignAlg(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                 const CHZCRYPT_EC211PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 CHZCRYPT_EC211SIGNATURE *pSignature, CHZCRYPT_ALGO algo) {
  EC211
    
  ECSecKey sk;
  BigNum c, d, m;
  int res;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec211);
  CHECK_CONTEXT(context);  

  m.fromBinary((unsigned char *)pMessage, 27);  
  m = m.rsh(6); // ensure m is exactly 210 bits long
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 27);
  
  res = sk.signSSA(m, c, d, algo==CHZCRYPT_ALGO_ECC_DSA);
  c.toBinaryN(((unsigned char *)(pSignature))+00, 27);
  d.toBinaryN(((unsigned char *)(pSignature))+27, 27);   
  
  return res?ZCCRYPT_NOERROR:ZCCRYPT_ERROR_MISC;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211HashAndSignAlg(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                 const CHZCRYPT_EC211PRIVATEKEY *pPrivateKey,
                                                 const void *pMessage,
                                                 DWORD cbMessage,
                                                 CHZCRYPT_EC211SIGNATURE *pSignature, CHZCRYPT_ALGO algo) {
  EC211
    
  ECSecKey sk;
  BigNum c, d;
  int res;
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec211);
  CHECK_CONTEXT(context);  
  
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 27);

  res = sk.signSSA((unsigned char *)pMessage, cbMessage, c, d, 1, algo==CHZCRYPT_ALGO_ECC_DSA);
  c.toBinaryN(((unsigned char *)(pSignature))+00, 27);
  d.toBinaryN(((unsigned char *)(pSignature))+27, 27);   
  
  return res?ZCCRYPT_NOERROR:ZCCRYPT_ERROR_MISC;
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211SharedSecret(CHZCRYPT_CONTEXT_EC211 Ec211,
                                                const CHZCRYPT_EC211PRIVATEKEY *pPrivateKey,
                                                const CHZCRYPT_EC211PUBLICKEY *pPublicKey,
                                                PCHZCRYPT_EC211SHAREDSECRET pSecret) {
  EC211
  
  ECPubKey pk;
  ECSecKey sk;
  F2M sec;
  
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec211);
  CHECK_CONTEXT(context);

  pk.dp = context->dp;
  pk.q = cry_int_expand211(context, (unsigned char *)pPublicKey);
  
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 27);
  
  sk.sharedSecret(pk, sec);
  sec.toBinaryN(pSecret, 27);
  
  return ZCCRYPT_NOERROR;  
}

ZCDLLEXP ZCCRYPTRET ZCCRYPTLINK CHZCryptEC211SessionKey(CHZCRYPT_CONTEXT_EC211 Ec211,
                                              const CHZCRYPT_EC211PRIVATEKEY *pPrivateKey,
                                              const CHZCRYPT_EC211PUBLICKEY *pPublicKey,
                                              const void *pKdp,
                                              DWORD cbKdp,
                                              PCHZCRYPT_EC211SESSIONKEY pKey) {
  EC211
  
  ECPubKey pk;
  ECSecKey sk;
  F2M sec;
  
  cry_int_ecc_context* context = (cry_int_ecc_context *)(Ec211);
  CHECK_CONTEXT(context);

  pk.dp = context->dp;
  pk.q = cry_int_expand211(context, (unsigned char *)pPublicKey);
  
  sk.dp = context->dp;
  sk.s.fromBinary((unsigned char *)pPrivateKey, 27);
  
  sk.sharedSecret(pk, sec);
  sk.sessionKey(sec, 27, (unsigned char *)pKdp, cbKdp, (unsigned char *)pKey, 1);
  
  return ZCCRYPT_NOERROR;  
}
#endif
