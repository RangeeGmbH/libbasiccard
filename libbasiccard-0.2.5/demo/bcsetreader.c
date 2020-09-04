#ifdef _WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <zccri.h>

int main(int argc, char *argv[]) {
    ULONG c;
	ZCCRIREADERNAME n;
	ZCCRIRET r;

#ifndef _WIN32	
	if (argc>1) {
		printf("Usage: bcsetreader\nAsks the user for the default smard card reader to be used by libbasiccard.\n");
		return 1;
	}
#endif	

	r = ZCCriGetReaderCount(&c);
	if ((r!=ZCCRI_NOERROR) || (c==0)) {
#ifdef _WIN32
		MessageBox(0, "No smart card readers found!", "Error", MB_ICONSTOP);
#else
		fprintf(stderr, "No smart card readers found!\n");
#endif
		return 1;
	}

	if (ZCCriSelectReaderDialog(n)==ZCCRI_NOERROR) {
		r = ZCCriSetDefaultReader(n);
		if (r==ZCCRI_NOERROR) {
#ifdef _WIN32
			MessageBox(0, n, "Default smart card reader:", MB_ICONINFORMATION);
#else
			printf("'%s' has been set as default smart card reader.\n", n);
#endif
	    }
	}

	return 1;
}
