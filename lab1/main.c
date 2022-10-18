#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include<readline/readline.h>
#include<readline/history.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/resource.h>

char* variables[10];
char* values[10];
int ind=0;
//function to print the current directory
void printd(){
    char cwd[500];
    printf("\ncurrent Directory: %s $",getcwd(cwd, sizeof(cwd)));
}
//function to get the command from user
void getcomm(char* in,char** comm){
    gets(in);
    char* first = strsep(&in, " ");
    strcpy(&comm[0],&first);
    strcpy(&comm[1],&in);
}
//function to get current directory at the begining
void setup_environment(){
    char cwd[500];
    chdir(getcwd(cwd, sizeof(cwd)));
}
//function to execute built in commands in shell
void execute_shell_builtin(char** com){
    if(strcmp(com[0],"cd")==0){
        if(com[1][0]=='$'){replacevar(com);}
        chdir(com[1]);
    }
    else if(strcmp(com[0],"exit")==0){
        exit(0);
    }else{
        char* first =strsep(&com[1],"=");
        strcpy(&variables[ind],&first);
        strcpy(&values[ind],&com[1]);
        ind++;
    }
}
//function to replace the variable with its value
void replacevar(char** com){
    int index=0;
    for(int i=0;i<ind;i++){
        if(com[1][1]==variables[i][0]){
            index = i;
        }
    }
    strcpy(&com[1],&values[index]);
}
//function to execute commands which are not built in
void execute_command(char** com){
    int status;
    pid_t child_id = fork();
    if(child_id == 0){
        if(com[1]!=NULL){
        if(com[1][0]=='$'){replacevar(com);}
        char* com1[10];
        strcpy(&com1[0],&com[0]);
        for(int i=1;i<100;i++){
            com1[i] = strsep(&com[1], " ");
            if(com1[i]==NULL){break;}
        }
        if(execvp(com[0],com1)<0){printf("\ncommand is not recognised!");}
        exit(0);
        }else{
            if(execvp(com[0],com)<0){printf("\ncommand is not recognised!");}
            exit(0);
        }

    }else{
        if (waitpid(child_id, &status, WNOHANG|WUNTRACED) == -1) {
              perror("waitpid error");
              exit(EXIT_FAILURE);
        }
        return;
    }
}
//function to reap zombie processes and log killed processes in log file
int on_child_exit(){
    wait(NULL);
    FILE *log = fopen("/home/mahmoud/Desktop/lab1/logfile.txt", "a");
    fprintf(log, "child terminated\n");
    fclose(log);
    return 1;
}
//main loop of the shell
void shell(){
    char in[100];
    char* comm[3]={NULL,NULL,NULL};
    int flag=1;
    while(1){
        if(flag==0){signal(SIGCHLD,on_child_exit());}
        flag=0;
        comm[0]=NULL;
        comm[1]=NULL;
        comm[2]=NULL;
        printd();
        getcomm(in,comm);
        if(comm[1]!=NULL){
        if(strcmp(comm[1],"&")==0 ){flag=1;}}
        if(strcmp(comm[0],"cd")==0 || strcmp(comm[0],"exit")==0|| strcmp(comm[0],"export")==0){
            flag=1;
            execute_shell_builtin(comm);
        }else{
            execute_command(comm);
        }
    }
}
int main()
{
    setup_environment();
    shell();
}
