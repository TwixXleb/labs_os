
CC=gcc
CFLAGS=-Iinclude

all: main

main: src/parent.o src/child.o src/utils.o test/test.o
	$(CC) -o main src/parent.o src/child.o src/utils.o test/test.o

src/parent.o: src/parent.c
	$(CC) $(CFLAGS) -c src/parent.c

src/child.o: src/child.c
	$(CC) $(CFLAGS) -c src/child.c

src/utils.o: src/utils.c
	$(CC) $(CFLAGS) -c src/utils.c

test/test.o: test/test.c
	$(CC) $(CFLAGS) -c test/test.c

clean:
	rm -f *.o main
