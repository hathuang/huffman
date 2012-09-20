CC=gcc
CFLAGS =-I. -g -Wall -O0
#CFLAGS +=-DDebug
DEPS = huffman.h syslog.h
OBJ = main.o huffman.o syslog.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

huffman: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ core
