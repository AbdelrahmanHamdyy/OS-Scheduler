#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "headers.h"


int main()
{
    key_t key_id;
    int msgq_id, send_val;

    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);

    if (msgq_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    printf("Message Queue ID = %d\n", msgq_id);

    struct processData *processes = malloc(sizeof(struct processData)); // Array of processes
    processes[0].id = 16666;
    processes[0].arrivaltime = 2;
    processes[0].runningtime = 3;
    processes[0].priority = 4;

    send_val = msgsnd(msgq_id, &processes[0], sizeof(struct processData), !IPC_NOWAIT);

    if (send_val == -1)
        perror("Errror in send");

    return 0;
}