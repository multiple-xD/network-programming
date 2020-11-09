#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

void main(){
    pid_t child = fork();

    int i;
    for(i=0; i<5; i++)_{ 
        if(child == 0){
            if(getpid()%2 ==0)
                pause();
            else if(getpid()%2 !=0){
                printf("in odd process\n");
                exit(0);
                signal(
        }

        else if(child > 0){
        }
    }

}
