#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#define FILEPATH_SIZE 50
#define MSG_SIZE 500
#define ADD_FILE 1
#define ADD_CHUNK 2
#define CP 3
#define MV 4
#define RM 5

int msgid;

struct msg_buf{
    long msg_type;
    char msg[MSG_SIZE];
} message;

char *add_file();
char *add_chunk(char *filepath);
void mv();
void cp();
void rm();

/****************************************
            CLIENT CODE
*****************************************/
void main(){

    key_t key = ftok(".", 21);
    msgid = msgget(key, 0666);

    char *newly_added_file;
    int request;

    while(1){ 
        printf("\nEnter number corresponding to request\n");
        printf("1. ADD_FILE\n");
        printf("2. ADD_CHUNK\n");
        printf("3. CP\n");
        printf("4. MV\n");
        printf("5. RM\n");
        
        fflush(stdin);
        scanf("%d", &request);
        getchar();
        
        switch(request){
            case 1: newly_added_file = add_file();break;
            case 2: add_chunk(newly_added_file);break;
            case 3: mv();break;
            case 4: cp();break;
            case 5: rm();break;
            default: printf("Enter a valid request");
        }
    }
} 

char *add_file(){
    fflush(stdin);
    printf("Enter filename and filepath (file.txt /...): ");
    char *file = malloc(sizeof(char)*FILEPATH_SIZE); 
    fgets(file, FILEPATH_SIZE, stdin);
    strtok(file, "\n");
    strcpy(message.msg, file);
    message.msg_type = ADD_FILE;
    msgsnd(msgid, &message, sizeof(message), 0);
    while(!msgrcv(msgid, &message, sizeof(message), ADD_FILE, 0));
    printf("Created file %s\n", message.msg);
    return file;
}

char *add_chunk(char *filepath){
    int chunk_size;
    printf("Chunk can only be created for the last file created\n");
    printf("Enter chunk size: ");
    scanf("%d", &chunk_size); 
    char *chunk = (char *)malloc(chunk_size + FILEPATH_SIZE);
    
    fflush(stdin);
    printf("Enter chunk data: ");
    scanf("%s", chunk); 
    strcat(chunk, "\n");
    strcat(chunk, filepath);
    message.msg_type = ADD_CHUNK;
    strcpy(message.msg, chunk);
    msgsnd(msgid, &message, sizeof(message), 0);
    while(!msgrcv(msgid, &message, sizeof(message), ADD_CHUNK, 0));
    printf("\nData server addresses fetched: %s\n\n", message.msg);
    return chunk;
}

void mv(){
    printf("Specify old path and new path delimited by space (mv /dir/oldpath /dir2/newpath)\n");
    fflush(stdin);
    char *new_path = (char *)malloc(FILEPATH_SIZE);
    fgets(new_path, FILEPATH_SIZE, stdin);
    strtok(new_path, "\n");
    strcpy(message.msg, new_path);
    message.msg_type = RM;
    msgsnd(msgid, &message, sizeof(message), 0);
}

void cp(){
    printf("Specify old path and new path delimited by space (cp /dir/oldpath/ /dir2/newpath)\n");
    fflush(stdin);
    char *copy_path = (char *)malloc(FILEPATH_SIZE);
    fgets(copy_path, FILEPATH_SIZE, stdin);
    strtok(copy_path, "\n");
    strcpy(message.msg, copy_path);
    message.msg_type = CP;
    msgsnd(msgid, &message, sizeof(message), 0);
}

void rm(){
    printf("under construction");
}
