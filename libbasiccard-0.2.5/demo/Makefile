CFLAGS += -I../include -L..

demos: bcsetreader calcdemo

bcsetreader: bcsetreader.o
	$(CC) $(CFLAGS) -lbasiccard -o $@ $^ 

calcdemo: calcdemo.o
	$(CC) $(CFLAGS) -lbasiccard -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f bcsetreader calcdemo *.o 
