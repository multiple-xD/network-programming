#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define MSG_SIZE 500
#define PID_SIG 1
#define ADD_FILE 2
#define ADD_CHUNK 3
#define CP 4
#define MV 5
#define RM 6

int msgid;

struct msg_buf{
    long msg_type;
    char msg[MSG_SIZE];
} message;

/***********************************
        DATA SERVER CODE
************************************/
void main(){
    key_t key = ftok(".", 21);
    msgid = msgget(key, 0666);

    while(1){
        pause();
    }
}

