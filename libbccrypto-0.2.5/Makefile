CC = gcc
CXX = g++
AR = ar
CPPFLAGS = -O3 -Iinclude -I../libtomcrypt-1.16/src/headers -fpic

OBJECTS = bccrypto.o

ifdef ECC_BASICCARD
CPPFLAGS += -I../libtommath-0.41 -DECC_BASICCARD
OBJECTS += fldconv.o ecc.o
MATHLIB = ../libtommath-0.41/libtommath.a
endif

libbccrypto: libbccrypto.a libbccrypto.so.0.2.5

libbccrypto.so.0.2.5: $(OBJECTS)
	$(CXX) $(CPPFLAGS) -Wl,-hlibbccrypto.so.0 -shared -o $@ $^ ../libtomcrypt-1.16/libtomcrypt.a $(MATHLIB) -lpthread
	ln -f -s $@ libbccrypto.so.0
	ln -f -s $@ libbccrypto.so

libbccrypto.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)
	sh ../misc/mergelib.sh $@ ../libtomcrypt-1.16/libtomcrypt.a
ifdef ECC_BASICCARD
	sh ../misc/mergelib.sh $@ $(MATHLIB)
endif
	ranlib $@

PREFIX ?= $(DESTDIR)/usr
install: libbccrypto
	install -m 0755 -d $(PREFIX)/lib/
	install -m 0644 libbccrypto.a libbccrypto.so.0.2.5 $(PREFIX)/lib/
	ln -f -s libbccrypto.so.0.2.5 $(PREFIX)/lib/libbccrypto.so.0
	ln -f -s  libbccrypto.so.0.2.5 $(PREFIX)/lib/libbccrypto.so
	install -m 0755 -d $(PREFIX)/include/
	install -m 0644 include/*.h $(PREFIX)/include/
	-ldconfig

ecc.o: ecc_inc.cpp

clean:
	-rm *.o *.a libbccrypto.so*

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<
