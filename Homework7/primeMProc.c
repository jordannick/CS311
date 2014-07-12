/*
Nicholas Jordan
CS 311
Homework 7 
Process portion
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


#define GETBIT(x,i) ((x[i/8] & (1<<(i%8)))!=0) 
#define SETBIT(x,i) x[i/8] |= (1<<(i%8)) 
#define CLEARBIT(x,i) x[i/8]&=(1<<(i%8))^0xFF

unsigned char *bitmap;
unsigned char *bitused;
unsigned int MAX_SIZE = UINT_MAX;


void *mount_shmem(char *path, int object_size){
	int shmem_fd;
	void *addr;

	shmem_fd = shm_open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
	if (shmem_fd == -1){
	    fprintf(stdout, "failed to open shared memory object\n");
	    exit(EXIT_FAILURE);
	}

	if (ftruncate(shmem_fd, object_size) == -1){
		fprintf(stdout, "failed to resize shared memory object\n");
		exit(EXIT_FAILURE);
	}

	addr = mmap(NULL, object_size, PROT_READ | PROT_WRITE, MAP_SHARED, shmem_fd, 0);
	if (addr == MAP_FAILED){
		fprintf(stdout, "failed to map shared memory object\n");
		exit(EXIT_FAILURE);
	}

	return addr;
}


void sieve_process(){
    unsigned int i, j;
    for (i = 2; i < sqrt(MAX_SIZE); i++){
	if (!GETBIT(bitmap, i)){ //If not already marked as non-prime
	    if (!GETBIT(bitused, i)){//If not already marked as used
	    	SETBIT(bitused, i);
		for (j = i * i; j < MAX_SIZE; j+= i){
		    SETBIT(bitmap, j);
		}
	    }
	}	    
    }	  
}

void fork_child(){ 
    switch (fork()){
    case -1:
	exit(EXIT_FAILURE);
    case 0: //child
	sieve_process();
	exit(EXIT_SUCCESS);
    default: //continue parent
        break;
    }
}

int main (int argc, char *argv[]){
    char c;
    int num_procs = 1;
    int print_flag = 1;
    int object_size, object_used_size;
    unsigned int i;
   
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
	    num_procs = atoi(optarg);
	    break;
	}
    }

    if (MAX_SIZE == UINT_MAX){
	printf("Max value of UINT_MAX is not currently functional.\n");
	exit(-1);
    }

    object_size = MAX_SIZE / 8;

    //First bit-array to mark non-primes
    void *addr = mount_shmem("/primelist", object_size);
    bitmap = addr;
    
    //Special case for small sieving list
    if (MAX_SIZE > 63) object_used_size = sqrt(MAX_SIZE)/8+1;
    else object_used_size = 1;
    
    //Second bit-array to mark sieving primes in use
    void *addr_used = mount_shmem("/prime_used", object_used_size);
    bitused = addr_used;

    clock_gettime(CLOCK_MONOTONIC, &start);

    //Assume 0, 1 will not be prime
    SETBIT(bitmap, 0);         
    SETBIT(bitmap, 1);

    for (i=0;i<num_procs;i++){
	fork_child(i);
    }

    for (i=0;i<num_procs;i++){
	wait(NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

    //print all output
    if (print_flag == 1){
	for (i=0;i<MAX_SIZE;i++){
	    if (!GETBIT(bitmap, i)) printf("Prime: %d\n", i);
	}
    }
    else printf("Max value:\t%u\nProcesses:\t%d\nElapsed time:\t%f seconds\n", MAX_SIZE, num_procs, elapsed);

    //test range
    /*  for (i=1000000000;i<1000000300;i++){
	if (!GETBIT(bitmap, i)) printf("Prime: %d\n", i);
	}*/
    
    munmap(addr, object_size);
    munmap(addr_used, object_used_size);
    shm_unlink("/primelist");
    shm_unlink("/prime_used");

    exit(EXIT_SUCCESS);

    return 0;
}

