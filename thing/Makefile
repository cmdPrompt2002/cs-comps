SRCS = thing.c 
OBJS = $(SRCS:.c=.o)
CC = gcc
CFLAGS = -g 

all: thing

thing: $(OBJS)
	$(CC) $(CFLAGS) -rpath . -lssh $^ -o $@
	rm -f *.o

clean:
	rm -f *.o
	rm -f thing


