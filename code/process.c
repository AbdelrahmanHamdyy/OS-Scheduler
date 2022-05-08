#include "headers.h"
void handl(int signum)
{
    destroyClk(false);
    raise(SIGKILL);
}
/* Modify this file as needed*/
int main(int agrc, char * argv[])
{
    initClk();
    //printf("ana fel process\n");
    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    signal(SIGCHLD,handl);
    key_t shmKey1;
    shmKey1=ftok("keyfile",65);
    int shmid1=shmget(shmKey1,sizeof(int),0666);
    if (shmid1 == -1)
    {
        perror("Error in getting the shared memory");
        exit(-1);
    }
    int *shmaddr =(int *) shmat(shmid1, (void *)0, 0);
    int prev=getClk();
    while ((*shmaddr) > 0)
    {
        printf("yarab\n");
       int curr=getClk();
       while(prev==curr)
       {
       curr=getClk();
       }
       prev=curr;
       (*shmaddr)--;
       printf("remaining time is : %d \n",*shmaddr);
     
    }
    //printf("ana tl3t mn l process\n");
    destroyClk(false);
    return 0;
}


