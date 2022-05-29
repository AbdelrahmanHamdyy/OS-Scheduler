#include "headers.h"
#define MEMORY_LIMIT 1024

//Physical memory 
struct memory_node*memory[11];
struct memory_node
{
    int size;
    int start;
    struct memory_node*next;
};
void rabbit(int start,int i,int size)
{
    struct memory_node*temp=malloc(sizeof(struct memory_node));
    temp->start=start;
    temp->size=size;
    temp->next=memory[i];
    memory[i]=temp;
}
int allocate(int size)
{
    
    int memSize=ceil(log2(size));
    int startSplit=memSize;
    while(startSplit<11 && !memory[startSplit])
    {
        startSplit++;
    } 
    for(int i=startSplit;i>memSize&& i<11;i--)
    {
        int childSize=1<<(i-1);
        struct memory_node*temp=memory[i];
        memory[i]=memory[i]->next;
        int start=temp->start;
        free(temp);
        rabbit(start+childSize,i-1,childSize);
        rabbit(start,i-1,childSize);
    }
     
    if(memory[memSize])
    {
        struct memory_node*temp=memory[memSize];
        memory[memSize]=memory[memSize]->next;
        int start=temp->start;
        free(temp);
        return start;
    }
    else
    {
        return -1;
    }
}
void deallocate(int start,int size)
{
    int i=ceil(log2(size));
    size=1<<i;
    int index=start/size;
    int op=0;
    if(index%2==0)
    {
         op=1;
    }
    else
    {
        op=-1;
    }
     struct memory_node* temp=memory[i];
        int found=0;
         struct memory_node* prev=NULL;
        while(temp){
            int tempIndex=temp->start/temp->size;
            if(tempIndex==(index+op)){
                if(prev==NULL)
                    memory[i]=temp->next;
                else
                    prev->next=temp->next;
                if(index%2==0)
                {
                    deallocate(start, size*2);
                }
                else
                {
                    deallocate(temp->start, size*2);
                }
                  free(temp);
                found=1;
                break;
            }
            prev=temp;
        temp=temp->next;
        }
        if(!found){
           rabbit(start,i,size);
    }
}

//this struct can be considered as letter 

int shmid1,msgq_id,prevClkID,numOfProcesses,typeAlgo,slot,finishedProcesses=0;
int*remainingTime;
union Semun semun;
int sem;
struct PCB *runningProcess,*temporary;
Node* readyQueue,*Stopping_Resuming_Queue;
FILE * SchedulerLog;
FILE * MemoryLog;
struct Queue* queue;
Node* waitingQueue;
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

void prevClkSharedMemory()
{
    key_t prevClkKey;
    prevClkKey=ftok("keyfile",100);
    prevClkID=shmget(prevClkKey,sizeof(int),IPC_CREAT|0666);
    if (prevClkID == -1)
    {
        perror("Error in creating the shared memorytttttttttttttt");
        exit(-1);
    }
}

