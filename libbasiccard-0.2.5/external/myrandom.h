// random provider for libbasiccard

#ifndef __MYRANDOM_H__
#define __MYRANDOM_H__

#ifdef __cplusplus
extern "C" {
#endif

void seedmyrandom(long seed);
long myrandom(void);

#ifdef __cplusplus
}
#endif

#endif

