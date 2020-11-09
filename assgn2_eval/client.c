#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/msg.h>

void main(){
    msgctl(65537, IPC_RMID, NULL);
}
