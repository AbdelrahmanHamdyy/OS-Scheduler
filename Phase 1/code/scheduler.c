#include "headers.h"

int shmid1,arrivalsshID,msgq_id,prevClkID,numOfProcesses,typeAlgo,slot,finishedProcesses=0,totalbrust=0,finalclk=0;
int*remainingTime;
int*arrivals;
int idx=0;
double TotalWaitingTime=0,TotalWTA=0;
double*WTA_Arr;
struct PCB *runningProcess,*temporary;
Node* readyQueue,*Stopping_Resuming_Queue;
FILE * SchedulerLog,*schedulerperf;
struct Queue* queue;
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
void numberOfProcessarrived()
{
    key_t shmKey1;
    shmKey1=ftok("keyfile",120);
    arrivalsshID=shmget(shmKey1,sizeof(int),0666);
    if (arrivalsshID == -1)
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

void prevClkSharedMemory()
{
    key_t prevClkKey;
    prevClkKey=ftok("keyfile",100);
    prevClkID=shmget(prevClkKey,sizeof(int),IPC_CREAT|0666);
    if (prevClkID == -1)
    {
        perror("Error in creating the shared memoy");
        exit(-1);
    }
}


struct PCB *createProcess()
{

    struct processData message;
    int rec_val = msgrcv(msgq_id, &message, sizeof(message), 0, IPC_NOWAIT);
    if (rec_val == -1)
    {
        return NULL;
    }
    else
    {
        struct PCB *newProcess=malloc(sizeof(struct PCB));
        newProcess->arrival=message.arrivaltime;
        newProcess->brust=message.runningtime;
        newProcess->id=message.id;
        newProcess->priority=message.priority;
        newProcess->running=0;
        newProcess->wait=0;
        newProcess->stop=0;
        return newProcess;
    }
}

void HPF();
void SRTN();
void RR();

void handl(int signum) {}
void Writeperf();
int main(int argc, char * argv[])
{
    initClk();
    SchedulerLog = fopen("scheduler.log", "w");
    schedulerperf=fopen("scheduler.perf", "w");
    //upon termination release the clock resources.
    //shared memory between process and scheduler (remaining time)
    remainingTimeSharedMemory();
    //message queue between process_generator and scheduler (insert processes)
    processesMessageQueue();
    //shared memory between process and scheduler (previous clock)
    prevClkSharedMemory();
    numberOfProcessarrived();
    arrivals =(int *) shmat(arrivalsshID, (void *)0, 0);
    remainingTime=(int*)shmat(shmid1,0,0);
     if (argc<2) {
		printf("Too few arguments. Exiting!\n");
		exit(1);
	}
    signal(SIGCHLD,handl);
    numOfProcesses = atoi(argv[0]);
   WTA_Arr=malloc(numOfProcesses*sizeof(double));
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
            SRTN();
            break;

        case 3:
            queue=createQueue(numOfProcesses);
            RR();
            free(queue);
            break;

        default:
        HPF();
    }
    Writeperf();
    printf("Scheduling done\n");
    shmdt(remainingTime);
    shmdt(arrivals);
    shmctl(shmid1, IPC_RMID, NULL);
    shmctl(prevClkID, IPC_RMID, NULL);
    shmctl(arrivalsshID, IPC_RMID, NULL);
    fclose(SchedulerLog);
    fclose(schedulerperf);
    destroyClk(true);
}
void Writeperf()
{
     fprintf(schedulerperf, "CPU utilization = %0.2f%c \n",((float)totalbrust/finalclk)*100,'%');
     fprintf(schedulerperf,"Avg WTA = %0.2f \n",TotalWTA/numOfProcesses);
     double avg=TotalWTA/numOfProcesses;
     fprintf(schedulerperf,"Avg Waiting = %0.2f \n",TotalWaitingTime/numOfProcesses);
     double sd=0;
     for(int i=0;i<numOfProcesses;i++)
     {
         sd+=pow((WTA_Arr[i]-avg),2);
     }
      fprintf(schedulerperf,"Std WTA = %0.2f \n",sd/numOfProcesses);
}

void resumeProcess(struct PCB* p)
{
    p->wait += getClk()-p->stop;
    fprintf(SchedulerLog, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",
                                getClk(), p->id, p->arrival, p->brust,p->brust-p->running, p->wait);
    int *prev =(int *) shmat(prevClkID, (void *)0, 0);
    *prev=getClk();
    shmdt(prev);
    kill(p->pid,SIGCONT);
}
void finishProcess(struct PCB* p)
{
    int stat_loc;
    waitpid(runningProcess->pid, &stat_loc, 0);    
    p->wait = getClk()-p->arrival-p->brust;
    double WTA = (getClk() - p->arrival) * 1.0 / p->brust;
    if (p->brust==0)
      WTA=0;
    TotalWaitingTime+=p->wait;
    TotalWTA+=((int)(WTA*100))/100.0;
    WTA_Arr[idx]=((int)(WTA*100))/100.0;
    idx++;
    totalbrust+=p->brust;
    finalclk=getClk();
    fprintf(SchedulerLog, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n", 
                               getClk(), p->id, p->arrival, p->brust, p->brust-p->running ,p->wait,getClk() - p->arrival,WTA);             
                    
}
void stopProcess(struct PCB* p)
{
    if(typeAlgo == 3&&runningProcess->running==runningProcess->brust)
    {
        finishProcess(p);
        finishedProcesses++;
        runningProcess=NULL;
        return ;
    }
    p->stop = getClk();
    fprintf(SchedulerLog, "At time %d process %d stopped arr %d total %d remain %d wait %d\n", 
                               getClk(), p->id, p->arrival, p->brust,p->brust-p->running, p->wait);
    kill(p->pid,SIGSTOP);
}



