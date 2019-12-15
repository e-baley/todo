CC=gcc
CFLAGS=-g -Wall

todo: main.c
	$(CC) $(CFLAGS) -o todo main.c

clean:
	rm -rf todo
