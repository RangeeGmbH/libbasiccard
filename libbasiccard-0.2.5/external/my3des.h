#ifndef __3DES_H__
#define __3DES_H__

#ifdef __cplusplus
extern "C" {
#endif

int mek3des(unsigned char *message, int msglen, unsigned char key[16], unsigned char init[8]);
int mdk3des(unsigned char *message, int msglen, unsigned char key[16], unsigned char init[8]);

#ifdef __cplusplus
}
#endif
#endif

