#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

char *search_paths[32];

typedef struct command {
    char *cmd;
    char *arg[32];
    char *infile;
    char *outfile;
} COMMAND;

COMMAND* new_command() {
    COMMAND* command = (COMMAND*)malloc(sizeof(COMMAND));
    command->cmd = NULL;
    for(int i=0; i<32; i++)
        command->arg[i] = NULL;
    command->infile = NULL;
    command->outfile = NULL;
    return command;
}

void print_command(COMMAND *command) {
    printf("cmd : %s\n", command->cmd);
    printf("args : ");
    int i=0; 
    while(command->arg[i]) {
        printf("%s ", command->arg[i]);
        i++;
    }
    printf("\n");
    printf("infile : %s\n", command->infile);
    printf("outfile : %s\n", command->outfile);
}

int init_search_paths(char *env[]) {

    char all_path[128];

    int i=0;
    while(env[i]) {
        if(strlen(env[i]) > 5 && env[i][0]=='P' && env[i][1]=='A' && env[i][2]=='T' && env[i][3]=='H') {

            int j=5;
            while(env[i][j]) {
                all_path[j-5]=env[i][j];
                j++;
            }
            all_path[j-5] = 0;

            char *each_path = strtok(all_path, ":");
            int k=0;
            while(each_path && k < 32) {
                search_paths[k] = strdup(each_path);
                each_path = strtok(NULL, ":");
                k++;
            }
            search_paths[k] = NULL;

            break;
        }
        i++;
    }
    
    return 0;
}

int command_path_index(char *cmd) {
    int i=0, temp_file;
    while(search_paths[i]) {
        
        char full_path[256];
        strcpy(full_path, search_paths[i]);
        strcat(full_path, "/");
        strcat(full_path, cmd);

        temp_file = open(full_path, O_RDONLY);

        if(temp_file >=0 ) {
            close(temp_file);
            return i;
        }
        i++;
        close(temp_file);
    }
    return -1;
}

COMMAND* parse_single_command(char *command_string, int *error_flag) {
    COMMAND *command = new_command();
    char command_string_dup[128];
    strcpy(command_string_dup, command_string);
    char *token = strtok(command_string_dup, " \t");
    command->cmd = strdup(token);

    int i=0;
    while(token && i < 31) {
        if(strcmp(token, ">")==0) {
            token = strtok(NULL, " \t");
            if(token) {
                command->outfile = strdup(token);
            } else {
                *error_flag=1;
                return command;
            }
        } else if(strcmp(token, "<")==0) {
            token = strtok(NULL, " \t");
            if(token) {
                command->infile = strdup(token);
            } else {
                *error_flag=1;
                return command;
            }
        } else if(strcmp(token, ">>")==0) {
            token = strtok(NULL, " \t");
            if(token) {
                command->outfile = strdup(token);
            } else {
                *error_flag=1;
                return command;
            }
        } else {
            command->arg[i] = strdup(token);
            i++;
        }

        token = strtok(NULL, " \t");
    }
    *error_flag=0;
    return command;
}

int sh(COMMAND *command, char *env[]) {

    print_command(command);

    int index = command_path_index(command->cmd);

    if(index == -1) {
        return 1;
    }

    char full_path[256];
    strcpy(full_path, search_paths[index]);
    strcat(full_path, "/");
    strcat(full_path, command->cmd);
    printf("%s\n", full_path);

    if(fork()) {
        int status;
        wait(&status);
        printf("done, status=%d\n", status);
    } else {
        if(command->infile) {
            int fd=open(command->infile, O_RDONLY);
            if(fd < 0) {
                exit(1);
            }
            close(0);
            dup(fd);
            close(fd);
        }
        if(command->outfile) {
            int fd=open(command->outfile, O_WRONLY|O_CREAT, 0644);
            if(fd < 0) {
                exit(1);
            }
            close(1);
            dup(fd);
            close(fd);
        }
        execve(full_path, command->arg, env);
    }
    return 0;
}

int main(int argc, char *argv[], char *env[]) {

    init_search_paths(env);

    // int i=0;
    // while(search_paths[i]) {
    //     printf("%s\n", search_paths[i]);
    //     i++;
    // }

    int error_flag;
    // printf("alpha\n");
    COMMAND *command = parse_single_command("ls -l", &error_flag);
    // print_command(command);

    sh(command, env);
    return 0;
}