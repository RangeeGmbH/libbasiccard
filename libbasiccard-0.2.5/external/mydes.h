#ifndef __DES_H__
#define __DES_H__

#ifdef __cplusplus 
extern "C" {
#endif

int mekdes(unsigned char *message, int msglen, unsigned char key[8], unsigned char init[8]);
int mdkdes(unsigned char *message, int msglen, unsigned char key[8], unsigned char init[8]);

#ifdef __cplusplus 
}
#endif

#endif

