SRCS = sprinkler.c sprinkler-http.c sprinkler-ssh.c
CC = gcc
CFLAGS = -g 
OS = OSX
ARCH = ARM
LIBSSH = 

# Detecting OS and architecture. Code from Trevor Robinson and Somu on 
# https://stackoverflow.com/questions/714100/os-detecting-makefile
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
	$(CC) $(CFLAGS) $(LIBSSH) -lssh -lssl -lcrypto $(SRCS) -o $@
endif
ifeq ($(OS),LINUX)
	$(CC) $(CFLAGS) -o $@ -lssh $(SRCS) -lm -lssh -lssl -lcrypto
endif
	rm -f *.o

install: sprinkler
	sudo cp -r ./help /usr/local/bin/sprinkler-help
	sudo cp sprinkler /usr/local/bin/sprinkler

uninstall:
	rm -f /usr/local/bin/sprinkler
	rm -r /usr/local/bin/sprinkler-help

clean:
	rm -f *.o
	rm -f sprinkler


