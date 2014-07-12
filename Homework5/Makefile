#Nicholas Jordan
#CS 311 
#Makefile for homework5

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall -Wshadow -Wunreachable-code -Wredundant-decls -Wmissing-declarations -Wold-style-definition -Wmissing-prototypes -Wdeclaration-after-statement
PROGS=homework5


all: $(PROGS)


homework5: homework5.o
	$(CC) $(CFLAGS) -o homework5 homework5.o

homework5.o: homework5.c
	$(CC) $(CFLAGS) -c homework5.c


test:
	rm -f my.txt
	rm -f sample.txt
	rm -f myoutput.txt
	rm -f sampleoutput.txt
	./example.bash -f sample.txt < words.txt > sampleoutput.txt
	./homework5 -f my.txt < words.txt > myoutput.txt
	diff my.txt sample.txt
	diff myoutput.txt sampleoutput.txt

clean:
	rm -f $(PROGS) *.o *~

cleantest:
	rm -f my.txt
	rm -f sample.txt
	rm -f myoutput.txt
	rm -f sampleoutput.txt