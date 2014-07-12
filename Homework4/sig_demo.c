/* Nicholas Jordan
CS 311
Homework 4 Part 2
*/
#define _POSIX_SOURCE
#define _BSD_SOURCE

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

//extern void psignal(int sig, const char *msg);
//extern const char *const sys_siglist[];

void sig_usr1(int signal){
  printf("Caught SIGUSR1\n");
  return;
}

void sig_usr2(int signal){
  printf("Caught SIGUSR2\n");
  return;
}

void sig_int(int signal){
  printf("Caught SIGINT, exiting\n");
  exit(EXIT_FAILURE);
  return;
}

int main(int argc, char **argv){
  struct sigaction s;

  s.sa_handler = sig_usr1;
  sigaction(SIGUSR1, &s, NULL);

  s.sa_handler = sig_usr2;
  sigaction(SIGUSR2, &s, NULL);

  s.sa_handler = sig_int;
  sigaction(SIGINT, &s, NULL);

  pid_t pid = getpid();
  kill(pid, SIGUSR1);
  kill(pid, SIGUSR2);
  kill(pid, SIGINT);

  return 0;
}
