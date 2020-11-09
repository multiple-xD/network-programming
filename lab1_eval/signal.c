#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

// Global variables
int i=0;
int fork_count = 0;
int sigusr1_count=0;
int sigterm_count=0;
int proc_num;
int proc_trig;

// Random number generator
int rand_num(int range){
    srand(time(0));
    return rand()%range;
}

// SIGUSR1 handler
void SIGUSR1_handler(int sig, siginfo_t *info, void *ucontext){
    sigusr1_count++; 
    if(sigusr1_count == proc_trig){
        printf("##EVEN## %d received %d SIGUSR1 signals, last from %d : TERMINATED SELF\n", getpid(), sigusr1_count, info->si_pid);
        kill(info->si_pid, SIGTERM);
        //kill(info->si_pid, SIGKILL);  sending SIGKILL just after SIGTERM doesnt allow SIGTERM_handler to execute
        exit(0);
    }
    return;
}

// SIGTERM handler
void SIGTERM_handler(int sig, siginfo_t *info, void *ucontext){
    sigterm_count++;
    printf("##ODD##  %d recieved %d SIGTERM signals, last from %d : TERMINATED BY %d\n", getpid(), sigterm_count, info->si_pid, info->si_pid);
}


void main(){
    // User input 
    printf("Enter no. of processes to be created(N): ");
    scanf("%d", &proc_num);
    printf("Enter the number of processes required to trigger a signal(M): ");
    scanf("%d", &proc_trig);

    // Defining sigaction parameters
    struct sigaction usr1;
    usr1.sa_sigaction = SIGUSR1_handler;
    usr1.sa_flags = SA_RESTART | SA_SIGINFO;

    struct sigaction term;
    term.sa_sigaction = SIGTERM_handler;
    term.sa_flags = SA_RESTART | SA_SIGINFO;

    // Allocating memory for storing all child PIDs
    pid_t *pid_list = (pid_t *)malloc(sizeof(pid_t)*proc_num);

    // Looping to create multiple children
    for(i=0; i<proc_num; i++){
        pid_t child = fork();
        fork_count++;

        // Parent code
        if(child > 0){
            pid_list[i] = child;    // Store all child PIDs
        }
            
        // Child code
        else if(child == 0){
            sigaction(SIGUSR1, &usr1, NULL);
            sigaction(SIGTERM, &term, NULL);

            // Odd child code
            if(getpid()%2 == 0){
                while(1){
                    pause();
                }
            }
            // Even child code
            else if(getpid()%2 != 0){
                int j=0, k=0; 
                int even[proc_num];
                for(j=0; j<fork_count-1; j++){
                    if(pid_list[j]%2 == 0){
                        even[k] = pid_list[j];  // Creating a subarray of even processes to 
                        k++;                    // randomly choose a PID from
                    }
                }
                while(1){
                    kill(even[rand_num(k)], SIGUSR1); // Sending SIGUSR1 using a random index in subarray of even PIDs
                }
            }
        }
    }

    // Print list of all children once all are terminated, unlikely to happen 
    while(wait(NULL)>0)     
        wait(NULL);

    for(i=0; i<proc_num; i++){
        printf("Child %d : %d\n", i+1, pid_list[i]);
    }
}
