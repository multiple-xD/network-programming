#include <stdio.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/sem.h>

int PHIL_COUNT;          // Number of philosophers
int chopsticks;          // Semaphore id for resources        
int at_table;            // Sempaphore id for aligned execution 
pid_t *philosophers;     // Array storing PIDs of all philosopher processes

// SIGINT Handler
void terminate(){
    int i;
    for(i=0; i<PHIL_COUNT; i++){
        kill(philosophers[i], SIGINT);
    }
    semctl(chopsticks, 0, IPC_RMID, 0);
    semctl(at_table, 0, IPC_RMID, 0);
    exit(1);
}

// Dining Philosophers
int execute(int n){
    int i, j, first, second;

    // sembuf for performing operations
    struct sembuf op;            
    op.sem_flg = 0;

    // Avoid deadlock by switching the request order of the last process
    first =  (n < PHIL_COUNT) ? n : 0;                
    second = (n < PHIL_COUNT) ? (n+1) : PHIL_COUNT-1; 

    // Decrementing sem_val
    op.sem_op = -1;
    op.sem_num = 0;
    semop(at_table, &op, 1);

    // Suspending process until sem_val is decremented to zero by remaining processes
    op.sem_op = 0;
    op.sem_num = 0;
    semop(at_table, &op, 1);

    while(1) {
        // Pickup first chopstick
        op.sem_op = -1;
        op.sem_num = first;
        semop(chopsticks, &op, 1);
    
        // Pickup second chopstick
        op.sem_op = -1;
        op.sem_num = second;
        semop(chopsticks, &op, 1);

        // Eat 
        printf("Philosopher %d with PID %d is eating\n*****************************************\n", n+1, getpid());
        
        // Put down first chopstick
        op.sem_op = +1;
        op.sem_num = first;
        semop(chopsticks, &op, 1);

        // Put down second chopstick
        op.sem_op = +1;
        op.sem_num = second;
        semop(chopsticks, &op, 1);

        // Added delay
        sleep(1);
    }
}

void main(){
    // Handle SIGINT
    signal(SIGINT, terminate);

    // User Input
    printf("Enter no. of Philosophers: ");
    scanf("%d", &PHIL_COUNT);

    // Initialize PID array
    philosophers = (pid_t *)malloc(sizeof(pid_t)*PHIL_COUNT);

    // Initialize System V Semaphores
    chopsticks = semget(IPC_PRIVATE, PHIL_COUNT, IPC_CREAT | 0600);
    int i;
    for(i=0; i<PHIL_COUNT; i++){
        semctl(chopsticks, i, SETVAL, 1);
    }
    at_table = semget(IPC_PRIVATE, 1, IPC_CREAT | 0600);
    semctl(at_table, 0, SETVAL, PHIL_COUNT);

    // Forking and Execution
    for(i=0; i < PHIL_COUNT; i++){
        int pid = fork();
        if(pid == 0){               
          execute(i);
          exit(1);      
        }
        else{                       
          philosophers[i] = pid;  
        }
    }

    // Wait for SIGINT
    pause();
}

