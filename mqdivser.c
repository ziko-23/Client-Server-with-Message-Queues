/* Autor: Alex Leidwein */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <math.h>
     
extern int errno;
#define MSGPERM 0600

int main(int argc,char **argv)
{ 
    //Variablendefinitionen  
    int msgqid_rcv, msgqid_snd, rc;

    struct q_rcv {
        long mtype;
        int  client_kennung, genauigkeit, dividend, divisor;
    }rcv;

    struct q_snd {
        long mtype;
        double  test;
    }snd;

    //Create Recive
    msgqid_rcv = msgget(1, IPC_CREAT | MSGPERM);
    if (msgqid_rcv < 0) {
        perror( strerror(errno) );
        printf("[S] msgget failed, msgqid_rcv = %d\n", msgqid_rcv);
        exit(1);
    }
    printf("[S] message queue %d created\n",msgqid_rcv);

    for(;;)
    {
        //Recive
        rc=msgrcv(msgqid_rcv,&rcv,sizeof(rcv) - sizeof(long),0,0);
        if (rc < 0) {
            perror( strerror(errno) );
            printf("[S] msgrcv failed, rc=%d\n", rc);
            exit(1);
        }
        printf("[S] received msg: %d/%d, %d, %ld\n", rcv.dividend,rcv.divisor,rcv.genauigkeit, rcv.mtype); 

        //Beenden Recive
        if(rcv.mtype == 1000)
        {
            rc=msgctl(msgqid_rcv,IPC_RMID,NULL);
            if (rc < 0) {
                perror( strerror(errno) );
                printf("[S] msgctl (return queue) failed, rc=%d\n", rc);
                exit(1);
            }
            printf("[S] message queue %d is gone\n",msgqid_rcv);

            break;
        }
        
        //Open Send
        msgqid_snd = msgget(rcv.mtype, MSGPERM);

        if (msgqid_snd < 0) {
            perror( strerror(errno) );
            printf("[S] msgget failed, msgqid = %d\n", msgqid_snd);
            exit(1);
        }
        printf("[S] message queue %d opened\n",msgqid_snd);

        //Berechenen
        snd.test = (double)rcv.dividend/(double)rcv.divisor;
        snd.mtype = 1;

        //Send
        rc = msgsnd(msgqid_snd, &snd, sizeof(snd) - sizeof(long), 0);
        if (rc < 0) {
            perror( strerror(errno) );
            printf("[S] msgsnd failed, rc = %d\n", rc);
            exit(1);
        }
        printf("[S] sent msg:\t%f\n", snd.test); 
        //printf("sent msg: %s\n", q_entry.mtext);       
    }

    printf("[S] Exit\n");
    exit(0);
}

