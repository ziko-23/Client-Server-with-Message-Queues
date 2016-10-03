/* Autor: Alex Leidwein */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/wait.h>
#include<sys/errno.h>
     
extern int errno;
#define MSGPERM 0600

int msgqid_rcv, msgqid_snd, rc;
int done;
time_t _ltime;

struct q_snd {
  long mtype;
  int  client_kennung, genauigkeit, dividend, divisor;
}snd;

struct q_rcv {
  long mtype;
  double test;
}rcv;

int main(int argc,char **argv)
{
    int i;

    if (argc > 1)
    {
      for (i = 1; i < argc; i++)
	    { 
	      switch(argv[i][1])
        {
          case 'g':  //Genauigkeit
            i++;
            snd.genauigkeit = atoi(argv[i]);
            break;
            
          case 'd':  //Divident
            i++;
            snd.dividend = atoi(argv[i]);
            break;

          case 's':  //Divisor
            i++;
            snd.divisor = atoi(argv[i]);
            break;

          case 't':  //Divisor
            i++;
            snd.mtype = atoi(argv[i]);
            break;  
        }
	    }
    }

    ///Open
    msgqid_snd = msgget(1, IPC_PRIVATE | MSGPERM);
    //msgqid=msgget(IPC_PRIVATE,MSGPERM|IPC_CREAT|IPC_EXCL);
    /* create message queue, abort if message queue with the same key exists */
    if (msgqid_snd < 0) {
      perror( strerror(errno) );
      printf("msgget failed, msgqid_snd = %d\n", msgqid_snd);
      exit(1);
    }
    printf("message queue %d opened\n",msgqid_snd);

    if(snd.mtype != 1000)///Create
    {
      msgqid_rcv = msgget(snd.mtype, IPC_PRIVATE | IPC_CREAT | MSGPERM);
        if (msgqid_rcv < 0) {
          perror( strerror(errno) );
          printf("msgget failed, msgqid_rcv = %d\n", msgqid_rcv);
          exit(1);
        }
        printf("message queue %d created\n",msgqid_rcv);
    }

    ///Send
    rc = msgsnd(msgqid_snd, &snd, sizeof(snd) - sizeof(long), 0);
    /* take IPC_NOWAIT instead of 0, if you want the process not to suspend, 
    if message delivery is not possible at the moment */

    if (rc < 0) {
        perror( strerror(errno) );
        printf("msgsnd failed, rc = %d\n", rc);
        exit(1);
    }
    printf("sent msg: %d/%d, %d, %ld\n", snd.dividend,snd.divisor,snd.genauigkeit,snd.mtype); 

/********************************************************************************/
    if(snd.mtype != 1000)
    {
      ///Recive
      rc=msgrcv(msgqid_rcv,&rcv,sizeof(rcv) - sizeof(long),0,0);
      if (rc < 0) {
        perror( strerror(errno) );
        printf("msgrcv failed, rc=%d\n", rc);
        exit(1);
      } 
      printf("received msg:\t%f\n", rcv.test);

      ///Beenden
      rc=msgctl(msgqid_rcv,IPC_RMID,NULL);
      if (rc < 0) {
          perror( strerror(errno) );
          printf("msgctl (return queue) failed, rc=%d\n", rc);
          exit(1);
        }
        printf("message queue %d is gone\n",msgqid_rcv);
    }

    exit(0);
}