void ProcessSemaphore() {
    sem = semget(IPC_PRIVATE, 1, 0666 | IPC_CREAT);
    if (sem == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }

    semun.val = 1; /* initial value of the semaphore, Binary semaphore */
    if (semctl(sem, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
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
        newProcess->running=0;
        newProcess->wait=0;
        newProcess->stop=0;
        newProcess->size = message.memsize;
       // printf("\nMessage received: at time %d\n",getClk());
        return newProcess;
    }
}

void HPF();
void SRTN();
void RR();

void handl(int signum) {}

int main(int argc, char * argv[])
{
    initClk();
    rabbit(0,10,MEMORY_LIMIT);
    SchedulerLog = fopen("scheduler.log", "w");
    MemoryLog = fopen("memory.log", "w");
    //printf("********\nana fel scheduler\n***********\n");
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    //shared memory between process and scheduler (remaining time)
    remainingTimeSharedMemory();
    //message queue between process_generator and scheduler (insert processes)
    processesMessageQueue();
    //Semaphore to make sure no two processes are executed at the same time
    //ProcessSemaphore();
    prevClkSharedMemory();

    remainingTime=(int*)shmat(shmid1,0,0);
     if (argc<2) {
		printf("Too few arguments. Exiting!\n");
		exit(1);
	}
    signal(SIGCHLD,handl);
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
    
    printf("Scheduling done\n");
    shmdt(remainingTime);
    shmctl(shmid1, IPC_RMID, NULL);
    //semctl(sem, 0, IPC_RMID);
    fclose(SchedulerLog);
    fclose(MemoryLog);
    destroyClk(true);
}

bool allocateMemoryLog(struct PCB* p) 
{
    printf("Allocating Memory..\n");
    p->startAddress = allocate(p->size);
    int i = p->startAddress;
    printf("After Allocation - start %d - size %d\n", i, p->size);
    if (i == -1)
        return false;
    int j = i + pow(2, ceil(log2(p->size))) - 1;
    fprintf(MemoryLog, "At time %d allocated %d bytes for process %d from %d to %d\n", getClk(), p->size, p->id, i, j);
    return true;
}

void deallocateMemoryLog(struct PCB* p) 
{
    printf("Deallocating Memory..\n");
    int i = p->startAddress;
    int j = i + pow(2, ceil(log2(p->size))) - 1;
    deallocate(p->startAddress, p->size);
    fprintf(MemoryLog, "At time %d freed %d bytes for process %d from %d to %d\n", getClk(), p->size, p->id, i, j);
}

void resumeProcess(struct PCB* p)
{
    p->wait += getClk()-p->stop;
    fprintf(SchedulerLog, "At time %d process %d resume arr  %d total %d remain %d wait %d\n",
                                getClk(), p->id, p->arrival, p->brust,p->brust-p->running, p->wait);
    int *prev =(int *) shmat(prevClkID, (void *)0, 0);
    *prev=getClk();
    shmdt(prev);
    printf("pid: %d, process %d \n",runningProcess->pid,runningProcess->id);
    kill(p->pid,SIGCONT);
    printf("At time %d process %d resume arr  %d total %d remain %d wait %d\n",
                                 getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);
}
void stopProcess(struct PCB* p)
{
    fflush(stdout);
    p->stop = getClk();
    fprintf(SchedulerLog, "At time %d process %d stopped arr  %d total %d remain %d wait %d\n", 
                               getClk(), p->id, p->arrival, p->brust,p->brust-p->running, p->wait);
    kill(p->pid,SIGSTOP);
     printf( "At time %d process %d stopped arr  %d total %d remain %d wait %d\n", 
                                getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);
}

void resumeSRTN(struct PCB* p)
{
    p->wait += getClk()-p->stop;
    fprintf( SchedulerLog, "At time %d process %d resume arr  %d total %d remain %d wait %d\n",
                                getClk(), p->id, p->arrival, p->brust,p->brust-p->running, p->wait);
    // printf("At time %d process %d resume arr  %d total %d remain  %d wait %d\n",
    //                             getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);

}
void stopSRTN(struct  PCB* p)
{
     p->stop=getClk();
    fprintf( SchedulerLog, "At time %d process %d stopped arr  %d total %d remain %d wait %d\n", 
                               getClk(), p->id, p->arrival, p->brust,p->brust-p->running, p->wait);
    // printf( "At time %d process %d stoped arr  %d total %d remain  %d wait %d\n", 
    //                            getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);
}

void finishProcess(struct PCB* p)
{
    fflush(stdout);
    int stat_loc;
    waitpid(runningProcess->pid, &stat_loc, 0);    
    p->wait = getClk()-p->arrival-p->brust;
    deallocateMemoryLog(p);
    double WTA = (getClk() - p->arrival) * 1.0 / p->brust;
      fprintf(SchedulerLog, "At time %d process %d finished arr  %d total %d remain %d wait %d  TA %d WTA %.2f\n", 
                               getClk(), p->id, p->arrival, p->brust, p->brust-p->running ,p->wait,getClk() - p->arrival,WTA);
    printf("At time %d process %d finished arr  %d total %d remain %d wait %d  TA %d WTA %.2f\n", 
                                getClk(), p->id, p->arrival, p->brust, p->brust-p->running ,p->wait,getClk() - p->arrival,WTA);                        
    while(!isEmpty(&waitingQueue)) {
        printf("Inside Waiting Queue Loop\n");
        struct PCB *waitingProcess = peek(&waitingQueue);
        bool allocated = allocateMemoryLog(waitingProcess);
        if (allocated) {
            pop(&waitingQueue);
            if (typeAlgo == 3)
                enqueue(queue, waitingProcess);
            else if (typeAlgo == 2)
                push(&readyQueue, waitingProcess, waitingProcess->brust - waitingProcess->running);
            else
                push(&readyQueue, waitingProcess, waitingProcess->priority);
        }
        else
            break;
    }                      
}

void startProcess(struct PCB* p)
{
    p->start = getClk();
    p->wait += p->start-p->arrival;
    fprintf(SchedulerLog, "At time %d process %d started arr  %d total %d remain %d wait %d\n", 
                               getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);
     printf("At time %d process %d started arr  %d total %d remain %d wait %d\n", 
                                getClk(), p->id, p->arrival, p->brust, p->brust-p->running , p->wait);

}

void HPF(){
    printf("HPF\n");
    while(finishedProcesses < numOfProcesses){
        //sleep(1);
        //printf("yamosahel\n");
        struct PCB *curr = createProcess();
        while(curr)
        {
            //printf("ana keda estalamt element mn el queue\n");
            //printf("\n****\n%d --- %d --- %d --- %d\n****\n",curr->id,curr->arrival,curr->priority,curr->brust);
            fprintf( SchedulerLog,"At time %d process %d arrived arr  %d total %d remain %d wait %d\n", 
                               getClk(), curr->id, curr->arrival, curr->brust, curr->brust-curr->running , curr->wait);
            /*bool allocated = allocateMemoryLog(curr);
            if (allocated)
               push(&readyQueue,curr,curr->priority);
            else
               push(&waitingQueue, curr, curr->size);*/
            push(&readyQueue,curr,curr->priority);
            curr=createProcess();
        }
        // if(runningProcess)
        //     printf("dlw2te fe running process rkm %d w fadlha %d\n",runningProcess->id,*remainingTime);

        if(!runningProcess && !isEmpty(&readyQueue))
        {
            runningProcess = peek(&readyQueue);
            pop(&readyQueue);
            allocateMemoryLog(runningProcess);
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
                printf("%d\n", runningProcess->id);
                execl("./process","process", NULL);
            }
        }

        //int currClk = getClk();
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
    printf("SRTN\n");

    while(finishedProcesses < numOfProcesses){   
        // printf("we are at time %d\n",getClk()); 
        struct PCB *curr = createProcess();
        while(curr)
        {
           // printf("ana keda estalamt element mn el queue\n");
            printf("\n*********\n ID: %d---Arr:%d ---Priority:%d ---Brust:%d\n*********\n",curr->id,curr->arrival,curr->priority,curr->brust);
            fprintf( SchedulerLog,"At time %d process %d arrived arr  %d total %d remain %d wait %d\n", 
                               getClk(), curr->id, curr->arrival, curr->brust, curr->brust-curr->running , curr->wait);
            bool allocated = allocateMemoryLog(curr);
            if (allocated)
               push(&readyQueue, curr, curr->brust - curr->running);
            else
               push(&waitingQueue, curr, curr->size);

            curr=createProcess();
        }
 
       if(runningProcess && !isEmpty(&readyQueue))
        {

           temporary = peek(&readyQueue);
           if(temporary->brust < *remainingTime)
           {
              // printf("there is a process with id =%d has a burst time equals %d which is smaller than the running process with id %d and time %d\n",temporary->id
              // ,temporary->brust,runningProcess->id,*remainingTime);
                runningProcess->running = runningProcess->brust-(*remainingTime);
                struct PCB*MYone = malloc(sizeof(struct PCB));
                MYone->arrival = runningProcess->arrival;
                MYone->brust = runningProcess->brust;
                MYone->id = runningProcess->id;
                MYone->priority = runningProcess->priority;
                MYone->running = runningProcess->running;
                MYone->wait = runningProcess->wait;
                MYone->stop = getClk();
                MYone->size = runningProcess->size;
                push(&readyQueue, MYone, MYone->brust-MYone->running);
                push(&Stopping_Resuming_Queue, MYone, MYone->brust-MYone->running);
                stopSRTN(runningProcess);
                free(runningProcess);
                runningProcess = NULL;  
                killpg(getpgrp(), SIGCHLD);
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

        if(!runningProcess && !isEmpty(&readyQueue))
        {
            
            runningProcess = peek(&readyQueue);
            pop(&readyQueue);
            *remainingTime = runningProcess->brust-runningProcess->running;
            QueuePrint(&readyQueue);
            QueuePrint(&Stopping_Resuming_Queue);
            if(IsTHere(&Stopping_Resuming_Queue,runningProcess->id))
            {
                resumeSRTN(runningProcess);
            }
            else
            {
                startProcess(runningProcess);
            }
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
        }

        if(*remainingTime==0 && runningProcess )
        {
            runningProcess->running=runningProcess->brust;
            finishProcess(runningProcess);
            free(runningProcess);
            runningProcess=NULL;
            finishedProcesses++;
        }
        // sleep(1);
    }
}
void RR()
{
    int remainingStart=0;
	printf("Round Robin with time slot %d\n", slot);
    while(finishedProcesses < numOfProcesses){
        //sleep(1);
        //printf("yamosahel\n");
        struct PCB *curr = createProcess();
        while(curr)
        {
            //printf("ana keda estalamt element mn el queue\n");
            //printf("\n**\n%d --- %d --- %d --- %d\n**\n",curr->id,curr->arrival,curr->priority,curr->brust);
            fprintf( SchedulerLog,"At time %d process %d arrived arr  %d total %d remain %d wait %d\n", 
                               getClk(), curr->id, curr->arrival, curr->brust, curr->brust-curr->running , curr->wait);
            /*printf("At time %d process %d arrived arr  %d total %d remain  %d wait %d\n", 
                               getClk(), curr->id, curr->arrival, curr->brust, curr->brust-curr->running , curr->wait);*/

            bool allocated = allocateMemoryLog(curr);
            if (allocated)
               enqueue(queue,curr);
            else
               push(&waitingQueue, curr, curr->size);
            curr=createProcess();
        }
        if(runningProcess)
        {
            //printf("remaining: %d",*remainingTime);
        if(*remainingTime == 0)
        {
            runningProcess->running = runningProcess->brust;
            finishProcess(runningProcess);
            free(runningProcess);
            runningProcess = NULL;
            finishedProcesses++;
            //printf("elhamdol\n");
        }
        else if(remainingStart-*remainingTime==slot)
        {
            if(!isEmptyQ(queue))
            {
                runningProcess->running+=slot;
                //printf("remaining: %d,%d",*remainingTime,remainingStart);
                stopProcess(runningProcess);
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
                //printf("remianing : %d\n",*remainingTime);
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
                printf("pid: %d, process %d \n",pid,runningProcess->id);
            }
            remainingStart=*remainingTime;
        }
        
    }
}