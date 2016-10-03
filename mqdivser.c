/* Autor: Alex Leidwein */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<sys/wait.h>
#include<sys/errno.h>
#include<math.h>
     
extern int errno;
#define MSGPERM 0600

int msgqid_rcv, msgqid_snd, rc;
int done;
time_t _ltime;

struct q_type {
    long mtype;
    int  client_kennung, genauigkeit, dividend, divisor;
}rcv;

struct q_rcv {
    long mtype;
    double  test;
}snd;

int main(int argc,char **argv)
{
    //msgqid=msgget(IPC_PRIVATE,MSGPERM|IPC_CREAT|IPC_EXCL);
    /* create message queue, abort if message queue with the same key exists */
    /*if (msgqid < 0) {
        perror( strerror(errno) );
        printf("msgget failed, msgqid = %d\n", msgqid);
        exit(1);
    }*/
    ///Create
    msgqid_rcv = msgget(1, IPC_PRIVATE | IPC_CREAT | MSGPERM);
    if (msgqid_rcv < 0) {
        perror( strerror(errno) );
        printf("msgget failed, msgqid_rcv = %d\n", msgqid_rcv);
        exit(1);
    }
    printf("message queue %d created\n",msgqid_rcv);

    for(;;) ///Recive
    {
        rc=msgrcv(msgqid_rcv,&rcv,sizeof(rcv) - sizeof(long),0,0);
        if (rc < 0) {
            perror( strerror(errno) );
            printf("msgrcv failed, rc=%d\n", rc);
            exit(1);
        } 

        if(rcv.mtype == 1000) ///Beenden
        {
            rc=msgctl(msgqid_rcv,IPC_RMID,NULL);
            if (rc < 0) {
                perror( strerror(errno) );
                printf("msgctl (return queue) failed, rc=%d\n", rc);
                exit(1);
            }
            printf("message queue %d is gone\n",msgqid_rcv);
            exit(0);
        }
        printf("received msg: %d/%d, %d, %ld\n", rcv.dividend,rcv.divisor,rcv.genauigkeit, rcv.mtype); 
    /***********************************/ //TODO
        ///Open
        msgqid_snd = msgget(rcv.mtype, IPC_PRIVATE | MSGPERM);

        if (msgqid_snd < 0) {
            perror( strerror(errno) );
            printf("msgget failed, msgqid = %d\n", msgqid_snd);
            exit(1);
        }
        printf("message queue %d opened\n",msgqid_snd);

        snd.test = (double)rcv.dividend/(double)rcv.divisor;
        snd.mtype = 1;

        ///Send
        rc = msgsnd(msgqid_snd, &snd, sizeof(snd) - sizeof(long), 0);
        if (rc < 0) {
            perror( strerror(errno) );
            printf("msgsnd failed, rc = %d\n", rc);
            exit(1);
        }
        printf("sent msg:\t%f\n", snd.test); 
        //printf("sent msg: %s\n", q_entry.mtext);       
    }
}