void startProcess(struct PCB* p)
{
    p->start = getClk();
    p->wait += p->start-p->arrival;
    fprintf(SchedulerLog, "At time %d process %d started arr %d total %d remain %d wait %d\n", 
                               getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);
}

void HPF(){
    printf("HPF running..\n");
    while(finishedProcesses < numOfProcesses){
        while(arrivals[getClk()])
        {
            struct PCB *curr = createProcess();
        while(curr)
        {
            arrivals[getClk()]--;
            push(&readyQueue,curr,curr->priority);
            curr=createProcess();
            
        }
        }
        if(!runningProcess && !isEmpty(&readyQueue))
        {
            runningProcess = peek(&readyQueue);
            pop(&readyQueue);
            *remainingTime = runningProcess->brust;
            startProcess(runningProcess);
            int pid = fork();
            if (pid == -1)
            {
  	            scanf("error in fork in scheduler\n");
                kill(getpgrp(), SIGKILL);
            }
            if(pid == 0)
            {
                execl("./process","process", NULL);
            }
        }
        if(*remainingTime == 0 && runningProcess)
        {
            runningProcess->running = runningProcess->brust;
            finishProcess(runningProcess);
            free(runningProcess);
            runningProcess = NULL;
            finishedProcesses++;
        }
    }
}

void SRTN(){
    printf("SRTN running..\n");
    while(finishedProcesses < numOfProcesses){   
         while(arrivals[getClk()])
        {
                struct PCB *curr = createProcess();
                while(curr)
             {
                arrivals[getClk()]--;  
                push(&readyQueue, curr, curr->brust - curr->running);
                curr=createProcess();
             }
        }
          if(*remainingTime<=0 && runningProcess )
        {
            runningProcess->running=runningProcess->brust;
            finishProcess(runningProcess);
            free(runningProcess);
            runningProcess=NULL;
            finishedProcesses++;
        }
       if(runningProcess && !isEmpty(&readyQueue))
        {
           temporary = peek(&readyQueue);
           if(temporary->brust < *remainingTime)
           {
                runningProcess->running = runningProcess->brust-(*remainingTime);
                push(&readyQueue,runningProcess, runningProcess->brust-runningProcess->running);
                push(&Stopping_Resuming_Queue, runningProcess, runningProcess->brust-runningProcess->running);
                
                stopProcess(runningProcess);
                runningProcess = NULL;  
           }
        }
        if(!runningProcess && !isEmpty(&readyQueue))
        {
            runningProcess = peek(&readyQueue);
            pop(&readyQueue);
            while(runningProcess->brust==0)
            {
                startProcess(runningProcess);
                finishProcess(runningProcess);
                free(runningProcess);
                runningProcess=NULL;
                finishedProcesses++;
                runningProcess = peek(&readyQueue);
                pop(&readyQueue);
            }
            *remainingTime = runningProcess->brust-runningProcess->running;
            if(IsTHere(&Stopping_Resuming_Queue,runningProcess->id))
            {
                resumeProcess(runningProcess);
            }
            else
            {
                startProcess(runningProcess);
            int pid = fork();
            if (pid == -1)
            {
  	            printf("error in fork in scheduler\n");
                kill(getpgrp(), SIGKILL);
            }
            if(pid==0)
            {
                execl("./process","process", NULL);
            }
            runningProcess->pid=pid;
            }
        }
        if(*remainingTime==0 && runningProcess )
        {
            runningProcess->running=runningProcess->brust;
            finishProcess(runningProcess);
            free(runningProcess);
            runningProcess=NULL;
            finishedProcesses++;
        }
    }
}
void RR()
{
    int remainingStart=0;
	printf("Round Robin with time slot %d running..\n", slot);
    while(finishedProcesses < numOfProcesses){
        while(arrivals[getClk()])
        {
            struct PCB *curr = createProcess();
        while(curr)
        {
            arrivals[getClk()]--;   
            printf("At time %d process %d arrived arr  %d total %d remain  %d wait %d\n", 
                               getClk(), curr->id, curr->arrival, curr->brust, curr->brust-curr->running , curr->wait);
            enqueue(queue,curr);
            
            curr=createProcess();
        }
        }
        
        if(runningProcess)
        {
        if(*remainingTime == 0)
        {
            runningProcess->running = runningProcess->brust;
            finishProcess(runningProcess);
            free(runningProcess);
            runningProcess = NULL;
            finishedProcesses++;
        }
        else if(remainingStart-*remainingTime==slot&&!arrivals[getClk()])
        {
        remainingStart=*remainingTime;
            if(!isEmptyQ(queue))
            {
                runningProcess->running=runningProcess->brust-*remainingTime;
                stopProcess(runningProcess);
                if(runningProcess)
                enqueue(queue,runningProcess);
                runningProcess=NULL;
            }
        }
        }
        if(!runningProcess && !isEmptyQ(queue))
        {
            runningProcess = dequeue(queue);
            *remainingTime = runningProcess->brust-runningProcess->running;
            if(runningProcess->running)
            {
                resumeProcess(runningProcess);   
            }
            else{
                startProcess(runningProcess);
                int pid = fork();
                if (pid == -1)
                {
  	                scanf("error in fork in scheduler\n");
                    kill(getpgrp(), SIGKILL);
                }
                if(pid == 0)
                {
                    execl("./process","process", NULL);
                }
                runningProcess->pid=pid;
            }
            remainingStart=*remainingTime;
        }
        
    }
}
