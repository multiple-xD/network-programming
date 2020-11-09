#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

#define CMD_SIZE 50

char* delim_pipe;
char* delim_space;
int global_commands;

char ***parse_input(char *input_str);
char **parse_cmd(char *cmd);
int param_count(char *command);
int cmd_count(char *input_str);

int execute(int count, char ***parsed_input);
int fork_redirect(int in, int out, char ***parsed_input);

char **history;
int hist_count = 0;

void SIGINT_handler();
void SIGQUIT_handler();

void main(){

    //Signal Handling
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask, SIGINT);
    sigdelset(&mask, SIGQUIT);
    sigprocmask(SIG_SETMASK, &mask, NULL);
    signal(SIGINT, SIGINT_handler);
    signal(SIGQUIT, SIGQUIT_handler);

    // Allocate memory for user commands and delimiter
    char *input_str = malloc(sizeof(char)*CMD_SIZE);
    delim_pipe = (char *)malloc(sizeof(char)); *delim_pipe = '|';
    delim_space = (char *)malloc(sizeof(char)); *delim_space = ' ';

    // Allocate memory for history storage
    history = (char **)malloc(sizeof(char)*100);
    int i;
    for(i=0; i<100; i++)
        history[i] = (char *)malloc(CMD_SIZE);

    pid_t child, wpid;
    int status;

    while(1){
        // User Input
        printf("user@machine:~$ ");
        fgets(input_str, CMD_SIZE, stdin);
        strtok(input_str, "\n");

        // Storing history
        strcpy(history[hist_count], input_str); 
        hist_count++;

        // Setting global counter
        global_commands = cmd_count(input_str);

        // Parse the input
        char ***parsed_input = parse_input(input_str);

        // Execution using execvp() which checks PATH
        if(fork() == 0){
            if(execute(global_commands,parsed_input) == -1)
                printf("File is not in PATH\n");
        }
        
        // Wait for child to execute all commands
        wait(NULL);
        printf("PID: %d  Status: RUNNING\n", getpid()); // The shell will keep on running so i added the same as the status
    }
}

// parse all commands here
char ***parse_input(char *input_str){

    char *dummy_parse_input = (char *)malloc(sizeof(input_str));
    strcpy(dummy_parse_input, input_str);


    int count = cmd_count(input_str);
    int i, j;
    char ***parsed_input = (char ***)malloc(sizeof(parsed_input)*count);

    parsed_input[0] = parse_cmd(strtok(dummy_parse_input, delim_pipe));
    for(i=1; i<count; i++){
        strcpy(dummy_parse_input, input_str);
        strtok(dummy_parse_input, delim_pipe);
        
        for(j=0; j<i-1; j++)
            strtok(NULL, delim_pipe);

        parsed_input[i] = parse_cmd(strtok(NULL, delim_pipe));
    }

    return parsed_input;
}

char **parse_cmd(char *command){
    char *dummy_parse_cmd = (char *)malloc(sizeof(command));
    strcpy(dummy_parse_cmd, command);

    int count = param_count(dummy_parse_cmd);
    int i=0; 
    
    char **command_arr = (char **)malloc(sizeof(command_arr)*count);

    command_arr[0] = strtok(dummy_parse_cmd, delim_space);
    for(i=1; i<count; i++)
        command_arr[i] = strtok(NULL, delim_space);

    return command_arr;
}

// Parameters in each command
int param_count(char *command){
    char *dummy_param_count = (char *)malloc(sizeof(command));      // Create a dummy string to not affect original
    strcpy(dummy_param_count, command);
    int count=0;
    strtok(dummy_param_count, delim_space);
    while(strtok(NULL, delim_space)){
        count++;
    }
    return count+1;
}

// Total no of commands
int cmd_count(char *input_str){
    char *dummy_cmd_count = (char *)malloc(sizeof(input_str));    // Create a dummy string not to affect original
    strcpy(dummy_cmd_count, input_str);
    int count=0;
    strtok(dummy_cmd_count, delim_pipe);
    while(strtok(NULL, delim_pipe)){
        count++;
    }
    return count+1;
}

//execute commands here
int execute(int count, char ***parsed_input){
  int i;
  pid_t pid;
  int in, fd [2];

  in = 0;
  // Executing the pre final stages of pipeline
  for (i = 0; i < count - 1; ++i){
      pipe (fd);
      if(fork_redirect(in, fd [1], parsed_input + i) == -1){
          printf("File is not in PATH\n");
      }
      close (fd [1]);
      in = fd [0];
  }

  // Executing final stage of pipeline
  if(in != 0){
    dup2(in, 0);
  }

  return execvp(parsed_input[i][0], parsed_input[i]);
}

// forking and redirecting routine
int fork_redirect(int in, int out, char ***parsed_input)
{
  pid_t pid;
  if ((pid = fork ()) == 0){
      if (in != 0){
          dup2 (in, 0);
          close (in);
      }
      if (out != 1){
          dup2 (out, 1);
          close (out);
      }
      return execvp(**parsed_input, *parsed_input);
  }
  return pid;
}

// SIGINT handler
void SIGINT_handler(){
    int j;
    printf("\n");
    for(j=hist_count-1; j>hist_count-11 && j>=0; j--){
        printf("%s\n", history[j]);
    }
}

// SIGQUIT handler
void SIGQUIT_handler(){
    printf("\nDo you really want to exit (y/n): ");
    char *resp = malloc(sizeof(char));
    fgets(resp, sizeof(resp), stdin);
    strtok(resp, "\n");

    if(*resp == 'y'){
        free(resp);
        exit(1);
    }
    else{
        return;
    }
}
