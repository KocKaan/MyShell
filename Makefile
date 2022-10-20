CC=gcc 
CFLAGS=-g -pedantic -std=gnu17 -Wall -Wextra
LDFLAGS=-lreadline

.PHONY: all
all: nyush

nyush: nyush.o 
nyush.o: nyush.c

.PHONY: clean
clean: rm -f *.o nyush
	