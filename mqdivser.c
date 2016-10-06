/************************
 * Message Queue Devide *
 * Server               *
 *                      *
 * Autor: Alex Leidwein *
 *        Felix Eilmer  *
 ************************/

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

#define MSGPERM 0600    //Read(0400) | Write(0200) Permission

int main()
{ 
    //Variablendefinitionen  
    int msgqid_rcv, msgqid_snd, rc, i, pot = 1;

    //Recive Message
    struct q_rcv {
        long mtype;
        int  client_kennung, genauigkeit, dividend, divisor;
    }rcv;

    //Send Message
    struct q_snd {
        long mtype;
        double  test;
    }snd;

    //Create Recive
    msgqid_rcv = msgget(1, IPC_CREAT | MSGPERM);    //Message Queue mit Key=1 erstellen
    if (msgqid_rcv < 0) {   //Erstellen Fehlgeschlagen
        perror( strerror(errno) );
        printf("[S] msgget failed, msgqid_rcv = %d\n", msgqid_rcv);
        exit(1);
    }
    printf("[S] message queue %d created\n",msgqid_rcv);

    for(;;) //Endlosschleife
    {
        //Recive
        rc=msgrcv(msgqid_rcv,&rcv,sizeof(rcv) - sizeof(long),0,0);  //Empfange Message
        if (rc < 0) {   //Empfangen fehlgeschlagen
            perror( strerror(errno) );
            printf("[S] msgrcv failed, rc=%d\n", rc);
            exit(1);
        }
        printf("[S] received msg: %ld: %d/%d, %d\n", rcv.mtype, rcv.dividend,rcv.divisor,rcv.genauigkeit); 

        //Beenden Recive
        if(rcv.mtype == 1000)   //Wenn Type der empfangenen Nachricht = 1000 -> Beende Server
        {
            rc=msgctl(msgqid_rcv,IPC_RMID,NULL);    //Message Queue mit Key=1 Schließen
            if (rc < 0) {   //Schließen Fehlgeschlagen
                perror( strerror(errno) );
                printf("[S] msgctl (return queue) failed, rc=%d\n", rc);
                exit(1);
            }
            printf("[S] message queue %d is gone\n",msgqid_rcv);

            break;  //Springe aus Endlosschleife
        }
        
        //Open Send
        msgqid_snd = msgget(rcv.mtype, MSGPERM);    //Öffne Message Queue mit Key=Type
        if (msgqid_snd < 0) {   //Öffnen Fehlgeschlagen
            perror( strerror(errno) );
            printf("[S] msgget failed, msgqid = %d\n", msgqid_snd);
            exit(1);
        }
        printf("[S] message queue %d opened\n",msgqid_snd);


        snd.mtype = 1;  //Type Festlegen

        //Berechenen	
        snd.test = (double)rcv.dividend/(double)rcv.divisor;    //Division

	    pot = 1;	

	    for(i=0;i<rcv.genauigkeit;i++) {
		    pot *= 10;
	    }
	    snd.test = (int)(snd.test*pot);   //"Schiebe" Werte mit 10^(genauigkeit) nach links. Typecast int um Kommastellen abzuschneiden
	    snd.test /= pot;    //Dividiere durch 10^(genauigkeit) um Originalwert mit Eingestellter Genauigkeit herzustellen

        //Send
        rc = msgsnd(msgqid_snd, &snd, sizeof(snd) - sizeof(long), 0);   //Sende berechneten Wert
        if (rc < 0) {   //Senden Fehlgeschlagen
            perror( strerror(errno) );
            printf("[S] msgsnd failed, rc = %d\n", rc);
            exit(1);
        }
        printf("[S] sent msg:     %f\n", snd.test); 
    }

    printf("[S] Exit\n");
    exit(0);
}

