/******************************************************************************/
/* Copyright      : ZeitControl cardsystems GmbH 1999                         */
/* Author         : Michael Petig                                             */
/* Modified by    : Christian Zietz for libbasiccard                          */      
/******************************************************************************/
/* Project        : Common-Reader-Interface                                   */
/* Description    : Global Header defines types used by CRI                   */
/* History        : 19990211 MP initial version                               */
/******************************************************************************/

#ifndef __ZCCRITYP_H__
#define __ZCCRITYP_H__

#pragma pack(1)

#ifndef CHAR
typedef char CHAR;
typedef short SHORT;
#endif

/* common data types */
typedef DWORD ZCCRIRET;  /* 32 bit CRI return value */
typedef ZCCRIRET *PZCCRIRET;
typedef WORD  ZCCRIVER;  /* 16 bit version number   */
typedef ZCCRIVER *PZCCRIVER;
typedef DWORD ZCCRIHANDLE; /* 32 bit handle */
typedef ZCCRIHANDLE *PZCCRIHANDLE;
typedef ZCCRIHANDLE ZCCRIREADER; /* reader handle */
typedef ZCCRIREADER *PZCCRIREADER;
/* CRI service name */
typedef CHAR ZCCRISERVICENAME[ZCCRI_MAX_SERVICE_NAME+1];
typedef CHAR *PZCCRISERVICENAME;
/* CRI reader name */
typedef CHAR ZCCRIREADERNAME[ZCCRI_MAX_READER_NAME+1];
typedef CHAR *PZCCRIREADERNAME;
/* CRI ATR (history only) */
typedef struct _ZCCRIATR
{
	BYTE bLength;
	BYTE abAtr[ZCCRI_MAX_ATR+1];
} ZCCRIATR;
typedef ZCCRIATR *PZCCRIATR;

#pragma pack()

#endif
