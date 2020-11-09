#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>

void main(){
        pid_t mypid = wait(NULL); 
        printf("%d\n", mypid);
}

