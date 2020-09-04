/******************************************************************************/
/* Copyright      : ZeitControl cardsystems GmbH 1999                         */
/* Author         : Michael Petig                                             */
/* Modified by    : Christian Zietz for libbasiccard                          */      
/******************************************************************************/
/* Project        : Common-Reader-Interface                                   */
/* Description    : Global Header defines funtions used by CRI                */
/* History        : 19990211 MP initial version                               */
/*                  20000614 MP prepare to create import libraries            */
/******************************************************************************/

#ifndef __ZCCRIPRT_H__
#define __ZCCRIPRT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* ZeitControl internal use for creating import libraries */
#ifdef ZCMAKEDLLCRI
#define ZCDLLEXP __declspec(dllexport)
#define ZCEXPD ;
#endif

#ifdef ZCMAKEIMPCRI
#define ZCDLLEXP __declspec(dllexport)
#define ZCEXPD { return 0; }
#endif

#ifndef ZCEXPD
#define ZCDLLEXP
#define ZCEXPD ;
#endif

/* API version 1.1 */
#define ZCCRI_API      0x200
#define ZCCRI_API_1_0
#define ZCCRI_API_1_1
#define ZCCRI_API_2_0

/******************************************************************************/
/* Function       : ZCCriGetReaderCount                                       */
/* Description    : get number of known readers                               */
/* Parameter      : (OUT) *pCount number of known readers                     */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer          */
/******************************************************************************/
/* History        : 19990211 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetReaderCount(PULONG pCount) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriGetReaderName                                        */
/* Description    : get name of single reader supported by any service        */
/* Parameter      : (in) ulNum number of requested reader (0 .. readercount-1)*/
/*                  (OUT) *pName name of requested reader                     */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer          */
/*                  ZCCRI_ERROR_UNKOWN_READER reader not known, ulNum to big  */
/******************************************************************************/
/* History        : 19990211 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetReaderName(ULONG ulNum, 
											   PZCCRIREADERNAME pName) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriOpenReader                                           */
/* Description    : open reader                                               */
/* Parameter      : (out) *phReader handle to reader                          */
/*                  (in)  *pName name of requested reader (NULL for default)  */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer          */
/*                  ZCCRI_ERROR_UNKOWN_READER reader not known                */
/*                  ZCCRI_ERROR_READER_BUSY reader busy or not accessible     */
/******************************************************************************/
/* History        : 19990303 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriOpenReader(PZCCRIREADER phReader, 
											PZCCRIREADERNAME pName) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriCloseReader                                          */
/* Description    : close reader                                              */
/* Parameter      : (in) hReader handle to connection                         */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer          */
/******************************************************************************/
/* History        : 19990303 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriCloseReader(ZCCRIREADER hReader) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriWaitCard                                             */
/* Description    : wait timout ms for card to be inserted                    */
/* Parameter      : (in) hReader handle to connection                         */
/*                  (in) lTimeout timeout in ms (0 no wait, -1 infinite wait) */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer          */
/*                  ZCCRI_ERROR_NO_CARD no card in reader                     */
/*                  ZCCRI_ERROR_READER_BUSY reader busy                       */
/******************************************************************************/
/* History        : 19990303 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriWaitCard(ZCCRIREADER hReader, 
										  LONG lTimeout) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriConnect                                              */
/* Description    : connect to card in given reader                           */
/* Parameter      : (in) hReader handle to reader                             */
/*                  (OUT) *pAtr card ATR NOTE pAtr may be NULL                */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer          */
/*                  ZCCRI_ERROR_UNKOWN_READER reader not known                */
/*                  ZCCRI_ERROR_READER_BUSY reader busy or not accessible     */
/*                  ZCCRI_ERROR_NO_CARD no card in reader                     */
/******************************************************************************/
/* History        : 19990211 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriConnect(ZCCRIREADER hReader, 
										 PZCCRIATR pAtr) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriReconnect                                            */
/* Description    : reconnect to card in reader or in other words reset card  */
/* Parameter      : (in) hReader handle to open connection                    */
/*                  (OUT) *pAtr card ATR NOTE pAtr may be NULL                */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer, invalid */
/*                                   handle                                   */
/*                  ZCCRI_ERROR_NO_CARD no card in reader                     */
/******************************************************************************/
/* History        : 19990211 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriReconnect(ZCCRIREADER hReader, 
										   PZCCRIATR pAtr) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriDisconnect                                           */
