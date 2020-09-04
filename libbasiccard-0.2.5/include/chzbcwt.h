/* Definition of some common Windows data types */

#ifndef __WINTYP_H__
#define __WINTYP_H__

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef _WIN32
#include <windows.h>
#else

typedef unsigned char BYTE;
typedef unsigned short WORD;

typedef long LONG;
typedef unsigned long DWORD;
typedef unsigned long ULONG;
typedef unsigned long *PULONG;

typedef short BOOL;

#endif

#ifdef __cplusplus
}
#endif

#endif

