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
    //printf("**esmo el lo****\n");
    signal(SIGCHLD,handl);
    key_t shmKey1,prevClkKey;
    shmKey1=ftok("keyfile",65);
    prevClkKey=ftok("keyfile",100);
    int prevClkID=shmget(prevClkKey,sizeof(int),0666);
    if (prevClkID == -1)
    {
        perror("Error in getting the shared memoryzzzzzzzzzzzzzzzzzzz");
        exit(-1);
    }
    int shmid1=shmget(shmKey1,sizeof(int),0666);
    if (shmid1 == -1)
    {
        perror("Error in getting the shared memory");
        exit(-1);
    }
    //printf("**esmo el mo****\n");
    int *shmaddr =(int *) shmat(shmid1, (void *)0, 0);
    int *prev=(int *) shmat(prevClkID, (void *)0, 0);
    *prev=getClk();
    //printf("****esmo el lo\n");
    while ((*shmaddr) > 0)
    {
       //printf("clock from process %d\n",getClk());
       //printf("**prev : %d, curr : %d\n",*prev,getClk());
       fflush(stdout);
       while(*prev==getClk())
       {
           //printf("adham\n");
       }
       while(*prev==getClk())
       {
           //printf("adham\n");
       }
       //printf("prev : %d, curr : %d\n",*prev,getClk());
       *prev=getClk();
       (*shmaddr)--;
       printf("*****remaining time is : %d, pid: %d \n",*shmaddr,getpid());
       fflush(stdout);
     
    }
    printf("ana tl3t mn l process %d\n",getpid());
    shmdt(shmaddr);
    shmdt(prev);
    destroyClk(false);
    return 0;
}