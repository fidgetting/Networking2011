CC=gcc
CFLAGS=-Wall -lpthread -I . 

all: ss.o

ss.o: ss.c
	$(CC) $(CFLAGS) ss.c -o ss.out

clean:
	rm -rf *.out
