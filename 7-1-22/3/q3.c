#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    printf("1: this is parent processs\n");
    int c = fork();
    if(c>0){
        printf("2: this is parent process\n");
        int c2 = fork();
        if(c2>0){
            printf("4-1-1: Waiting for Child 1 (p2) process to terminate\n");
            waitpid(c,NULL,0);
            printf("4-1-2: Finished waiting for Child 1 (p2) process \n");
        }else{
            char *args[] = {NULL};
            execvp("./p3.exe",args);    
        }

        printf("4-2-1: Waiting for Child 2 (p3) process to terminate\n");
        waitpid(c2,NULL,0);
        printf("4-2-2: Finished waiting for Child 2 (p3) process\n");

    }else{
        printf("3: this is child 1 process\n");
        char *args[] = {NULL};
        execvp("./p2.exe",args);
        printf("6: after p2 exec system call\n");
    }
    
    return 0;
}