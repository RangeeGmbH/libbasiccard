/* (C) 2004 Christian Zietz */
#include <stdlib.h>
#include <string.h>

#include "myaes.h"
#include <tomcrypt.h>


int mekaes(unsigned char *message, int msglen, unsigned char key[32], int keylen, unsigned char init[16]) {
  unsigned char buf[16];
  int m, n, i, j;
  char *c, *p;
  symmetric_key k;

  if (msglen<16)
    return 0;

  // preparation
  m = 16-(msglen%16);
  if (m==16) {
    n = msglen/16;
    m = 0;
  } else {
   n = (msglen/16)+1;
  }
  c = (char *)malloc((n+1)*16);
  p = (char *)malloc(n*16);
  memset(p, 0, n*16);
  memcpy(p, message, msglen);
  memcpy(c, init, 16);

  register_cipher(&aes_desc);
  aes_setup(key, keylen, 0, &k);
  
  // encrypt
  for (i=0; i<n; i++) {
    for (j=0; j<16; j++) {
      buf[j] = *(c+(i*16)+j) ^ *(p+(i*16)+j);
    }
    aes_ecb_encrypt(buf, buf, &k);
    memcpy(c+16+(i*16), buf, 16);
  }

  // "steal" m bytes
  memcpy(message, c+16, ((n-1)*16)-m);
  memcpy(message+((n-1)*16)-m, c+(n*16), 16);

  // update init
  memcpy(init, c+(n*16), 16);

  free(c);
  free(p);
  return 1;
}

int mdkaes(unsigned char *message, int msglen, unsigned char key[32], int keylen, unsigned char init[16]) {
  unsigned char buf[16];
  int m, n, i, j;
  char *c, *p;
  symmetric_key k;

  if (msglen<16)
    return 0;

  m = 16-(msglen%16);
  if (m==16) {
    n = msglen/16;
    m = 0;
  } else {
   n = (msglen/16)+1;
  }
  c = (char *)malloc((n+1)*16);
  p = (char *)malloc(n*16);
  memset(p, 0, n*16);
  memcpy(c, init, 16);
  memcpy(c+16, message, ((n-1)*16)-m);
  memcpy(c+(n*16), message+((n-1)*16)-m, 16); 

  register_cipher(&aes_desc);
  aes_setup(key, keylen, 0, &k);

  // restore pen-ultimate block
  memcpy(buf, c+(n*16), 16);
  
  aes_ecb_decrypt(buf, buf, &k);
  
  for (i=0; i<m; i++)
    *(c+(n*16)-m+i) = buf[(16-m)+i];

  // decrypt
  
  for (i=0; i<n; i++) {
    memcpy(buf, c+16+(i*16), 16);
    aes_ecb_decrypt(buf, buf, &k);
    for (j=0; j<16; j++) {
      buf[j] ^= *(c+(i*16)+j);
    }

    memcpy(p+(i*16), buf, 16);
  }

  memcpy(message, p, msglen);

  // update init
  memcpy(init, c+(n*16), 16);

  free(c);
  free(p);
  return 1;
}
