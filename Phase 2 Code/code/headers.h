#include <stdio.h>      //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include "semlib.h"

typedef short bool;
#define true 1
#define false 0

#define SHKEY 300


///==============================
//don't mess with this variable//
int * shmaddr;                 //
//===============================



int getClk()
{
    return *shmaddr;
}


/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
*/
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    while ((int)shmid == -1)
    {
        //Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *) shmat(shmid, (void *)0, 0);
}


/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
*/
char*string(int n,char*result)
{
    sprintf(result, "%d", n);
    return result;
}
void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
    {
        killpg(getpgrp(), SIGINT);
    }
}
//-----------------------------------------------------------
struct PCB
{
    int id;
    int pid;
    int arrival;
    int brust;
    int finish;
    int running;
    int stop;
    int priority;
    int start;
    int wait;
    int startAddress;
    int size;
};
void setPCB(struct PCB* wantedone,int val1,int val2,int val3,int val4,int val5,int val6,int val7,int val8,int val9,int val10)
{
    wantedone->arrival=val1;
    wantedone->brust=val2;
    wantedone->finish=val3;
    wantedone->id=val4;
    wantedone->pid=val5;
    wantedone->priority=val6;
    wantedone->running=val7;
    wantedone->start=val8;
    wantedone->stop=val9;
    wantedone->wait=val10;
}
// Memory Node
struct memory_node
{
    int size;
    int start;
    struct memory_node*next;
};
//-----------------------------------------------------------
// Node
typedef struct node {
    struct PCB* data;
 
    // Lower values indicate higher priority
    int priority;
 
    struct node* next;
} Node;
 
// Function to Create A New Node
Node* newNode(struct PCB* d, int p)
{
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->data = d;
    temp->priority = p;
    temp->next = NULL;
 
    return temp;
} 
// Return the value at head
struct PCB* peek(Node** head)
{
    return (*head)->data;
}
 
// Removes the element with the
// highest priority form the list
void pop(Node** head)
{
    Node* temp = *head;
    (*head) = (*head)->next;
    free(temp);
}

void QueuePrint(Node** head)
{
    Node*start=*head;
    int index=1;
    while(start){
        printf("the node number %d which has id %d and burst time %d\n",index,start->data->id,start->data->brust);
        index++;
        start=start->next;
    }
}
// Function to push according to priority
void push(Node** head, struct PCB* d, int p)
{
    Node* start = (*head);
    if(start==NULL)
    {
        (*head)=newNode(d, p);
        return;
    }
    // Create new Node
    Node* temp = newNode(d, p);
 
    // Special Case: The head of list has lesser
    // priority than new node. So insert new
    // node before head node and change head node.
    if ((*head)->priority > p) {
 
        // Insert New Node before head
        temp->next = *head;
        (*head) = temp;
    }
    else {
 
        // Traverse the list and find a
        // position to insert new node
        while (start->next != NULL &&
            start->next->priority <= p) {
            start = start->next;
        }
 
        // Either at the ends of the list
        // or at required position
        temp->next = start->next;
        start->next = temp;
    }
}

 
// Function to check is list is empty
int isEmpty(Node** head)
{
    return (*head) == NULL;
}

bool IsTHere(Node**head,int val)
{
    Node* start=*head;
    if(start==NULL)
    return false;
    if(start->data->id==val)
    {
        *head=start->next;
        return true;
    }
    while(start->next)
    {
        if(start->next->data->id==val)
        {
           start->next=start->next->next;
           return true;   
        }    
        start=start->next;    
    }
    return false;
}
struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
    int memsize;
};

//----------------------queue---------------------------------
// C program for array implementation of queue
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

// A structure to represent a queue
struct Queue {
	int front, rear, size;
	unsigned capacity;
	struct PCB** array;
};

// function to create a queue
// of given capacity.
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
	struct Queue* queue = (struct Queue*)malloc(
		sizeof(struct Queue));
	queue->capacity = capacity;
	queue->front = queue->size = 0;

	// This is important, see the enqueue
	queue->rear = capacity - 1;
	queue->array = (struct PCB**)malloc(
		queue->capacity * sizeof(struct PCB*));
	return queue;
}

// Queue is full when size becomes
// equal to the capacity
int isFull(struct Queue* queue)
{
	return (queue->size == queue->capacity);
}

// Queue is empty when size is 0
int isEmptyQ(struct Queue* queue)
{
	return (queue->size == 0);
}

// Function to add an item to the queue.
// It changes rear and size
void enqueue(struct Queue* queue, struct PCB* item)
{
	if (isFull(queue))
		return;
	queue->rear = (queue->rear + 1)
				% queue->capacity;
	queue->array[queue->rear] = item;
	queue->size = queue->size + 1;
	//printf("%d enqueued to queue\n", item);
}

// Function to remove an item from queue.
// It changes front and size
struct PCB* dequeue(struct Queue* queue)
{
	if (isEmptyQ(queue))
		return NULL;
	struct PCB* item = queue->array[queue->front];
	queue->front = (queue->front + 1)
				% queue->capacity;
	queue->size = queue->size - 1;
	return item;
}

// Function to get front of queue
struct PCB* front(struct Queue* queue)
{
	if (isEmptyQ(queue))
		return NULL;
	return queue->array[queue->front];
}

// Function to get rear of queue
struct PCB* rear(struct Queue* queue)
{
	if (isEmptyQ(queue))
		return NULL;
	return queue->array[queue->rear];
}
