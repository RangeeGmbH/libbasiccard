%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "keyparse.h"

void yyerror(const char *str)
{
}

#define YYPARSE_PARAM parm
#define DYN_BUFSIZE 64

int keysize, bufsize, keynum, keycount;
int numofkeys;

unsigned char *keydata;

extern FILE* yyin;

int bci_int_getkey2(FILE* f, struct bci_int_parm* parm)
{
  yyin = f;
  numofkeys = 0;
  yyparse((void *)parm);
  return numofkeys;
}

%}

%token DECLARECMD KEY NUMBER NEWLINE

%%
commands: /* empty */
        |
        commands command
        ;

command:
        declarecommand NEWLINE
        |
        error NEWLINE { yyerrok; }
        ;

declarecommand:
        declareheader declaredata
        {
          if (bufsize == DYN_BUFSIZE) {
            keysize = keycount;
          } else {
            for ( ; keycount<bufsize; keycount++) {
              keydata[keycount] = 0;
            }
            keysize = bufsize;
          }

          ((struct bci_int_parm *)parm)->callback(((struct bci_int_parm *)parm)->cardindex, keynum, keydata, keysize);
          numofkeys++;

          if (bufsize>0) {
            free(keydata);
          }
        }
        |
        declareheader error
        {
          if (bufsize>0) {
            free(keydata);
          }
          yyerrok;
        }
        ;

declareheader:
        DECLARECMD KEY NUMBER size
        {
          keynum = $3;
          bufsize = $4;
          if ((keydata = malloc(bufsize)) == NULL) {
            bufsize = 0;
          }
          keycount = 0;
        }
        ;

size:   /* empty */
        {
          $$ = DYN_BUFSIZE;
        }
        |
        '(' NUMBER ')'
        {
          $$ = $2;
        }
        |
        '(' NUMBER ',' NUMBER ')'
        {
          $$ = $2;
        }
        ;

declaredata:
        /* empty */
        |
        '=' listofbytes
        ;

listofbytes:
        keybyte ',' listofbytes | keybyte
        ;

keybyte:
        NUMBER
        {
          if (keycount<bufsize) {
            keydata[keycount] = (unsigned char) $1;
            keycount++;
          }
        }
        ;
