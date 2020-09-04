#ifndef __KEYPARSE_H__
#define __KEYPARSE_H__

#ifdef __cplusplus
extern "C" {
#endif


struct bci_int_parm {
  void (*callback)(int,int,unsigned char *,int);
  int cardindex;
};

int bci_int_getkey2(FILE* f, struct bci_int_parm* parm);

#ifdef __cplusplus
}
#endif

#endif
