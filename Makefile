CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic
EOBJ = huffman.o stack.o queue.o bv.o encode.o
DOBJ = huffman.o stack.o queue.o bv.o decode.o

.PHONY: all
all: encode decode

encode: $(EOBJ)
	$(CC) $(CFLAGS) $(EOBJ) -g -o encode

decode: $(DOBJ)
	$(CC) $(CFLAGS) $(DOBJ) -g -o decode

encode.o: encode.c
	$(CC) $(CFLAGS) -g -c encode.c

decode.o: decode.c
	$(CC) $(CFLAGS) -g -c decode.c

stack.o: stack.c
	$(CC) $(CFLAGS) -g -c stack.c

huffman.o: huffman.c
	$(CC) $(CFLAGS) -g -c huffman.c

queue.o: queue.c
	$(CC) $(CFLAGS) -g -c queue.c

bv.o: bv.c
	$(CC) $(CFLAGS) -g -c bv.c

.PHONY: clean
clean:
	rm $(EOBJ) decode.o encode decode
