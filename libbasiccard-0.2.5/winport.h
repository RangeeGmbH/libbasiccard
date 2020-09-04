#ifndef WINPORT_H
#define WINPORT_H

#ifdef _WIN32

#include <windows.h>

#define setenv(x,y,z) SetEnvironmentVariable(x,y)

#define SCARD_SCOPE_GLOBAL SCARD_SCOPE_SYSTEM
#define SCARD_PROTOCOL_ANY (SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1) 

#else

#endif
#endif
