/*
Nicholas Jordan
CS 311
Homework 7 
Threaded portion
----
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <getopt.h>
#include <limits.h>
#include <math.h>
#include <time.h>
#include <pthread.h>


#define GETBIT(x,i) ((x[i/8] & (1<<(i%8)))!=0) 
#define SETBIT(x,i) x[i/8] |= (1<<(i%8)) 
#define CLEARBIT(x,i) x[i/8]&=(1<<(i%8))^0xFF

unsigned char *bitmap;
unsigned char *bitused;
unsigned int MAX_SIZE = UINT_MAX; 


void * sieve_thread(){
    unsigned int i, j;

    for (i = 3; i < sqrt(MAX_SIZE); i+=2){
	if ((!GETBIT(bitmap, i))&&(!GETBIT(bitused, i))){ //not already marked as non-prime and not sieved already
	    SETBIT(bitused, i);//mark this prime as sieved		 		 
	    //	printf("process %u used: %d\n", pthread_self(), i);
	    for (j = i * i; j < MAX_SIZE; j+= i){//mark all multiple of this prime
		SETBIT(bitmap, j);
	    }	
	}	    
    }
    pthread_exit((void*)0);
}


int main (int argc, char *argv[]){
    char c;
    int num_threads = 1;
    int print_flag = 1;
    unsigned int object_size, object_used_size;
    unsigned int i, j;
   
    struct timespec start, finish;
    double elapsed;
    
    while ((c = getopt(argc, argv, "qm:c:")) != -1){
	switch (c){
	case 'q':
	    print_flag = 0;
	    break;
	case 'm':
	    MAX_SIZE = atoi(optarg);
	    break;
	case 'c':
	    num_threads = atoi(optarg);
	    break;
	}
    }
    
    if (MAX_SIZE == UINT_MAX){
	printf("Max value of UINT_MAX is not currently functional.\n");
	exit(-1);
    }

    

    pthread_t thread[num_threads];
  
    object_size = MAX_SIZE / 8;


    //First bit-array to mark non-primes
    bitmap = (unsigned char *) malloc(sizeof(unsigned char) * (object_size));


    //Special case for small sieving list
    if (MAX_SIZE > 63) object_used_size = sqrt(MAX_SIZE)/8+1;
    else object_used_size = 1;
    

    //Second bit-array to mark sieving primes in use
    bitused = (unsigned char *) malloc(sizeof(unsigned char) * (object_used_size));

    clock_gettime(CLOCK_MONOTONIC, &start);


    //Assume 0, 1, and ALL evens 4 plus will not be prime
    SETBIT(bitmap, 0);         
    SETBIT(bitmap, 1);
    for (i = 4; i < MAX_SIZE; i+=2){
	SETBIT(bitmap, i);
    }


    for (i=0;i<num_threads;i++){
	if (0 != pthread_create(&thread[i], NULL, sieve_thread, NULL)){
		perror("Cannot create thread");
		exit(EXIT_FAILURE);
	}
    }

    for (i=0;i<num_threads;i++){
	pthread_join(thread[i], NULL);
    }
  
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    if (print_flag == 1){
	for (j=0;j<MAX_SIZE;j++){
	    if (!GETBIT(bitmap, j)) printf("Prime: %d\n", j);
	}
    }
    else printf("Max value:\t%u\nThreads:\t%d\nElapsed time:\t%f seconds\n", MAX_SIZE, num_threads, elapsed);
    

    //test range
    /* for (i=1000000000;i<1000000300;i++){
	if (!GETBIT(bitmap, i)) printf("Prime: %d\n", i);
	}*/
    
    free(bitmap);
    free(bitused);

    exit(EXIT_SUCCESS);
    return 0;
}

