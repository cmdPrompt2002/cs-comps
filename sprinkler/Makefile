SRCS = sprinkler.c sprinkler-http.c sprinkler-ssh.c
OBJS = $(SRCS:.c=.o)
CC = gcc
CFLAGS = -g 

all: sprinkler

sprinkler: $(OBJS)
	$(CC) $(CFLAGS) -rpath ./prompt_ssh/ -lssh -lssl -lcrypto $^ -o $@
	rm -f *.o

sprinkler2: #Sam's compiler
	$(CC) $(CFLAGS) -o $@ -lssh thing.c thing-ssh.c
	rm -f *.o

clean:
	rm -f *.o
	rm -f thing


