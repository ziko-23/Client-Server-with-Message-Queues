#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>

// Define message queue structure
struct msg_buffer
{
    long msg_type;
    int n1;
    int n2;
    char operator[100];
} message;
struct result
{
    long msg_type;

    char rcvd[100];
    int c;
} res;
void remove_spaces(char *s)
{
    char *d = s;
    do
    {
        while (*d == ' ')
        {
            ++d;
        }
    } while (*s++ = *d++);
}
int main()
{
    
    key_t my_key;
    int msg_id;
    my_key = ftok("progfile", 65); // create unique key
    msg_id = msgget(my_key, 0666 | IPC_CREAT);
    printf("server created .\n");
    printf("server waiting for messages ...\n");
    res.msg_type = 1;
    // create message queue and return id
    //    while (msgrcv(msg_id, &message, sizeof(message), 1, 0))
    //    {
    /* code */
    msgrcv(msg_id, &message, sizeof(message), 1, 0); // used to receive message
    // display the message
    //    printf("Received Message is : %s \n", message.operator);
    remove_spaces(message.operator);

    printf("Received message is : %d %s %d\n", message.n1, message.operator, message.n2);
    // printf("%s", message.msg);
    //  printf("Received Message is : %s \n", message.operator);
    //     printf("%d\n", strcmp("*",message.operator));
    // printf("* choisi -> solution est : %s\n",message.operator);
    if (strcmp("*", message.operator) == -10)
    {
        // printf("* choisi -> solution est : %d\n", (message.n1 * message.n2));
        // char *charValue = (message.n1 * message.n2) + '0';

        strcpy(res.rcvd, "no error");
        res.c = (message.n1 * message.n2);
        msgsnd(msg_id, &res, sizeof(res), 0); // send message
        printf("msg send succefully -> : %d\n", res.c);
    }
    else if (strcmp("/", message.operator) == -10)
    {
        if (message.n2 == 0)
        {
            strcpy(res.rcvd, "Error divid by 0");
            res.c = -999;
            msgsnd(msg_id, &res, sizeof(res), 0); // send message
            printf("msg send succefully -> : %d\n", res.c);
            printf("operateur entree invalid !");
            msgctl(msg_id, IPC_RMID, NULL); // destroy the message queue
        }
        printf("/ choisi -> solution est : %d", (message.n1 / message.n2));
        strcpy(res.rcvd, "no error");
        res.c = (message.n1 / message.n2);
        msgsnd(msg_id, &res, sizeof(res), 0); // send message
        printf("msg send succefully -> : %d\n", res.c);
    }
    else if (strcmp("+", message.operator) == -10)
    {
        strcpy(res.rcvd, "no error");
        res.c = (message.n1 + message.n2);
        msgsnd(msg_id, &res, sizeof(res), 0); // send message
        printf("msg send succefully -> : %d\n", res.c);
        printf("+ choisi -> solution est : %d", (message.n1 + message.n2));
    }
    else if (strcmp("-", message.operator) == -10)
    {
        strcpy(res.rcvd, "no error");
        res.c = (message.n1 - message.n2);
        msgsnd(msg_id, &res, sizeof(res), 0); // send message
        printf("msg send succefully -> : %d\n", res.c);
        printf("- choisi ->solution est : %d", (message.n1 - message.n2));
    }
    else
    {
        strcpy(res.rcvd, "Error dump");
        res.c = -1;
        msgsnd(msg_id, &res, sizeof(res), 0); // send message
        printf("msg send succefully -> : %d\n", res.c);
        printf("operateur entree invalid !");
        msgctl(msg_id, IPC_RMID, NULL); // destroy the message queue
    }

    //    }

    msgctl(msg_id, IPC_RMID, NULL); // destroy the message queue
    return 0;
}