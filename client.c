#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
// structure for message queue
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
    my_key = ftok("progfile", 65);             // create unique key
    msg_id = msgget(my_key, 0666 | IPC_CREAT); // create message queue and return id
    message.msg_type = 1;
    // while (1)
    // {
    char *str;
#define VAL 10
    char nf[VAL] = {0};
    char ns[VAL] = {0};
    char *oper = malloc(1);
    printf("Write n1 : ");
    fgets(nf, 100, stdin);
    message.n1 = atoi(nf);
    printf("Write n2 : ");
    fgets(ns, 100, stdin);
    message.n2 = atoi(ns);
    printf("Write operator : ");
    fgets(oper, 100, stdin);
    strcpy(message.operator, oper);
    res.msg_type = 1;
    //  = oper;
    msgsnd(msg_id, &message, sizeof(message), 0); // send message
    remove_spaces(message.operator);
    // printf("Sent message is : %d %s %d \n", message.n1, message.operator, message.n2);
    msgrcv(msg_id, &res, sizeof(res), 1, 0);
    printf("received from server : %d \nwhith msg : %s\n", res.c, res.rcvd);
   
}