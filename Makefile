# Master makefile for libbasiccard

export ECC_BASICCARD
export PREFIX

default: libbasiccard libbccrypto

libbasiccard:
	export CFLAGS="-fpic -DLTC_LIBBASICCARD" && $(MAKE) -C libtomcrypt-1.16
	$(MAKE) -C libbasiccard-0.2.5

libbccrypto:
	export CFLAGS="-fpic -DLTC_LIBBASICCARD" && $(MAKE) -C libtomcrypt-1.16
ifdef ECC_BASICCARD
	export CFLAGS=-fpic && $(MAKE) -C libtommath-0.41
endif
	$(MAKE) -C libbccrypto-0.2.5

install: libbasiccard libbccrypto
	$(MAKE) -C libbasiccard-0.2.5 install
	$(MAKE) -C libbccrypto-0.2.5 install

clean:
	$(MAKE) -C libtomcrypt-1.16 clean
ifdef ECC_BASICCARD
	$(MAKE) -C libtommath-0.41 clean
endif
	$(MAKE) -C libbasiccard-0.2.5 clean
	$(MAKE) -C libbccrypto-0.2.5 clean
