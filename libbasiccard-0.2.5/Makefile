CC = gcc
CXX = g++
CPPFLAGS = -O3 -Wall -I. -Iinclude -Iexternal -I../libtomcrypt-1.16/src/headers -I/usr/include/PCSC -I/usr/local/include/PCSC -DUSE_TOMSRANDOM -fpic

OBJECTS = abstractcard.o \
abstractcrypt.o \
abstractdriver.o \
abstractreader.o \
basiccard.o \
bccommandapdu.o \
bcresponseapdu.o \
commandapdu.o \
desaescrypt.o \
eaxcrypt.o \
omaccrypt.o \
pcscdriver.o \
pcscreader.o \
responseapdu.o \
zcbci.o \
zccri.o \
external/keyparse.o \
external/keyparse.tab.o \
external/my3des.o \
external/mydes.o \
external/myaes.o \
external/crc.o \
external/myrandom.o

all: libbasiccard demos

demos:
	$(MAKE) -C demo

libbasiccard: libbasiccard.a libbasiccard.so.0.2.5

libbasiccard.so.0.2.5: $(OBJECTS)
	$(CXX) $(CPPFLAGS) -L/usr/local/lib -Wl,-hlibbasiccard.so.0 -shared -o $@ $^ ../libtomcrypt-1.16/libtomcrypt.a -lpcsclite -lpthread
	ln -f -s $@ libbasiccard.so.0
	ln -f -s $@ libbasiccard.so

libbasiccard.a: $(OBJECTS)
	ar $(ARFLAGS) $@ $(OBJECTS)
	sh ../misc/mergelib.sh $@ ../libtomcrypt-1.16/libtomcrypt.a
	ranlib $@

PREFIX ?= $(DESTDIR)/usr
install: libbasiccard demos
	install -m 0755 -d $(PREFIX)/bin/
	install -m 0755 demo/bcsetreader $(PREFIX)/bin/
	install -m 0755 -d $(PREFIX)/lib/
	install -m 0644 libbasiccard.a libbasiccard.so.0.2.5 $(PREFIX)/lib/
	ln -f -s libbasiccard.so.0.2.5 $(PREFIX)/lib/libbasiccard.so.0
	ln -f -s  libbasiccard.so.0.2.5 $(PREFIX)/lib/libbasiccard.so
	install -m 0755 -d $(PREFIX)/include/
	install -m 0644 include/*.h $(PREFIX)/include/
	install -m 0755 -d $(PREFIX)/doc/libbasiccard/
	install -m 0644 docs/*.pdf $(PREFIX)/doc/libbasiccard/
	-ldconfig


# zccri also depends from its include files
zccri.o: zccri_inc_win32.cpp zccri_inc_unix.cpp

# don't rebuild parser by default
external/keyparse.c:

external/keyparse.tab.c:

# use "make parser" to rebuild parser
parser: external/keyparse.l external/keyparse.y
	flex -i -oexternal/keyparse.c external/keyparse.l
	bison -d -oexternal/keyparse.tab.c external/keyparse.y

clean:
	-rm *.o *.a external/*.o libbasiccard.so*
	$(MAKE) -C demo clean

%.o: %.cpp
	$(CXX) $(CPPFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CPPFLAGS) -c -o $@ $<
