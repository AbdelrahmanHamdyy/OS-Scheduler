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
    signal(SIGCHLD,handl);
    key_t shmKey1,prevClkKey;
    shmKey1=ftok("keyfile",65);
    prevClkKey=ftok("keyfile",100);
    int prevClkID=shmget(prevClkKey,sizeof(int),0666);
    if (prevClkID == -1)
    {
        perror("Error in getting the shared memory");
        exit(-1);
    }
    //remaining time shared memory
    int shmid1=shmget(shmKey1,sizeof(int),0666);
    if (shmid1 == -1)
    {
        perror("Error in getting the shared memory");
        exit(-1);
    }
    int *shmaddr =(int *) shmat(shmid1, (void *)0, 0);
    int *prev=(int *) shmat(prevClkID, (void *)0, 0);
    *prev=getClk();

    while ((*shmaddr) > 0)
    {
       fflush(stdout);
       while(*prev==getClk())
       {
           //to wait one clock cycle
       }
       while(*prev==getClk())
       {
           //to wait one cycle after continue running
       }
       *prev=getClk();
       (*shmaddr)--;
       kill(SIGUSR1, getppid());
       printf("Remaining time: %d pid: %d\n", *shmaddr, getpid());
     
    }
    shmdt(shmaddr);
    shmdt(prev);
    destroyClk(false);
    return 0;
}
