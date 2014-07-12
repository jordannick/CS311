/*
Nicholas Jordan
CS 311
Homework 6
----
Notes/bugs

Maximum # child processes is 20, otherwise bad things.

Parser: 
Works.

Sorters: 
Works.

Combiner:
Semi-works.
-Depending on # procs, may still have word duplicate issues with large input files.
-Count for first word may be off.
-Was hanging near end with more than 2 procs in combiner dup check, solved (for now) with IPC_NOWAIT in msgrcv.
----

Binary tree functions adapted from http://www.thegeekstuff.com/2013/02/c-binary-tree/

*/

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>
#include <string.h>
#include <search.h>
#include <ctype.h>


struct mbuf {
    long mtype;
    char mtext[50]; //assume words no longer than 50
};

struct bin_tree {
    char data[50];
    int wcount;
    struct bin_tree * right, * left;
};
typedef struct bin_tree node;

struct word {
    char data[50];
    int wcount;
};

void parser(int, int, int*);
void fork_child(int, int, int);
void sort_process(int, int, int);
void insert(node **, char *);
void send_inorder(node *, int);
void combiner(int, int *);

int main(int argc, char *argv[]) {
    int num_procs;
    int c, i, j, msqid;
    int proc_done=0;
    int child_msqids[20];

    if ((c=getopt(argc, argv, "n:")) != -1) {
        if (c == 'n') { 
	    num_procs = atoi(optarg);
	    if (num_procs > 20) {
		//printf("Number of processes should not exceed 20. Setting to 20.\n");
		num_procs = 20;
	    } 
	}
	else num_procs = 1;
	}
    else num_procs = 1;
    

    //Create parser-to-sorters msg queue
    msqid = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666);

    //Create sorters-to-combiner msg queues and fork the sorter children
    for (i=1;i<num_procs+1;i++) { 
	child_msqids[i] = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666);

        fork_child(msqid, child_msqids[i], i);
    }

    //Parse while children running
    parser(msqid, num_procs, child_msqids);

    //Combine while children running
    combiner(num_procs, child_msqids);

    //Reap the children
    for (j=1;j<num_procs+1;j++){ 
	if (wait(NULL) != -1) proc_done++;
    }

    // printf("Number of processes done: %d\n", proc_done);
 
    //Combine after the children are done -- nope doesn't work
    //  combiner(num_procs, child_msqids);
 
    //Destroy the parser-to-sorters message queue
     if (proc_done == num_procs){
	 msgctl(msqid, IPC_RMID, NULL);
     }

     exit(EXIT_SUCCESS);
     return 0;
}

//Read input line by line, tokenize words. Send to msg queue.
void parser(int msqid, int num_procs, int * child_msqids) {

    struct mbuf msg;
    int proc_dstr = 1;
    char buf[200];
    int i;
    char * token;
    const char *delim = " `_+\'\'\'/'<>[]{}:\"';.,?-=~!@#$%^&*()1234567890\n\0\t||";
    
    while (fgets(buf, sizeof(buf), stdin) != NULL){ 
	if (strlen(buf)>2) {//skip empty line
	    if (proc_dstr > num_procs) proc_dstr = 1;
	    token = strtok(buf, delim);
	    while (token) {
		for(i = 0; token[i]; i++){
		    token[i] = tolower(token[i]);
		}
		msg.mtype = proc_dstr; //type matches our distribution count
		strcpy(msg.mtext, token);      
		msgsnd(msqid, &msg, strlen(msg.mtext)+1, 0);
		token = strtok(NULL, delim);
	    }
	    proc_dstr++; 
	}
    }

    //send terminator value
    for (i=1; i<num_procs+1;i++){
	msg.mtype = i; 
	strcpy(msg.mtext, "END_READ"); 
	msgsnd(msqid, &msg, strlen(msg.mtext)+1, 0);
    }
}


void fork_child(int msqid, int child_msqid, int myprocnum) { 
    switch (fork()) {
    case -1:
	exit(EXIT_FAILURE);
    case 0: //child
	sort_process(msqid, child_msqid, myprocnum);
	//	printf("Process %d completed\n", myprocnum); 
	exit(EXIT_SUCCESS);
        break;
    default: //continue parent
        break;
    }
}

