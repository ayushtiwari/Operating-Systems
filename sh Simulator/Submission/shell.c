//
//  main.c
//  sh_simulator
//
//  Created by Ayush Tiwari on 24/01/20.
//  Copyright © 2020 Ayush Tiwari. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <libgen.h>

#define DEBUG 0

char *search_paths[32];

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

int external_command_validity(char *cmd) {

    char fname[256];

    if(!cmd) return -1;
    if(cmd && cmd[0]=='.') {
        char cwd[128];
        getcwd(cwd, 128);

        strcpy(fname, cwd);
        strcat(fname, "/");
        strcat(fname, basename(cmd));
        
        if(access(fname, F_OK) != -1) {
            return 0;
        } else {
            return -1;
        }
    }

    int i=0;
    while(search_paths[i]) {
        bzero(fname, 256);
        strcpy(fname, search_paths[i]);
        strcat(fname, "/");
        strcat(fname, cmd);
        if(access(fname, F_OK) != -1) {
            return 0;
        }
        i++;
    }
    return -1;
}

typedef struct simple_command_line {
    char *cmd;
    char *arg[32];
    char *stdin_file;
    char *stdout_file;
    struct simple_command_line *next;
}SCL;

typedef struct complex_command_line {
    SCL* head;
    int to_wait;
}CCL;

int init_simple_command_line(SCL* cmd_line) {
    cmd_line->cmd = NULL;
    for(int i=0; i<32; i++) cmd_line->arg[i] = NULL;
    cmd_line->stdin_file = NULL;
    cmd_line->stdout_file = NULL;
    cmd_line->next = NULL;
    return 0;
}

int init_complex_command_line(CCL* c_cmd_line) {
    c_cmd_line->head = NULL;
    c_cmd_line->to_wait = 1;
    return 0;
}

int delete_command_list(CCL* c_cmd_line) {
    if(c_cmd_line->head!=NULL) {
        SCL* prev=NULL;
        SCL* t=c_cmd_line->head;
        while(t) {
            prev = t;
            t = t->next;
            free(prev);
        }
    }
    return 0;
}

int print_simple_command_line(SCL* cmd_line) {
    printf("cmd : %s\n", cmd_line->cmd);
    printf("arg : ");
    int i=0;
    while(cmd_line->arg[i]) {
        printf("%s,", cmd_line->arg[i]);
        i++;
    }
    printf("\n");
    printf("stdin : %s,\n", cmd_line->stdin_file);
    printf("stdout : %s,\n", cmd_line->stdout_file);
    return 0;
}

int print_complex_command_line(CCL* c_cmd_line) {
    SCL *t = c_cmd_line->head;
    while(t) {
        print_simple_command_line(t);
        printf("----\n");
        t = t->next;
    }
    printf("to_wait : %d\n", c_cmd_line->to_wait);
    return 0;
}

int parse_simple_command(SCL* cmd_line, char *command_string) {

    char *delim = " \t\n";
    char *token = strtok(command_string, delim);

    if(!token) return 1;
    cmd_line->cmd = strdup(token);

    int i=0;
    while(token && i<31) {

        if(token[0]=='>') {
            if(strlen(token)==1) {
                token = strtok(NULL, delim);
                if(!token) return 1;
                cmd_line->stdout_file = strdup(token);
            } else {
                cmd_line->stdout_file = strdup(token+1);
            }
        } else if(token[0]=='<') {
            if(strlen(token)==1) {
                token = strtok(NULL, delim);
                if(!token) return 1;
                cmd_line->stdin_file = strdup(token);
            } else {
                cmd_line->stdin_file = strdup(token+1);
            }
        } else {
            cmd_line->arg[i] = strdup(token);
            i++;
        }
        token = strtok(NULL, delim);
    }
    cmd_line->arg[i] = NULL;
    
    return 0;
}

