SRCS = sprinkler.c sprinkler-http.c sprinkler-ssh.c
CC = gcc
CFLAGS = -g 
OS = OSX
ARCH = ARM

ifeq ($(OS),Windows_NT)
	OS = WIN32
	ifeq ($(PROCESSOR_ARCHITEW6432),AMD64)
		ARCH = AMD64
	else
		ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
			ARCH = AMD64
		endif
		ifeq ($(PROCESSOR_ARCHITECTURE),x86)
			ARCH = IA32
		endif
	endif
else
	ifeq ($(shell uname -s),Linux)
		OS = LINUX
	endif
	ifeq ($(shell uname -s),Darwin)
		OS = OSX
	endif
	ifeq ($(shell uname -p),x86_64)
		ARCH = AMD64
	endif
	ifneq ($(filter %86,$(shell uname -p)),)
		ARCH = IA32
	endif
	ifneq ($(filter arm%,$(shell uname -p)),)
		ARCH = ARM
	endif
endif

all: sprinkler

sprinkler: $(SRCS)
ifeq ($(OS),OSX)
	$(CC) $(CFLAGS) -rpath ./prompt_ssh/ -lssh -lssl -lcrypto $(SRCS) -o $@
endif
ifeq ($(OS),LINUX)
	$(CC) $(CFLAGS) -o $@ -lssh $(SRCS) -lm -lssh -lssl -lcrypto
endif
	rm -f *.o

sprinkler2: $(SRCS) #Sam's compiler
	$(CC) $(CFLAGS) -o $@ -lssh sprinkler.c sprinkler-ssh.c sprinkler-http.c -lm -lssh -lssl -lcrypto
	rm -f *.o

clean:
	rm -f *.o
	rm -f thing


