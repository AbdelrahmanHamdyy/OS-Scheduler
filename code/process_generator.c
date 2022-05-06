#include "headers.h"

void clearResources(int);

int msgq_id;

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);

    // TODO Initialization

    // Initializing Message Queue

    key_t key_id;
    int send_val;
    FILE *key = fopen("keyfile", "r");
    key_id = ftok("keyfile", 65);
    msgq_id = msgget(key_id, 0666 | IPC_CREAT);
    if (msgq_id == -1)
    {
        perror("Error in creating the message queue\n");
        exit(-1);
    }
    else
        printf("Message Queue created successfully!\n");

    // ------ 1. Read the input files.

    FILE *file = fopen("processes.txt", "r"); // Opening the File to get the number of processes
    if (file == NULL) {
        printf("Error! File not opened.\n");
        return 1;
    }
    int numberOfProcesses = 0;
    char id_str[10], arrival_str[10], runtime_str[10], priority_str[10];
    fscanf(file, "%s %s %s %s", id_str, arrival_str, runtime_str, priority_str);
    while (!feof(file)) {
        int i, a, r, p;
        fscanf(file, "%d\t%d\t%d\t%d", &i, &a, &r, &p);
        numberOfProcesses = i;
    }
    printf("Number of Processes: %d\n", numberOfProcesses);

    fclose(file);
    file = fopen("processes.txt", "r"); // Opening again to store the processes
    if (file == NULL) {
        printf("Error! File not opened.\n");
        return 1;
    }

    struct processData *processes = malloc(sizeof(struct processData) * numberOfProcesses); // Array of processes
    fscanf(file, "%s %s %s %s", id_str, arrival_str, runtime_str, priority_str);
    int index = 0;
    printf("**Input***\n");
    printf("%s  %s  %s  %s\n", id_str, arrival_str, runtime_str, priority_str);
    while (index < numberOfProcesses) {
        int i, a, r, p;
        fscanf(file, "%d\t%d\t%d\t%d", &i, &a, &r, &p);
        processes[index].id = i;
        processes[index].arrivaltime = a;
        processes[index].runningtime = r;
        processes[index].priority = p;
        printf("%d\t%d\t%d\t%d \n", i, a, r, p);
        index++;
    }
    fclose(file); // CLose File

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    
    int algorithm;
    printf("-------------------------------\n");
    printf("Choose a scheduling algorithm:\n");
    printf("1. Non-preemptive Highest Priority First (HPF).\n");
    printf("2. Shortest Remaining time Next (SRTN).\n");
    printf("3. Round Robin (RR).\n");
    scanf("%d", &algorithm);
    printf("-------------------------------\n");

    // 3. Initiate and create the scheduler and clock processes.

    // Clock
    printf("Forking Clock..\n");
    int pid = fork();
    if (pid == -1) 
        perror("Error in creating clock process\n");
    else if (pid == 0) {
        system("gcc clk.c -o clk.out");
        execl("./clk.out", "clk", NULL);
    }
    
    // 4. Use this function after creating the clock process to initialize clock

    initClk();

    // Scheduler
    if (pid != 0) {
        printf("Forking Scheduler..\n");
        int schedulerpid = fork();
        if (schedulerpid == -1) 
            perror("Error in creating scheduler process\n");
        else if (schedulerpid == 0) {
            system("gcc scheduler.c -o scheduler.out");
            if (algorithm == 3) {
                int timeSlot;
                printf("Enter the time slot for Round Robin: ");
                scanf("%d", &timeSlot);
                printf("Scheduling..\n");
                char n_str[10], a_str[10], t_str[10];
                execl("./scheduler.out", string(numberOfProcesses, n_str), string(algorithm, a_str), string(timeSlot, t_str), NULL);
            }
            else
            {
                printf("Scheduling..\n");
                char n_str[10], a_str[10];
                execl("./scheduler.out",  string(numberOfProcesses, n_str), string(algorithm, a_str), NULL);
            }
        }
        else {
            // TODO Generation Main Loop
            // 5. Create a data structure for processes and provide it with its parameters.

                // --> struct processData *processes = malloc(sizeof(struct processData) * numberOfProcesses);

            // 6. Send the information to the scheduler at the appropriate time.

            int current = 0;
            //printf("-------------------------------\n");
            while (current < numberOfProcesses) {
                int arrival = processes[current].arrivaltime;
                if (arrival == getClk()) {
                    //printf("Sending Process %d\n", current + 1);
                    //printf("Current Time: %d, Arrival: %d\n", getClk(), arrival);
                    send_val = msgsnd(msgq_id, &processes[current], sizeof(struct processData), !IPC_NOWAIT);
                    if (send_val == -1)
                        perror("Error in send\n");
                    else {
                        //printf("Message Queue filled\n");
                        //printf("-------------------------------\n");
                    }
                    current++;
                }
            }
            // 7. Clear clock resources
            int stat_loc;
            waitpid(schedulerpid, &stat_loc, 0);
        }
    }
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    printf("Clearing\n");
    msgctl(msgq_id, IPC_RMID, (struct msqid_ds *)0);
    raise(SIGKILL);
}
