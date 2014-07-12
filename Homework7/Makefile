#Nicholas Jordan
#CS 311 
#Makefile for homework 7

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall
PROCFLAGS=-lrt -lm
THREADFLAGS=$(PROCFLAGS) -pthread
PROGS=primeMProc primePThread


all: $(PROGS)

primeMProc: primeMProc.o
	$(CC) $(CFLAGS) -o primeMProc primeMProc.o $(PROCFLAGS)

primeMProc.o: primeMProc.c
	$(CC) $(CFLAGS) -c primeMProc.c 

primePThread: primePThread.o
	$(CC) $(CFLAGS) -o primePThread primePThread.o $(THREADFLAGS)

primePThread.o: primePThread.c
	$(CC) $(CFLAGS) -c primePThread.c 

clean:
	rm -f $(PROGS) *.o *~