int parse_complex_command(CCL* c_cmd_line, char *command_string) {
    char simple_command[128];

    SCL *cmd_line_previous;
    int i = 0;

    while(command_string[i] && (command_string[i]==' ' || command_string[i]=='\t' || command_string[i]=='\n')) i++;
    while(command_string[i]) {
        SCL *cmd_line = (SCL*)malloc(sizeof(SCL));
        init_simple_command_line(cmd_line);

        int j = 0;
        while(command_string[i] && command_string[i]!='|' && command_string[i]!='&') 
            simple_command[j++] = command_string[i++];
        simple_command[j]='\0';

        if(parse_simple_command(cmd_line, simple_command))
            return 1;

        if(c_cmd_line->head==NULL) 
            c_cmd_line->head = cmd_line;
        else 
            cmd_line_previous->next = cmd_line;

        cmd_line_previous = cmd_line;
        
        if(command_string[i]=='&') {
            c_cmd_line->to_wait = 0;
            return 0;
        }
        if(command_string[i]=='\0') return 0;
        i++;
    }
    return 0;
}

int do_simple_command(SCL* cmd_line) {

    // See for IO redirections and execute

    if(cmd_line->stdin_file!=NULL) {
        int in_fd = open(cmd_line->stdin_file, O_RDONLY);
        if(in_fd == -1) {
            printf("Error : unable to open %s\n", cmd_line->stdin_file);
            close(in_fd);
            exit(1);
        }
        close(0);
        dup(in_fd);
        close(in_fd);
    }
    if(cmd_line->stdout_file!=NULL) {
        int out_fd = open(cmd_line->stdout_file, O_WRONLY|O_CREAT|O_TRUNC, 0644);
        if(out_fd == -1) {
            printf("Error : unable to open %s\n", cmd_line->stdout_file);
            close(out_fd);
            exit(1);
        }
        close(1);
        dup(out_fd);
        close(out_fd);
    }
    execvp(cmd_line->cmd, cmd_line->arg);
    return 1;
}

int complex_command_execute(SCL* cmd_line) {

    if(external_command_validity(cmd_line->cmd)==-1) {
        printf("Error : %s : command not found!\n", cmd_line->cmd);
        exit(1);
    }

    if(cmd_line->next==NULL) {
        do_simple_command(cmd_line);
    } else {
        int pd[2];
        pipe(pd);

        if(fork()) {
            close(pd[1]);
            dup2(pd[0], 0);
            close(pd[0]);

            complex_command_execute(cmd_line->next);
        } else {
            close(pd[0]);
            dup2(pd[1], 1);
            close(pd[1]);

            do_simple_command(cmd_line);
        }
    }
    return 1;
}

int main(int argc, const char * argv[], char * env[]) {

    init_search_paths(env);

    char cwd[128], machine[128], user[128], prompt[128]="$";
    char command_input[256];
    CCL* c_cmd_line = (CCL*)malloc(sizeof(CCL));
    
    while(1) {
        bzero(command_input, 256);

        init_complex_command_line(c_cmd_line);
        
        getlogin_r(user, 128);
        gethostname(machine, 128);
        getcwd(cwd, 128);
        
        printf("%s:%s %s%s ", machine, basename(cwd), user, prompt);
        
        fgets(command_input, 256, stdin);

        if(parse_complex_command(c_cmd_line, command_input)) {
            printf("Syntax error!\n");
            continue;
        }

        // For two commands
        if(c_cmd_line->head==NULL) continue;

        if(DEBUG)
            print_complex_command_line(c_cmd_line);
        
        SCL* cmd_line = c_cmd_line->head;

        if(strcmp(cmd_line->cmd, "cd")==0) {
            if(chdir(cmd_line->arg[1])==-1) {
                printf("Error : No such directory : %s\n", cmd_line->arg[1]);
            }
        } else if(strcmp(cmd_line->cmd, "exit")==0 || strcmp(cmd_line->cmd, "logout")==0) {
            printf("logged out!\n");
            return 0;
        } else {
            int pid = fork();
            if(pid) {
                int status;
                if(c_cmd_line->to_wait) {
                    waitpid(pid, NULL, 0);
                    if(DEBUG)
                        printf("Done, status=%d\n", status);
                }
            } else {
                complex_command_execute(cmd_line);
            }
        }
        delete_command_list(c_cmd_line);
    }
    return 0;
}
