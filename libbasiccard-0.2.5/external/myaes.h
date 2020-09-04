#ifndef __AES_H__
#define __AES_H__

#ifdef __cplusplus
extern "C" {
#endif

int mekaes(unsigned char *message, int msglen, unsigned char key[32], int keylen, unsigned char init[16]);
int mdkaes(unsigned char *message, int msglen, unsigned char key[32], int keylen, unsigned char init[16]);

#ifdef __cplusplus
}
#endif
#endif