/* Description    : disconnect from card and reader                           */
/* Parameter      : (in) hReader handle to open connection                    */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, invalid handle        */
/******************************************************************************/
/* History        : 19990211 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriDisconnect(ZCCRIREADER hReader) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriTransaction                                          */
/* Description    : send (and receive) T1 transaction to smartcard            */
/* Parameter      : (in) hReader handle to open connection                    */
/*                  (in) *pDataIn input data (Cla, Ins, P1, P2, ..)           */
/*                  (in) cbData   size of input data                          */
/*                  (out) *pReply reply data                                  */
/*                  (in/out) *pcbReply (in) size of reply buffer              */
/*                                     (out) number of bytes received         */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer,         */
/*                                   invalid handle                           */
/*                  ZCCRI_ERROR_PROTOCOL protocol error transaction failed    */
/*                  ZCCRI_ERROR_NO_CARD no card in reader                     */
/*                  ZCCRI_ERROR_OVERFLOW buffer (pReply) overflow             */
/******************************************************************************/
/* History        : 19990211 MP initial version                               */
/*                  19990303 MP completly change call                         */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriTransaction(ZCCRIREADER hReader, 
									BYTE *pDataIn, DWORD cbDataIn,
									BYTE *pReply, DWORD *pcbReply) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriTransaction2                                         */
/* Description    : send (and receive) T1 transaction to smartcard            */
/* Parameter      : (in) hReader handle to open connection                    */
/*                  (in) Cla  class byte                                      */
/*                  (in) Ins  instruction byte                                */
/*                  (in) P1   parameter 1                                     */
/*                  (in) P2   parameter 2                                     */
/*                  (in) Lc   length of additional data                       */
/*                  (in) *pData additional data                               */
/*                  (in) Le   length of expected reply (0..254 or -1 to omit) */
/*                  (out) *pReply reply data (without SW1, SW2) may be NULL   */
/*                  (in/out) *pcbReply (in) length of reply buffer pReply     */
/*                                    (out) received length of data (without  */
/*                                          SW1SW2)                           */
/*                  (out) *pSW1SW2 SW1, SW2                                   */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer,         */
/*                                   invalid handle                           */
/*                  ZCCRI_ERROR_PROTOCOL protocol error transaction failed    */
/*                  ZCCRI_ERROR_NO_CARD no card in reader                     */
/******************************************************************************/
/* History        : 19990303 MP initial version                               */
/*                  19990421 MP change reply lentgth, le parameter add cbReply*/
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriTransaction2(ZCCRIREADER hReader, BYTE Cla, BYTE Ins,
								     BYTE P1, BYTE P2, BYTE Lc, BYTE *pData,
									 SHORT Le, BYTE *pReply, SHORT *pcbReply,
									 WORD *pSW1SW2) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriSetLogFile                                           */
/* Description    : logs all input and output data to log file                */
/* Parameter      : (in) hReader handle to open connection                    */
/*                  (in) pszFile name of log file or NULL to disable          */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer,         */
/*                                   invalid handle                           */
/*                  ZCCRI_ERROR_MISC unable to open file                      */
/******************************************************************************/
/* History        : 19990305 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriSetLogFile(ZCCRIREADER hReader, 
											CHAR *pszFile) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriRegisterService                                      */
/* Description    : register a RWL service dll                                */
/* Parameter      : CHAR *pszService                                          */
/*                  CHAR *pszDllName DLL to register                          */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer          */
/*                  ZCCRI_ERROR_MISC unknown error                            */
/******************************************************************************/
/* History        : 19990215 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriRegisterService(CHAR *pszService,
												 CHAR *pszDLLName) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriDeregisterService                                    */
/* Description    : deregister a RWL service dll                              */
/* Parameter      : CHAR *pszService service to deregister                    */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer          */
/*                  ZCCRI_ERROR_MISC unknown error                            */
/******************************************************************************/
/* History        : 19990215 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriDeregisterService(CHAR *pszService) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriSetDefaultReader                                     */
/* Description    : set default reader used when NULL or empty String is used */
/*                  as reader name in ZCCriConnect                            */
/* Parameter      : (in) *pName name of requested reader                      */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer          */
/*                  ZCCRI_ERROR_MISC unknown error                            */
/******************************************************************************/
/* History        : 19990211 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriSetDefaultReader(PZCCRIREADERNAME pName) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriGetDefaultReader                                     */
/* Description    : get default reader used when NULL or empty String is used */
/*                  as reader name in ZCCriConnect                            */
/* Parameter      : (out) *pName name of requested reader                     */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer          */
/*                  ZCCRI_ERROR_NO_DEFAULT no default reader                  */
/******************************************************************************/
/* History        : 19990211 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetDefaultReader(PZCCRIREADERNAME pName) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriSelectReaderDialog                                   */
/* Description    : show dialog to select reader                              */
/* Parameter      : (out) *pName name of selected reader                      */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter, null pointer          */
/*                  ZCCRI_CANCEL dialog canceled                              */
/******************************************************************************/
/* History        : 19990211 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriSelectReaderDialog(PZCCRIREADERNAME pName) ZCEXPD

#ifdef ZCCRI_API_1_1
/* new with version 1.1 */
/******************************************************************************/
/* Function       : ZCCriGetFeatures                                          */
/* Description    : return features supported by give reader, service         */
/* Parameter      : (in)  pName  name of reader, service (e.g. "PCSC:") or    */
/*                        NULL for default readerto query                     */
/*                  (out) *pdwFeatures bitmap of ZCCRI_FEATURE_...            */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter                        */
/*                  ZCCRI_ERROR_UNKOWN_READER reader not known                */
/******************************************************************************/
/* History        : 20000114 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetFeatures(PZCCRIREADERNAME pName, 
											 DWORD *pdwFeatures) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriCardInReader                                         */
