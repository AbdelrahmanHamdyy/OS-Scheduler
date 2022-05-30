#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
};
int main(int argc,char ** argv)
{
    key_t key_id;
    int msgq_id, send_val;
    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    struct processData message;
    int rec_val = msgrcv(msgq_id, &message, sizeof(message), 0, IPC_NOWAIT);
    if (rec_val == -1)
    {
        printf("Error in receive\n");
        return 0;
    }
    printf("%d --- %d --- %d --- %d\n",message.id,message.arrivaltime,message.priority,message.runningtime);

    return 0;
}
