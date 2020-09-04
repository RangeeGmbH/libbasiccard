/* (C) 2004 Christian Zietz */
/* This is a demo for libbasiccard and the calculator example that comes */
/* with the BasicCard SDK. Usage example: ./calcdemo "17+4"  */

#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <zccri.h>
#include <zcbci.h>

#ifndef NO_CHZBCI
#include <chzbci.h>
#endif

int main (int argc, char *argv[]) {
 ZCCRIREADER r;
 ZCBCICARD c;
 ZCCRIREADERNAME n;
 ZCCRIRET rv;
 ZCCRIATR atr;
 WORD SW1SW2;
 BYTE state, algo;
 WORD ver, CID;

 ZCBCIPARM parm[2];
 char temp[250];
 
 rv = ZCCriGetDefaultReader(n);
 
 if (rv != ZCCRI_NOERROR) {
   printf("Default reader doesn't exist or is unknown!\n");
   return 1;
 }

 rv = ZCCriOpenReader(&r, n);
 // rv = ZCCriSetLogFile(r, "CON:");
 rv = ZCCriWaitCard(r, 0);
 if (rv != ZCBCI_NOERROR) {
   printf("Insert card into %s\n", n);
   rv = ZCCriWaitCard(r, -1);
 }

#ifdef _WIN32
 Sleep(200);
#endif
 rv = ZCCriConnect(r, &atr);
 rv = ZCBciAttach(r, &c);
 rv = ZCBciGetApplicationID(c, &SW1SW2, temp, 249);
 if (strcmp(temp, "BasicCard Mini-Calculator")!=0) {
   printf("The BasicCard Mini-Calculator is not loaded onto this card!\n");
   return 1;
 }

 rv = ZCBciSetKey(c, 1, (BYTE *)"\xD1\x47\x52\xF8\xE2\xD8\xFE\xDA", 8);

 rv = ZCBciGetState(c, &SW1SW2, &state, &ver);
 if (ver >= 0x600) { // Multi Application Card
#ifndef NO_CHZBCI
   algo = 0;
   rv = CHZBciFindComponent(c, &SW1SW2, CHZBCI_COMP_KEY, "Key1", &CID);
   rv = CHZBciStartMultiAppEncryption(c, &SW1SW2, &algo, 1, CID);
#endif
 } else {
   rv = ZCBciStartEncryption(c, &SW1SW2, 0, 1, 0);
 }

 if (argc>1) {
   strncpy(temp, argv[1], 249);
 } else {
   strcpy(temp, "0");
 }

 parm[0].parmtype = ZCBCISTRING;
 parm[0].data.valstring.cbString = 250;
 parm[0].data.valstring.pString = temp;

 printf("%s = ", parm[0].data.valstring.pString);
 rv = ZCBciTransaction2(c, 0x20, 0x1, 0, 0, ZCBCI_DEFAULT_LC, ZCBCI_DEFAULT_LE, parm, 1, &SW1SW2);
 printf("%s\n", (rv == ZCBCI_NOERROR)?parm[0].data.valstring.pString:"(Error)");
 
 rv = ZCBciDetach(c);

 rv = ZCCriDisconnect(r);
 rv = ZCCriCloseReader(r);
 return 0;
}
