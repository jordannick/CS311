/*
Nicholas Jordan
CS 311
Homework 5
----
Modified from Listing 44-4 (pipe_ls_wc.c) in 
The Linux Programming Interface by M. Kerrisk
*/

#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>

void rev(void);
void sort(void);
void uniq(void);
void tee(char *);
void wc(void);

int pfd0[2];
int pfd1[2];
int pfd2[2];
int pfd3[2];

int main(int argc, char *argv[]) {
    int c;
    int i;
    char * filename;
    if ((c=getopt(argc, argv, "f:")) != -1) {
        if (c == 'f')
	    filename = optarg;
	else {
	    printf("Usage: -f [output file] < [input file]\n");
	    exit(EXIT_FAILURE);
	}
    }

    // Create 4 Pipes
    pipe(pfd0); 
    pipe(pfd1);
    pipe(pfd2);
    pipe(pfd3);    

    //Run the 5 processes, close both ends of pipes after done being used
    rev();

    sort();

    if (close(pfd0[0]) == -1)
	exit(EXIT_FAILURE);
    if (close(pfd0[1]) == -1)
	exit(EXIT_FAILURE);
   
    uniq();

    if (close(pfd1[0]) == -1)
	exit(EXIT_FAILURE);
    if (close(pfd1[1]) == -1)
	exit(EXIT_FAILURE);

    tee(filename);

    if (close(pfd2[0]) == -1)
	exit(EXIT_FAILURE);
    if (close(pfd2[1]) == -1)
	exit(EXIT_FAILURE);

    wc();

    if (close(pfd3[0]) == -1)
	exit(EXIT_FAILURE);
    if (close(pfd3[1]) == -1)
	exit(EXIT_FAILURE);

    //Reap the 5 children
    for (i=0;i<5;i++) {
	if (wait(0) == -1)
	    exit(EXIT_FAILURE);
    }

    return 0;
}


void rev(void) {
    switch (fork()) {
    case -1:
	exit(EXIT_FAILURE);

    case 0:         
        if (close(pfd0[0]) == -1)               
	    exit(EXIT_FAILURE);

        if (pfd0[1] != STDOUT_FILENO) {         
            if (dup2(pfd0[1], STDOUT_FILENO) == -1)
		exit(EXIT_FAILURE);
            if (close(pfd0[1]) == -1)
		exit(EXIT_FAILURE);
        }

	execlp("rev", "rev", (char *) NULL);       
	exit(EXIT_SUCCESS);

    default:       
        break;
    }
    return;
}


void sort(void) {
    switch (fork()) {
    case -1:
	exit(EXIT_FAILURE);

    case 0:           
        if (close(pfd0[1]) == -1)                   
	    exit(EXIT_FAILURE);

        if (pfd0[0] != STDIN_FILENO) {       
            if (dup2(pfd0[0], STDIN_FILENO) == -1)
		exit(EXIT_FAILURE);
            if (close(pfd0[0]) == -1)
		exit(EXIT_FAILURE);
        }

	if (close(pfd1[0]) == -1)            
	    exit(EXIT_FAILURE);	

	if (pfd1[1] != STDOUT_FILENO) {              
            if (dup2(pfd1[1], STDOUT_FILENO) == -1)
		exit(EXIT_FAILURE);	
            if (close(pfd1[1]) == -1)
		exit(EXIT_FAILURE);
		}

	execlp("sort", "sort", (char *) NULL);
	exit(EXIT_SUCCESS);

    default: 
        break;
    }
    return;
}


void uniq(void) {
    switch (fork()) {
    case -1:
	exit(EXIT_FAILURE);

    case 0:            
        if (close(pfd1[1]) == -1)                    
	    exit(EXIT_FAILURE);

        if (pfd1[0] != STDIN_FILENO) {               
            if (dup2(pfd1[0], STDIN_FILENO) == -1)
		exit(EXIT_FAILURE);
            if (close(pfd1[0]) == -1)
		exit(EXIT_FAILURE);
        }

	if (close(pfd2[0]) == -1)                    
	    exit(EXIT_FAILURE);
	
	if (pfd2[1] != STDOUT_FILENO) {       
            if (dup2(pfd2[1], STDOUT_FILENO) == -1)
		exit(EXIT_FAILURE);
            if (close(pfd2[1]) == -1)
		exit(EXIT_FAILURE);
	}
	
	execlp("uniq", "uniq", "-c", (char *) NULL);
	exit(EXIT_SUCCESS);

    default: 
        break;
    }
    return;
}

void tee(char *filename) {
    switch (fork()) {
    case -1:
	exit(EXIT_FAILURE);

    case 0:            
        if (close(pfd2[1]) == -1)                  
	    exit(EXIT_FAILURE);

        if (pfd2[0] != STDIN_FILENO) {        
            if (dup2(pfd2[0], STDIN_FILENO) == -1)
		exit(EXIT_FAILURE);
            if (close(pfd2[0]) == -1)
		exit(EXIT_FAILURE);
        }

	if (close(pfd3[0]) == -1)                  
	    exit(EXIT_FAILURE);

	if (pfd3[1] != STDOUT_FILENO) {             
            if (dup2(pfd3[1], STDOUT_FILENO) == -1)
		exit(EXIT_FAILURE);
            if (close(pfd3[1]) == -1)
		exit(EXIT_FAILURE);
		}
	
	execlp("tee", "tee", filename, (char *) NULL);
	exit(EXIT_SUCCESS);

    default: 
        break;
    }
    return;
}

void wc(void) {
    switch (fork()) {
    case -1:
	exit(EXIT_FAILURE);

    case 0:            
        if (close(pfd3[1]) == -1)                
	    exit(EXIT_FAILURE);

        if (pfd3[0] != STDIN_FILENO) {             
            if (dup2(pfd3[0], STDIN_FILENO) == -1)
		exit(EXIT_FAILURE);
            if (close(pfd3[0]) == -1)
		exit(EXIT_FAILURE);
        }

	execlp("wc", "wc", (char *) NULL);
	exit(EXIT_SUCCESS);

    default: 
        break;
    }
    return;
}
