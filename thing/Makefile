SRCS = thing.c thing-ssh.c
OBJS = $(SRCS:.c=.o)
CC = gcc
CFLAGS = -g 

all: thing

thing: $(OBJS)
	$(CC) $(CFLAGS) -rpath ./prompt_ssh/ -lssh $^ -o $@
	rm -f *.o

thing2: #Sam's compiler
	$(CC) $(CFLAGS) -o $@ -lssh thing.c thing-ssh.c
	rm -f *.o

clean:
	rm -f *.o
	rm -f thing


