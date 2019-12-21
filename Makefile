CC=gcc
CFLAGS=-g -Wall
LFLAGS=-lm
DEPS=todo_lib.h
OBJS=main.o todo_lib.o


%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) $(LFLAGS) -c -o $@ $<

todo: $(OBJS) 
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $^

clean:
	rm -rf $(OBJS) todo
