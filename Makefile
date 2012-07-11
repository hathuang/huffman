CC=gcc
CFLAGS=-I. -DDebug -g -O2
DEPS = huffman.h
OBJ = main.o huffman.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

huffman: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ core
