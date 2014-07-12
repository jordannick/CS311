#Nicholas Jordan
#CS 311 
#Makefile for homework 6 - uniqify

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
PROGS=uniqify


all: $(PROGS)


uniqify: uniqify.o
	$(CC) $(CFLAGS) -o uniqify uniqify.o

uniqify.o: uniqify.c
	$(CC) $(CFLAGS) -c uniqify.c


test:
	rm -f myoutput.txt
	rm -f sampleoutput.txt
	./uniquify.bash < websters.txt > sampleoutput.txt
	./uniqify -n 5 < websters.txt > myoutput.txt
	diff myoutput.txt sampleoutput.txt

test2:
	rm -f myoutput.txt
	rm -f sampleoutput.txt
	./uniquify.bash < iliad.txt > sampleoutput.txt
	./uniqify -n 5 < iliad.txt > myoutput.txt
	diff myoutput.txt sampleoutput.txt

clean:
	rm -f $(PROGS) *.o *~

cleantest:
	rm -f myoutput.txt
	rm -f sampleoutput.txt