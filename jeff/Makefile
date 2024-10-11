SRCS = jeff.c jeff-ssh.c
OBJS = $(SRCS:.c=.o)
CC = gcc
CFLAGS = -g 

all: jeff

jeff: $(OBJS)
	$(CC) $(CFLAGS) -rpath . -lssh $^ -o $@
	rm -f *.o

clean:
	rm -f *.o
	rm -f jeff


