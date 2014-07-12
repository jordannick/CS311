/* 
Nicholas Jordan
CS 311 - Homework 4 Part 3 
7/21/2013
myar.c

------
Created with inspiration from the work of:

Dan Albert
https://github.com/DanAlbert/myar/blob/master/myar.c
(ensuring even bytes, sprintf usage, general structure)

Austin Dubina
https://github.com/Mankee/CS311/blob/master/Assignment3/myar.c
(octal/time handling, general structure)

------
*/
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <getopt.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <ar.h>
#include <time.h>
#include <utime.h>
#include <string.h>

#define STR_SIZE sizeof("rwxrwxrwx")
#define FP_SPECIAL 1 

void append(char *, char *);
void append_all(char *);
void append_ops(int, char *, int);
void extract(char *, char *);
void delete(char *, char *);
void print_table(char *);
void print_vtable(char *);
char * file_perm_string(mode_t, int);

int main(int argc, char **argv){
  char c;
  int x; 
  while ((c = getopt(argc, argv, "qxtvdA")) != -1){
    switch (c){
    case 'q':
      for (x=3; x<argc; x++){
	append(argv[2], argv[x]);
      }
      break;
    case 'x':
      extract(argv[2], argv[3]);
      break;
    case 't':
      print_table(argv[2]);
      break;
    case 'v':
      print_vtable(argv[2]);
      break;
    case 'd':
      delete(argv[2], argv[3]);
      break;
    case 'A':
      append_all(argv[2]);
      break;
    }
  }
  return 0;
}

//Decide if need to open or create archive
void append(char *archname, char *filename){
  int archfd;
  if ((archfd = open(archname, O_APPEND | O_RDWR)) != -1){
    append_ops(archfd, filename, 0);
  }
  else if ((archfd = open(archname, O_CREAT | O_APPEND | O_RDWR, 00666)) != -1){
    append_ops(archfd, filename, 1);
  }
  else{
    perror("Could not open or create archive");
    exit(EXIT_FAILURE);
  }
  return;			    
}

//Find all regular files in directory, append
//Modified from code by user "Peter Parker" - http://stackoverflow.com/a/612176
void append_all(char *archname){
  DIR *dir;
  struct dirent *ent;
  struct stat fstat;
  char cwd[1024];

  //open current working directory
  if ((dir = opendir(getcwd(cwd, sizeof(cwd)))) != NULL) {
    while ((ent = readdir (dir)) != NULL) {
      char *fname = ent->d_name;
      stat(fname, &fstat);
      //is it a regular file?
      if(((fstat.st_mode & S_IFMT) == S_IFREG)){
	//don't want to add archive to itself 
	if (strcmp(archname, fname)!=0){
	  printf ("Appending: %s\n", fname);
	  append(archname, fname);
	}	
      }
    }
    closedir (dir);
  } 
  else{
    perror ("Could not open directory");
    exit(EXIT_FAILURE);
  }
  return;
}

//The dirty work to append a file to archive
void append_ops(int archfd, char *filename, int isnew){
  char fname[16];
  struct stat fstat;
  struct ar_hdr artemp;
  int numRead;

  //write ARMAG if needed
  if (isnew){
    write(archfd, ARMAG, SARMAG);
  }

  stat(filename, &fstat);
  strcpy(fname, filename);
  strcat(fname, "/");

  sprintf(artemp.ar_name, "%-16s", fname);
  sprintf(artemp.ar_date, "%-12d",(int) fstat.st_mtime);
  sprintf(artemp.ar_uid, "%-6d", (int) fstat.st_uid);
  sprintf(artemp.ar_gid, "%-6d", (int) fstat.st_gid);
  sprintf(artemp.ar_mode, "%-8o", (unsigned int) fstat.st_mode);
  sprintf(artemp.ar_size, "%-10d", (int) fstat.st_size);
  sprintf(artemp.ar_fmag, ARFMAG);

  //write all header info to archive
  write(archfd, &artemp, 60);

  //open file, write all contents to archive
  int filefd = open(filename, O_RDONLY);
  char contents[fstat.st_size];
  while((numRead = read(filefd, contents, sizeof(contents))) != 0){
    write(archfd, contents, numRead);
  }
  
  //make sure we're at even byte
  if ((fstat.st_size%2)==1){
    write(archfd, "\n", 1);
  }

  if(close(archfd)!=0){
    perror("Error while closing the archive.\n");
    exit(EXIT_FAILURE);
  }
  if(close(filefd)!=0){
    perror("Error while closing the file.\n");
    exit(EXIT_FAILURE);
  }
  return;
}

