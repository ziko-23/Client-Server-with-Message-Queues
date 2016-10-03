#include<stdio.h>
#include<string.h>
#include<time.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/wait.h>
#include<sys/errno.h>
     
extern int errno;
#define MSGPERM 0600

int msgqid, rc;
int done;
time_t _ltime;

struct q_type {
  long mtype;
  char mtext[81];
}q_entry;

int main(int argc,char **argv)
{
  int statloc;
  msgqid=msgget(IPC_PRIVATE,MSGPERM|IPC_CREAT|IPC_EXCL);
  /* create message queue, abort if message queue with the same key exists */
  if (msgqid < 0) {
    perror( strerror(errno) );
    printf("msgget failed, msgqid = %d\n", msgqid);
    exit(1);
  }

  printf("message queue %d created\n",msgqid);
  q_entry.mtype=1; /* set the type of message */
  _ltime = time(&_ltime); /* retrieving current time by calling time() */
  sprintf (q_entry.mtext,"%s\n",ctime(&_ltime)); /* setting the right time format by means of ctime() */

  rc = msgsnd(msgqid,&q_entry,80,0);
  /* take IPC_NOWAIT instead of 0, if you want the process not to suspend, 
     if message delivery is not possible at the moment */

  if (rc < 0) {
    perror( strerror(errno) );
    printf("msgsnd failed, rc = %d\n", rc);
    exit(1);
  }

  rc=msgrcv(msgqid,&q_entry,80,0,0);
  if (rc < 0) {
    perror( strerror(errno) );
    printf("msgrcv failed, rc=%d\n", rc);
    exit(1);
  } 
  printf("received msg: %s\n", q_entry.mtext);

  rc=msgctl(msgqid,IPC_RMID,NULL);
  if (rc < 0) {
    perror( strerror(errno) );
    printf("msgctl (return queue) failed, rc=%d\n", rc);
    exit(1);
  }
  printf("message queue %d is gone\n",msgqid);

}