/* Description    : Check if card is in reader without open, block reader     */
/*                  If supported ZCCRI_FEATURE_CHECKCARD will be returned by  */
/*                  ZCCriGetFeatures. If supported with returning ATR         */
/*                  ZCCRI_FEATURE_CHECKCARD_ATR will also be returned         */
/* Parameter      : (in)  pName  name of reader to query                      */
/*                  (out) *pAtr  ATR of card in reader (if supported)         */
/* Returnvalue    : ZCCRI_NOERROR  no error (card in reader)                  */
/*                  ZCCRI_ERROR_NO_CARD no card in reader                     */
/*                  ZCCRI_ERROR_PARM invalid parameter                        */
/*                  ZCCRI_ERROR_UNKOWN_READER reader not known                */
/*                  ZCCRI_ERROR_NOT_SUPPORTED this call is not supported by   */
/*                                            this reader or service          */
/******************************************************************************/
/* History        : 20000114 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriCardInReader(PZCCRIREADERNAME pName, 
											  PZCCRIATR pAtr) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriSetOptions                                           */
/* Description    : set options for open reader                               */
/* Parameter      : (in)  hReader handle of reader to set                     */
/*                  (in)  dwOptions bitmap of ZCCRI_OPTION_...                */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter                        */
/*                  ZCCRI_ERROR_UNKOWN_READER reader not known                */
/*                  ZCCRI_ERROR_NOT_SUPPORTED not supported by this service   */
/******************************************************************************/
/* History        : 20000114 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriSetOptions(ZCCRIREADER hReader, 
											DWORD dwOptions) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriGetOptions                                           */
/* Description    : get options for open reader                               */
/* Parameter      : (in)  hReader handle of reader to get                     */
/*                  (out)  *pdwOptions bitmap of ZCCRI_OPTION_...             */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter                        */
/*                  ZCCRI_ERROR_UNKOWN_READER reader not known                */
/*                  ZCCRI_ERROR_NOT_SUPPORTED not supported by this service   */
/******************************************************************************/
/* History        : 20000114 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetOptions(ZCCRIREADER hReader, 
											DWORD *pdwOptions) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriSetOptions2                                          */
/* Description    : set options for all or single service                     */
/*                  Note: When use for ZCCRI_OPTION_FASTINIT this must be     */
/*                        first call to ZCCri... functions and pServiceName   */
/*                        must be NULL                                        */
/* Parameter      : (in)  pServiceName name of service to set or NULL for all */
/*                                     services                               */
/*                  (in)  dwOptions bitmap of ZCCRI_OPTION_...                */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter                        */
/*                  ZCCRI_ERROR_UNKOWN_READER reader not known                */
/*                  ZCCRI_ERROR_NOT_SUPPORTED not supported by this service   */
/******************************************************************************/
/* History        : 20000114 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriSetOptions2(PZCCRISERVICENAME pServiceName, 
											 DWORD dwOptions) ZCEXPD

/******************************************************************************/
/* Function       : ZCCriGetOptions2                                          */
/* Description    : get options for single service                            */
/* Parameter      : (in)  pServiceName name of service to set                 */
/*                                     services                               */
/*                  (out)  *pdwOptions bitmap of ZCCRI_OPTION_...             */
/* Returnvalue    : ZCCRI_NOERROR  no error                                   */
/*                  ZCCRI_ERROR_PARM invalid parameter                        */
/*                  ZCCRI_ERROR_UNKOWN_READER reader not known                */
/*                  ZCCRI_ERROR_NOT_SUPPORTED not supported by this service   */
/******************************************************************************/
/* History        : 20000114 MP initial version                               */
/******************************************************************************/
ZCDLLEXP ZCCRIRET ZCCRILINK ZCCriGetOptions2(PZCCRISERVICENAME pServiceName, 
											 DWORD *pdwOptions) ZCEXPD
#endif

#ifdef __cplusplus
}
#endif

#endif
