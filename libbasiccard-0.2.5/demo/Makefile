CFLAGS += -I../include -L..

demos: bcsetreader calcdemo

bcsetreader: bcsetreader.o
	$(CC) $(CFLAGS) -o $@ $^ -lbasiccard

calcdemo: calcdemo.o
	$(CC) $(CFLAGS) -o $@ $^ -lbasiccard

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f bcsetreader calcdemo *.o 
