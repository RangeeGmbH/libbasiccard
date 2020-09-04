/******************************************************************************/
/* Copyright      : ZeitControl cardsystems GmbH 1999                         */
/* Author         : Michael Petig                                             */
/* Modified by    : Christian Zietz for libbasiccard                          */      
/******************************************************************************/
/* Project        : Common-Reader-Interface                                   */
/* Description    : Global Header defines constants used by CRI               */
/* History        : 19990211 MP initial version                               */
/******************************************************************************/

#ifndef __ZCCRIDEF_H__
#define __ZCCRIDEF_H__

/* Link option */
#ifdef _WIN32
#define ZCCRILINK APIENTRY
#else
#define ZCCRILINK
#endif

#define ZCCRIENTRY ZCCRILINK

/* global values */
#define ZCCRI_MAX_SERVICE_NAME 16
#define ZCCRI_MAX_READER_NAME  256
#define ZCCRI_MAX_ATR          34
#define ZCCRI_SEPARATOR        ':'

/* Error values */
#define ZCCRI_NOERROR                0UL
#define ZCCRI_ERROR_BASE             0x01000000UL
#define ZCCRI_ERROR_PARM             (ZCCRI_ERROR_BASE + 0x0000UL)
#define ZCCRI_ERROR_UNKOWN_READER    (ZCCRI_ERROR_BASE + 0x0001UL)
#define ZCCRI_ERROR_READER_BUSY      (ZCCRI_ERROR_BASE + 0x0002UL)
#define ZCCRI_ERROR_PROTOCOL         (ZCCRI_ERROR_BASE + 0x0003UL)
#define ZCCRI_ERROR_MEM              (ZCCRI_ERROR_BASE + 0x0004UL)
#define ZCCRI_ERROR_INITFAILED       (ZCCRI_ERROR_BASE + 0x0005UL)
#define ZCCRI_ERROR_MISC             (ZCCRI_ERROR_BASE + 0x0006UL)
#define ZCCRI_ERROR_NO_CARD          (ZCCRI_ERROR_BASE + 0x0007UL)
#define ZCCRI_ERROR_NO_DEFAULT       (ZCCRI_ERROR_BASE + 0x0008UL)
#define ZCCRI_CANCEL                 (ZCCRI_ERROR_BASE + 0x0009UL)
#define ZCCRI_ERROR_OVERFLOW         (ZCCRI_ERROR_BASE + 0x000AUL)
#define ZCCRI_ERROR_NOT_SUPPORTED    (ZCCRI_ERROR_BASE + 0x000BUL)
#define ZCCRI_ERROR_INTERNAL         (ZCCRI_ERROR_BASE + 0x000CUL)

/* new with version 1.1 */
/* CRI feature values */
#define ZCCRI_FEATURE_CHECKCARD        0x00000001UL  /* ZCCriCardInReader supported */
#define ZCCRI_FEATURE_CHECKCARD_ATR    0x00000002UL  /* ZCCriCardInReader with ATR supported */

/* CRI option values */
#define ZCCRI_OPTION_FASTINIT          0x00000001UL  /* fast init, do not check for connected readers */
#define ZCCRI_OPTION_WAITSERVICE       0x00000002UL  /* wait for service for example PC/SC resource manager */
/* Service depended options should use upper 16 bits */

#endif
