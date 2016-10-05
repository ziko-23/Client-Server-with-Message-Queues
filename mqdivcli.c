/* Autor: Alex Leidwein */

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
#define MSGPERM 0666

int main(int argc,char **argv)
{
    //Variablendefinitionen
    int msgqid_rcv, msgqid_snd, rc, i;
    pid_t pid;

    struct q_snd {
      long mtype;
      int  client_kennung, genauigkeit, dividend, divisor;
    }snd;

    struct q_rcv {
      long mtype;
      double test;
    }rcv;

    //Argumente
    if (argc > 1)
    {
      for (i = 1; i < argc; i++)
	    { 
        if (argv[i][0] == '-' & argv[i][2] == '\0')
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

            case 't':  //Type
              i++;
              snd.mtype = atoi(argv[i]);
              break;  
    } } } }

    //Open Send
    msgqid_snd = msgget(1, MSGPERM);
    if (msgqid_snd < 0) 
    {
      printf("[C] msgget failed, msgqid_snd = %d\n", msgqid_snd);

      //Starte server wenn mque nicht erreichbar (NOT WORKING)
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
        if (msgqid_snd < 0) {
          perror( strerror(errno) );
          printf("[C] msgget failed, msgqid_snd = %d\n", msgqid_snd);
          exit(1);
        }
        break;

      default: //Child
          printf("[C] starting server with PID = %d", getpid());
          execl("./mqdivser.out", "./mqdivser.out", NULL);
          exit(1);

      }
    }
    printf("[C] message queue %d opened\n",msgqid_snd);

    //Create Recive
    if(snd.mtype != 1000)
    {
      msgqid_rcv = msgget(snd.mtype, IPC_CREAT | MSGPERM);
      if (msgqid_rcv < 0) {
        perror( strerror(errno) );
        printf("[C] msgget failed, msgqid_rcv = %d\n", msgqid_rcv);
        exit(1);
      }
      printf("[C] message queue %d created\n",msgqid_rcv);
    }

    //Send
    rc = msgsnd(msgqid_snd, &snd, sizeof(snd) - sizeof(long), 0);
    if (rc < 0) {
      perror( strerror(errno) );
      printf("[C] msgsnd failed, rc = %d\n", rc);
      exit(1);
    }
    printf("[C] sent msg: %d/%d, %d, %ld\n", snd.dividend,snd.divisor,snd.genauigkeit,snd.mtype); 


    if(snd.mtype != 1000)
    {
      //Recive
      rc=msgrcv(msgqid_rcv,&rcv,sizeof(rcv) - sizeof(long),0,0);
      if (rc < 0) {
        perror( strerror(errno) );
        printf("[C] msgrcv failed, rc=%d\n", rc);
        exit(1);
      } 
      printf("[C] received msg:\t%f\n", rcv.test);

      //Beenden Recive
      rc=msgctl(msgqid_rcv,IPC_RMID,NULL);
      if (rc < 0) {
        perror( strerror(errno) );
        printf("[C] msgctl (return queue) failed, rc=%d\n", rc);
        exit(1);
      }
      printf("[C] message queue %d is gone\n",msgqid_rcv);
    }

    printf("[C] Exit\n");
    exit(0);
}