//Extract a given file from archive
void extract(char *archname, char *filename){
  struct ar_hdr artemp;
  struct utimbuf timestamp;
  int filefd;
  int FMAX;
  char newfile[16];

  // open the archive in read-only mode
  int archfd = open(archname, O_RDONLY); 
  if (archfd == -1){
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }
  // seek past size of ARMAG
  lseek(archfd, SARMAG, SEEK_SET);

  // while not at end and no error, read header info into temp
  while(read(archfd, &artemp, 60) > 0){
    strncpy(newfile, artemp.ar_name, 16);

    //remove trailing slash
    int i;
    for(i=0;i<16;i++){
      if (newfile[i] == '/') newfile[i] = '\0';
    }
    // printf("Do %s and %s match?\n", newfile, filename);
    //find match
    if (strcmp(newfile, filename)==0){
      printf("Extracting: %s.\n", newfile);
      //create file with same permissions
      filefd = open(newfile, O_CREAT | O_WRONLY | O_EXCL, strtol(artemp.ar_mode, NULL, 8));
      //save contents to file
      FMAX = atoi(artemp.ar_size);
      char writebuff[FMAX];
      read(archfd, writebuff, FMAX);
      write(filefd, writebuff, FMAX);
      //save mtime and atime to file
      timestamp.actime = atoi(artemp.ar_date);
      timestamp.modtime = atoi(artemp.ar_date);
      utime(newfile, &timestamp);

      printf("Wrote %s of size %d bytes.\n", newfile, FMAX);
      break;
    }

    lseek(archfd, atoi(artemp.ar_size), SEEK_CUR);

    // make sure we're at even byte
    if ((lseek(archfd, 0, SEEK_CUR) % 2) == 1){
      lseek(archfd, 1, SEEK_CUR);
    }
  }

  if(close(archfd)!=0){
    perror("Error while closing the archive.\n");
    exit(EXIT_FAILURE);
  }
  if(close(filefd)!=0){
    perror("Error while closing the file.\n");
    exit(EXIT_FAILURE);
  }
  return;
}

void delete(char *archname, char *filename){




  return;
}




// Simple table
void print_table(char *archname){
  struct ar_hdr artemp;
  char fname[16];

  // open the archive in read-only mode
  int fd = open(archname, O_RDONLY); 
  if (fd == -1){
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }
  // seek past size of ARMAG
  lseek(fd, SARMAG, SEEK_SET);

  // while not at end and no error, read header info into temp
  while(read(fd, &artemp, 60) > 0){

     strncpy(fname, artemp.ar_name, 16);

    //remove trailing slash
    int i;
    for(i=0;i<16;i++){
      if (fname[i] == '/') fname[i] = '\0';
    }

    printf("%.16s\n", fname);

    // seek past contents to next header data
    lseek(fd, atoi(artemp.ar_size), SEEK_CUR); 

    // make sure we're at even byte
    if ((lseek(fd, 0, SEEK_CUR) % 2) == 1){
      lseek(fd, 1, SEEK_CUR);
    }
  }

  if(close(fd)!=0){
    perror("Error while closing the file.\n");
    exit(EXIT_FAILURE);
  }
  return;
}

// Verbose table
void print_vtable(char *archname){
  struct ar_hdr artemp;
  char *perm;
  int uid, gid, size;
  char date[20];
  char fname[16];

  // open the archive in read-only mode
  int fd = open(archname, O_RDONLY);
  if (fd == -1){
    perror("Error while opening the file.\n");
    exit(EXIT_FAILURE);
  }

  // seek past size of ARMAG
  lseek(fd, SARMAG, SEEK_SET); 

  // while not at end and no error, read header info into temp
  while(read(fd, &artemp, 60) > 0){

    strncpy(fname, artemp.ar_name, 16);

    //remove trailing slash
    int i;
    for(i=0;i<16;i++){
      if (fname[i] == '/') fname[i] = '\0';
    }

    //convert mode from octal
    mode_t convperm = strtol(artemp.ar_mode, NULL, 8);  
    //get permission string
    perm = file_perm_string(convperm, 1); 

    uid = atoi(artemp.ar_uid);
    gid = atoi(artemp.ar_gid);
    size = atoi(artemp.ar_size); 

    //convert seconds to appropriate format
    time_t rawtime = atoi(artemp.ar_date); 
    struct tm *time = localtime(&rawtime); 
    strftime(date, sizeof(date), "%b %e %R %Y", time);

    printf("%s %d/%d %*d %s %.16s\n", perm, uid, gid, 6, size, date, fname);
    lseek(fd, atoi(artemp.ar_size), SEEK_CUR); 

    //make sure we're at even byte
     if ((lseek(fd, 0, SEEK_CUR) % 2) == 1){
       lseek(fd, 1, SEEK_CUR);
     }
  }

  if(close(fd)!=0){
    perror("Error while closing the file.\n");
    exit(EXIT_FAILURE);
  }
  return;
}

//Convert argument to unix permissions string
//From CS311 Homepage examples - file_stat.c
char * file_perm_string(mode_t perm, int flags){
  static char str[STR_SIZE];
  snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c",
	   (perm & S_IRUSR) ? 'r' : '-', (perm & S_IWUSR) ? 'w' : '-',
	   (perm & S_IXUSR) ?
	   (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
	   (((perm & S_ISUID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
	   (perm & S_IRGRP) ? 'r' : '-', (perm & S_IWGRP) ? 'w' : '-',
	   (perm & S_IXGRP) ?
	   (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 's' : 'x') :
	   (((perm & S_ISGID) && (flags & FP_SPECIAL)) ? 'S' : '-'),
	   (perm & S_IROTH) ? 'r' : '-', (perm & S_IWOTH) ? 'w' : '-',
	   (perm & S_IXOTH) ?
	   (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 't' : 'x') :
	   (((perm & S_ISVTX) && (flags & FP_SPECIAL)) ? 'T' : '-'));
  return str;
}