//Recieve words, insert to tree
void sort_process(int msqid, int child_msqid, int myprocnum){
    struct mbuf msg;
    node *root;
    root = NULL;
  
    //initial read
    msgrcv(msqid, &msg, 50, myprocnum, 0);
  
    while (strcmp(msg.mtext, "END_READ") != 0) {
	insert(&root, msg.mtext);
	msgrcv(msqid, &msg, 50, myprocnum, 0);
    }

    send_inorder(root, child_msqid);

    //send terminator value
    msg.mtype = myprocnum;
    strcpy(msg.mtext, "END_READ"); 
    msgsnd(child_msqid, &msg, strlen(msg.mtext)+1, 0);
}

//Insert values to tree in order
void insert(node ** tree, char * val) {
    node *temp = NULL;
     
    if(!(*tree)) {
	temp = (node *)malloc(sizeof(node));
        temp->left = temp->right = NULL;
	strcpy(temp->data, val);
	temp->wcount = 1;
        *tree = temp;
	return;
    }
    if ((strcmp(val, (*tree)->data)) < 0) {
        insert(&(*tree)->left, val);
    }

    else if ((strcmp(val, (*tree)->data)) > 0) {
        insert(&(*tree)->right, val);

    }
     else if (strcmp(val, (*tree)->data) == 0) {
	(*tree)->wcount++;
     }
}

//Traverse tree in order, send values to assigned child msg queue
void send_inorder(node * tree, int child_msqid) {
    struct mbuf msg;
    if (tree) {
        send_inorder(tree->left, child_msqid);
	msg.mtype=tree->wcount;
	strcpy(msg.mtext, tree->data); 
	msgsnd(child_msqid, &msg, strlen(msg.mtext)+1, 0);
        send_inorder(tree->right, child_msqid);
    }
}

//Merge words + counts, eliminate duplicates, output
void combiner(int num_procs, int * child_msqids){
    struct word words[21];
    struct mbuf newmsg;
    int i, j;
    char * small;
    int queue_done=0;
    int getnext;
    int smallcount;

    //initial fill
    for (i = 1;i<num_procs+1;i++){
	msgrcv(child_msqids[i], &newmsg, 50, 0, 0);
	strcpy(words[i].data, newmsg.mtext);
	words[i].wcount = newmsg.mtype;
    }
    
    while (queue_done < num_procs){
    	
	//dup eliminator
	for (i = 1; i < num_procs+1; i++) {
	    for (j = i + 1; j < num_procs+1; j++) {
		if (strcmp(words[i].data, words[j].data)==0) {

		    words[i].wcount+=words[j].wcount;
		    if(msgrcv(child_msqids[j], &newmsg, 50, 0, IPC_NOWAIT)!=-1) {
		    // msgrcv(child_msqids[j], &newmsg, 50, 0, 0);
			strcpy(words[j].data, newmsg.mtext);
			words[j].wcount = newmsg.mtype;
			 }
			 else {
			     	break;
			//	printf("rcv error 1!");
			 }
		    if  (strcmp(newmsg.mtext, "END_READ") == 0) {
			queue_done++;
		    }
		  
		}
	    }
	}
    
	//get first in array that is not end of a queue
	for (i = 1; i<num_procs+1;i++){
	    if (strcmp(words[i].data, "END_READ")!=0) {
		small = words[i].data;
		smallcount = words[i].wcount;
		getnext = i;
		break;
	    }
	}

	//is it the smallest?
	for (i = 1;i<num_procs+1;i++){	
	     if ((strcmp(words[i].data, small)<0)&&(strcmp(words[i].data, "END_READ")!=0)){ 
		 small = words[i].data;
		 smallcount = words[i].wcount;
		 getnext = i;
	     }
	}

	printf("%*d %s\n", 7, smallcount, small);//final output

	if(msgrcv(child_msqids[getnext], &newmsg, 50, 0, 0)!=-1){
	    strcpy(words[getnext].data, newmsg.mtext); 
	    words[getnext].wcount = newmsg.mtype;
	}
	else {
	    printf("rcv error 2!");
	}
       	
	if  (strcmp(newmsg.mtext, "END_READ") == 0){
	    queue_done++;  
	}
       

    } //Number of queues that reached end is now >= number of processes
    
    // destroy queues of children
    for (i = 1;i<num_procs+1;i++){
	msgctl(child_msqids[i], IPC_RMID, NULL);
    }

}
