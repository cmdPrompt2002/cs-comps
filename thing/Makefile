SRCS = thing.c 
OBJS = $(SRCS:.c=.o)
CC = gcc
CFLAGS = -g 

all: thing

thing: $(OBJS)
	$(CC) $(CFLAGS) -rpath ./prompt_ssh/ -lssh $^ -o $@
	rm -f *.o

thing2: $(OBJS) #Sam's compiler
	$(CC) $(CFLAGS) -rpath . -lssh $^ -o $@ 
	rm -f *.o

clean:
	rm -f *.o
	rm -f thing


