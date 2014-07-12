/*
Nicholas Jordan
CS 311
Assignment 3
*/

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

void file_ops(char *,int,int,int, int);

int main(int argc, char **argv){
  char c;
  char *filename = NULL;
  int len=0;
  int offset1=0;
  int offset2=0;
  int elen=0;

  while ((c = getopt (argc, argv, "vf:l:o:O:e:")) != -1){
    switch (c){
    case 'v':
      printf("Usage:\n -f [filename]\n -l [length]\n -o [offset 1]\n -O [offset 2]\n -e [length from end of file]\n \n");
      break;
    case 'f':
      filename = optarg;
      break;
    case 'l':
      len = atoi(optarg);
      break;
    case 'o':
      offset1 = atoi(optarg);
      break;
    case 'O':
      offset2 = atoi(optarg);
      break;
    case 'e':
      elen = atoi(optarg);
      break;
    }
  }

  if (filename != NULL){
    if (len > 0){
      if (offset1 >= 0){
	file_ops(filename, len, offset1, offset2, elen);
      }
      else{
	printf("Must input first offset greater than or equal to zero.\n");
      }
    }
    else{
      printf("Must input length  greater than zero.\n");
    }
  }
  else{
    printf("Must input filename.\n");
  }


  return 0;
}


void file_ops(char *filename, int len, int offset1, int offset2, int elen){

 char buffer[len];

 //open file in read-only mode
 int fd = open(filename, O_RDONLY); 
 if (fd == -1){
   perror("Error while opening the file.\n");
   exit(-1);
 }

 //set first offset from beginning of file
 lseek(fd, offset1, SEEK_SET); 

 //read into buffer len number of characters
 int numRead = read(fd, buffer, len); 
 if (numRead == -1){
   perror("Error reading the file.\n");
   exit(-1);
 }

 //write to output the contents of buffer
 printf("<offset1>------------------------------\n");
 int numWritten = write(STDOUT_FILENO, buffer, numRead);
 if (numWritten == -1 || numWritten != numRead){
   perror("Error writing the output.\n");
   exit(-1);
   }

 //now set second offset from current position
 lseek(fd, offset2, SEEK_CUR);

 //read into buffer len number characters again
 numRead = read(fd, buffer, len);
 if (numRead == -1){
   perror("Error reading the file.\n");
   exit(-1);
 }
 //write to terminal output new contents of buffer
 printf("\n<offset2>------------------------------\n");
 numWritten = write(STDOUT_FILENO, buffer, numRead);
 if (numWritten == -1 || numWritten != numRead){
   perror("Error writing the output.\n");
   exit(-1);
   }


 if (elen > 0){
   char ebuffer[elen];
   //now set offset 'elen' from end of file
   lseek(fd, -elen, SEEK_END);

   //read into ebuffer elen number characters
   numRead = read(fd, ebuffer, elen);
   if (numRead == -1){
     perror("Error reading the file.\n");
     exit(-1);
   }

   //write to terminal output contents of ebuffer
   printf("\n<end bytes>----------------------------\n");
   numWritten = write(STDOUT_FILENO, ebuffer, numRead);
   if (numWritten == -1 || numWritten != numRead){
     perror("Error writing the output.\n");
     exit(-1);
   }
 }
 

 printf("\n");

 //close the file
 if(close(fd)!=0){
   perror("Error while closing the file.\n");
   exit(-1);
 }

}
