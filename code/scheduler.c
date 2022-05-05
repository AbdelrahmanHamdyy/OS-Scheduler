#include "headers.h"

//this struct can be considered as letter 

int shmid1,msgq_id,numOfProcesses,typeAlgo,slot,finishedProcesses=0;
int*remainingTime;
struct PCB *runningProcess;
Node* readyQueue;
FILE * SchedulerLog;
void remainingTimeSharedMemory()
{
    key_t shmKey1;
    shmKey1=ftok("keyfile",65);
    shmid1=shmget(shmKey1,sizeof(int),IPC_CREAT|0666);
    if (shmid1 == -1)
    {
        perror("Error in creating the shared memory");
        exit(-1);
    }
}
void processesMessageQueue()
{
    key_t msgqKey;
    msgqKey = ftok("keyfile", 65);
    msgq_id = msgget(msgqKey, 0444);
    if (msgq_id == -1)
    {
        perror("Error in importing the message queue");
        exit(-1);
    }
}
struct PCB *createProcess()
{

    struct processData message;
    int rec_val = msgrcv(msgq_id, &message, sizeof(message), 0, IPC_NOWAIT);
    if (rec_val == -1)
    {
        //printf("Error in receive\n");
        return NULL;
    }
    else
    {
        struct PCB *newProcess=malloc(sizeof(struct PCB));
        newProcess->arrival=message.arrivaltime;
        newProcess->brust=message.runningtime;
        newProcess->id=message.id;
        newProcess->priority=message.priority;
        newProcess->wait=0;
        newProcess->stop=0;
        //printf("\nMessage received:\n");
        return newProcess;
    }
}
void HPF();
int main(int argc, char * argv[])
{
    initClk();
    SchedulerLog = fopen("SchedulerLog.txt", "w");
    //printf("*************************\nana fel scheduler\n**********************************\n");
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    //shared memory between process and scheduler (remaining time)
    remainingTimeSharedMemory();
    //message queue between process_generator and scheduler (insert processes)
    processesMessageQueue();
    remainingTime=(int*)shmat(shmid1,0,0);
     if (argc<2) {
		printf("Too few arguments. Exiting!\n");
		exit(1);
	}
    numOfProcesses = atoi(argv[0]);
    typeAlgo =  atoi(argv[1]);
    if(typeAlgo==3)
        slot= atoi(argv[2]);
    runningProcess=NULL;
    system("gcc process.c -o process");
    switch (typeAlgo)
    {
        case 1:
            HPF();
            break;

        case 2:
            HPF();
            break;

        case 3:
            HPF();
            break;

        default:
        HPF();
    }
    shmdt(remainingTime);
    fclose(SchedulerLog);
    destroyClk(true);
}

void resumeProcess(struct PCB* p)
{
    kill(p->pid, SIGCONT);
    p->wait += getClk() - p->stop;
    fprintf( SchedulerLog, "At time %d process %d resume arr  %d total %d remain  %d wait %d\n",
                                getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);
    // printf("At time %d process %d resume arr  %d total %d remain  %d wait %d\n",
    //                             getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);

}
void stopProcess(struct  PCB* p)
{
    kill(p->pid, SIGSTOP);
    p->stop = getClk();
    fprintf( SchedulerLog, "At time %d process %d stoped arr  %d total %d remain  %d wait %d\n", 
                               getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);
    // printf( "At time %d process %d stoped arr  %d total %d remain  %d wait %d\n", 
    //                            getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);
}
void finishProcess(struct  PCB* p)
{
    double WTA = (getClk() - p->arrival) * 1.0 / p->brust;
      fprintf( SchedulerLog, "At time %d process %d finished arr  %d total %d remain  %d wait %d  TA %d WTA %.2f\n", 
                               getClk(), p->id, p->arrival, p->brust, p->brust-p->running ,p->wait,getClk() - p->arrival,WTA);
    // printf("At time %d process %d finished arr  %d total %d remain  %d wait %d  TA %d WTA %.2f\n", 
    //                            getClk(), p->id, p->arrival, p->brust, p->brust-p->running ,p->wait,getClk() - p->arrival,WTA);
}

void startProcess(struct  PCB* p)
{
    p->start=getClk();
    p->wait=p->start-p->arrival;
    fprintf( SchedulerLog, "At time %d process %d started arr  %d total %d remain  %d wait %d\n", 
                               getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);
    // printf("At time %d process %d started arr  %d total %d remain  %d wait %d\n", 
    //                            getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);
}

void HPF(){
    //printf("ana gowa el HPF\n");
    while(finishedProcesses<numOfProcesses){
        //sleep(1);
        //printf("yamosahel\n");
        struct PCB *curr=createProcess();
        while(curr)
        {
            //printf("ana keda estalamt element mn el queue\n");
            //printf("\n*************************\n%d --- %d --- %d --- %d\n*************************\n",curr->id,curr->arrival,curr->priority,curr->brust);
            fprintf( SchedulerLog,"At time %d process %d arrived arr  %d total %d remain  %d wait %d\n", 
                               getClk(), curr->id, curr->arrival, curr->brust, curr->brust-curr->running , curr->wait);
            push(&readyQueue,curr,curr->priority);
            curr=createProcess();
        }
        if(!runningProcess&&!isEmpty(&readyQueue))
        {
            runningProcess=peek(&readyQueue);
            pop(&readyQueue);
            *remainingTime=runningProcess->brust;
            startProcess(runningProcess);
            int pid=fork();
            if (pid == -1)
            {
  	            scanf("error in fork in scheduler\n");
                kill(getpgrp(), SIGKILL);
            }
            if(pid==0)
            {
                execl("./process","process", NULL);
            }
        }
        int currClk=getClk();
        if(*remainingTime==0&&runningProcess)
        {
            runningProcess->running=runningProcess->brust;
            finishProcess(runningProcess);
            free(runningProcess);
            runningProcess=NULL;
            finishedProcesses++;
        }
    }
    
}