/************************
 * Message Queue Devide *
 * Client               *
 *                      *
 * Autor: Alex Leidwein *
 ************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/errno.h>
     
extern int errno;

#define MSGPERM 0600  //Read(0400) | Write(0200) Permission

int main(int argc,char **argv)
{
    //Variablendefinitionen
    int msgqid_rcv, msgqid_snd, rc, i;
    pid_t pid;

    //Send Message
    struct q_snd { 
      long mtype;
      int  client_kennung, genauigkeit, dividend, divisor;
    }snd;

    //Recive Message
    struct q_rcv {
      long mtype;
      double test;
    }rcv;

    //Standardwerte
    snd.genauigkeit = 0;
    snd.dividend = 0;
    snd.divisor = 1;
    snd.mtype = 0;

    //Argumente
    if (argc > 1) //Überprüfen ob Argumente übergeben wurden
    {
      for (i = 1; i < argc; i++)
	    { 
        if (argv[i][0] == '-' & argv[i][2] == '\0')
        {
          switch(argv[i][1])  //Argumente in Variabln Speichern
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

            case 't':  //Type
              i++;
              snd.mtype = atoi(argv[i]);
              if(snd.mtype <= 1)
              {
                printf("[C] invaild argument -t\n");
                exit(1);
              }
              break;  
          } 
        }
      }
    }
    if(snd.mtype == 0)
    {
      printf("[C] argument -t is required\n");
      exit(1);
    }

    //Open Send
    msgqid_snd = msgget(1, MSGPERM);  //Message Queue mit Key=1 öffnen
    if (msgqid_snd < 0) //Starte server wenn mque nicht erreichbar
    {
      printf("[C] msgget failed, msgqid_snd = %d\n", msgqid_snd);

      pid = fork();

      switch(pid)
      {
      case -1: //Error
        perror( strerror(errno) );
        printf("[C] fork failed, pid = %d\n", pid);
        exit(1);

      case 0: //Parent
      
        sleep(1);

        printf("[C] retry to open message queue\n");
        msgqid_snd = msgget(1, MSGPERM);
        if (msgqid_snd < 0) { //Öffnen erneut Fehlgeschlagen
          perror( strerror(errno) );
          printf("[C] msgget failed, msgqid_snd = %d\n", msgqid_snd); //Message Queue erneut öffnen
          exit(1);
        }
        break;

      default: //Child
          printf("[C] starting server with PID = %d", getpid());
          execl("./mqdivser.out", "./mqdivser.out", NULL);  //Server Starten
          exit(1);

      }
    }
    printf("[C] message queue %d opened\n",msgqid_snd); //Öffnen Erfolgreich

    //Create Recive
    if(snd.mtype != 1000) //Erstelle keine Message Queue wenn server geschlossen wird
    {
      msgqid_rcv = msgget(snd.mtype, IPC_CREAT | MSGPERM);  //Erstelle Message Queue mit Key=Type 
      if (msgqid_rcv < 0) { //Erstellen fehlgeschlagen
        perror( strerror(errno) );
        printf("[C] msgget failed, msgqid_rcv = %d\n", msgqid_rcv);
        exit(1);
      }
      printf("[C] message queue %d created\n",msgqid_rcv);
    }

    //Send
    rc = msgsnd(msgqid_snd, &snd, sizeof(snd) - sizeof(long), 0); //Sende Message
    if (rc < 0) { //Senden Fehlgeschlagen
      perror( strerror(errno) );
      printf("[C] msgsnd failed, rc = %d\n", rc);
      exit(1);
    }
    printf("[C] sent msg:     %ld: %d/%d, %d\n", snd.mtype, snd.dividend, snd.divisor, snd.genauigkeit); 


    if(snd.mtype != 1000) //Warte nicht auf Message wenn server geschlossen wird
    {
      //Recive
      rc=msgrcv(msgqid_rcv,&rcv,sizeof(rcv) - sizeof(long),0,0);  //Warte Auf Message von Server
      if (rc < 0) { //Empfangen fehlgeschlagen
        perror( strerror(errno) );
        printf("[C] msgrcv failed, rc=%d\n", rc);
        exit(1);
      } 
      printf("[C] received msg: %f\n", rcv.test);

      //Beenden Recive
      rc=msgctl(msgqid_rcv,IPC_RMID,NULL);  //Message Queue Beenden
      if (rc < 0) { //Beenden Fehlgeschlagen
        perror( strerror(errno) );
        printf("[C] msgctl (return queue) failed, rc=%d\n", rc);
        exit(1);
      }
      printf("[C] message queue %d is gone\n",msgqid_rcv);
    }

    printf("[C] Exit\n");
    exit(0);
}