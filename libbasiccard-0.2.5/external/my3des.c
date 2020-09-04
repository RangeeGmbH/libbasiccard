/* (C) 2004 Christian Zietz */
#include <stdlib.h>
#include <string.h>

#include "my3des.h"
#include <tomcrypt.h>


int mek3des(unsigned char *message, int msglen, unsigned char key[16], unsigned char init[8]) {
  unsigned char buf[8];
  unsigned char newkey[24];
  int m, n, i, j;
  char *c, *p;
  symmetric_key k;

  if (msglen<8)
    return 0;

  // preparation
  m = 8-(msglen%8);
  if (m==8) {
    n = msglen/8;
    m = 0;
  } else {
   n = (msglen/8)+1;
  }
  c = (char *)malloc((n+1)*8);
  p = (char *)malloc(n*8);
  memset(p, 0, n*8);
  memcpy(p, message, msglen);
  memcpy(c, init, 8);

  memcpy(newkey, key, 16);
  memcpy(newkey+16, key, 8);

  register_cipher(&des3_desc);
  des3_setup(newkey, 24, 0, &k);
  
  // encrypt
  for (i=0; i<n; i++) {
    for (j=0; j<8; j++) {
      buf[j] = *(c+(i*8)+j) ^ *(p+(i*8)+j);
    }
    des3_ecb_encrypt(buf, buf, &k);
    memcpy(c+8+(i*8), buf, 8);
  }

  // "steal" m bytes
  memcpy(message, c+8, ((n-1)*8)-m);
  memcpy(message+((n-1)*8)-m, c+(n*8), 8);

  // update init
  memcpy(init, c+(n*8), 8);

  free(c);
  free(p);
  return 1;
}

int mdk3des(unsigned char *message, int msglen, unsigned char key[16], unsigned char init[8]) {
  unsigned char buf[8];
  unsigned char newkey[24];
  int m, n, i, j;
  char *c, *p;
  symmetric_key k;

  if (msglen<8)
    return 0;

  m = 8-(msglen%8);
  if (m==8) {
    n = msglen/8;
    m = 0;
  } else {
   n = (msglen/8)+1;
  }
  c = (char *)malloc((n+1)*8);
  p = (char *)malloc(n*8);
  memset(p, 0, n*8);
  memcpy(c, init, 8);
  memcpy(c+8, message, ((n-1)*8)-m);
  memcpy(c+(n*8), message+((n-1)*8)-m, 8); 

  memcpy(newkey, key, 16);
  memcpy(newkey+16, key, 8);

  register_cipher(&des3_desc);
  des3_setup(newkey, 24, 0, &k);

  // restore pen-ultimate block
  memcpy(buf, c+(n*8), 8);
  
  des3_ecb_decrypt(buf, buf, &k);
  
  for (i=0; i<m; i++)
    *(c+(n*8)-m+i) = buf[(8-m)+i];

  // decrypt
  
  for (i=0; i<n; i++) {
    memcpy(buf, c+8+(i*8), 8);
    des3_ecb_decrypt(buf, buf, &k);
    for (j=0; j<8; j++) {
      buf[j] ^= *(c+(i*8)+j);
    }

    memcpy(p+(i*8), buf, 8);
  }

  memcpy(message, p, msglen);

  // update init
  memcpy(init, c+(n*8), 8);

  free(c);
  free(p);
  return 1;
}
