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
#define ADD_FILE 1
#define ADD_CHUNK 2
#define CP 3
#define MV 4
#define RM 5

#define MAX_FILES 100
#define DADDR1 1
#define DADDR2 2
#define DADDR3 3

struct msg_buf{
    long msg_type;
    char msg[MSG_SIZE];
} message;

int msgid;
int file_no = 0;

struct file **all_files;

struct file {
    char *filepath;
};

struct file *add_file();
void add_chunk();
void mv();

/*********************************************
            METADATA SERVER CODE
**********************************************/
void main(){
    key_t key = ftok(".", 21);
    msgid = msgget(key, IPC_CREAT | 0666);
    struct msqid_ds buf;

    all_files = (struct file **)malloc(sizeof(all_files)*MAX_FILES);
    msgctl(msgid, IPC_STAT, &buf);  

    while(buf.msg_qnum >= 0){
        msgctl(msgid, IPC_STAT, &buf);  
        if(msgrcv(msgid, &message, sizeof(message), ADD_FILE, 0)){
            printf("here in aaddfile\n");
            all_files[file_no] = add_file();
            file_no++;
            printf("Created file %s\n", message.msg);
        }
        else if(msgrcv(msgid, &message, sizeof(message), ADD_CHUNK, 0)){
            printf("here in addchunk\n");
            add_chunk();
        }
        else if(msgrcv(msgid, &message, sizeof(message), MV, 0)){
            mv();
        }
    }
}

struct file *add_file(){
    struct file *new_file = (struct file *)malloc(sizeof(new_file));
    new_file -> filepath = (char *)malloc(sizeof(char)*50); 
    strcpy(new_file -> filepath, message.msg);
    message.msg_type = ADD_FILE;
    msgsnd(msgid, &message, sizeof(message), 0);
    return new_file;
}

void add_chunk(){
    printf("inside m)server addchuni\n");
    message.msg_type = ADD_CHUNK;
    char d_addr[3] = {DADDR1, DADDR2, DADDR3};
    strcpy(message.msg, d_addr);
    msgsnd(msgid, &message, sizeof(message), 0);
} 

void mv(){
    printf("nside mv\n");
    char *new_path = (char *)malloc(sizeof(message.msg));
    strcpy(new_path, message.msg);
    printf("%s\n", strtok(new_path, " "));
    printf("%s\n", strtok(NULL, " "));
}
