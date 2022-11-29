CC=gcc
CFLAGS=-Wall -Wextra

default: main

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

main: main.o
	$(CC) main.o -o main

clean:
	-rm -f main.o
	-rm -f main
	-rm -f *.ppm
