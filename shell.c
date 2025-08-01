#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<string.h>

char **get_input(char* input);
void sigintHandler(int sig_num);
void printcommand(char **arr);
int cd(char *path);

#define MAX_SIZE 1024
#define MAX_PATH_LEN 40

int main(){
    char *inp = malloc(MAX_SIZE*sizeof(char));
    if(inp == NULL){
        perror("\nOut of memory...");
        exit(1);
    }
    pid_t process_id;
    int stat_loc;

    signal(SIGINT, sigintHandler);


    while(1){
        //Clearing input buffer
        memset(inp, '\0', MAX_SIZE);

        //Getting Input
        char* current_path = getcwd(NULL, 0);
        if(strlen(current_path)<=MAX_PATH_LEN){
            printf("\nMinishell@%s> ", current_path);
        }
        else{
            printf("\n%s> ", current_path+strlen(current_path)-MAX_PATH_LEN);
        }

        // printf("\nMinishell> ");
        fgets(inp, MAX_SIZE, stdin);
        inp[strcspn(inp, "\n\r")] = 0;
        char **arr = get_input(inp);

        if(arr[0] == NULL){
            free(arr);
            continue;
        }
        else if(strcmp(arr[0], "exit")==0){
            printf("\n[+] Exiting...\n");
            exit(0);
        }
        else if(strcmp(arr[0], "cd")==0){
            if(arr[1]==NULL){
                arr[1] = getenv("HOME");
            }
            if (cd(arr[1])<0){
                perror(arr[1]);
            }
            continue;
        }

        //creating a new fork for the process
        process_id = fork();
        if(process_id<0){
            printf("\n[-] Error Occured...");
            exit(1);
        }
        if(process_id == 0){
            if(execvp(arr[0], arr)<0){
                perror(arr[0]);
                exit(1);
            }
        }
        else{
            waitpid(process_id, &stat_loc, WUNTRACED);
            continue;
        }
        free(arr);
    }
}

char **get_input(char* input){
    int size = 8;
    char **command = malloc(size*sizeof(char*));
    if(command==NULL){
        perror("\nOut of memory...");
    }
    char *seperator = " ";
    char *parsed;
    int index = 0;
    
    parsed = strtok(input, seperator);
    while(parsed!=NULL){
        command[index] = parsed;
        index++;
        
        parsed = strtok(NULL, seperator);
        
        if(index>=size-1){
            size+=8;
            command = realloc(command, size*sizeof(char*));
        }
    }
    command[index] = NULL;
    return command;
}

int cd(char* path){
    return chdir(path);
}

void sigintHandler(int sig_num) {
    printf("\n[-] Cannot be terminated using Ctrl+C! Continuing execution...");
    printf("\n[+] type 'exit' to exit...\n");
}
void printcommand(char **arr){
    //Printing entered command
    printf("Entered command: ");
    int temp = 0;
    while(arr[temp]!=NULL){
        printf("{%s}, ", arr[temp]);
        temp++;
    }
    printf("{%s}, ", arr[temp]);
    printf("\n");
}