#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    printf("1: this is parent processs\n");
    int c = fork();
    if(c>0){
        printf("2: this is parent process\n");
        wait(NULL);
        printf("4: child terminated\n");
    }else{
        printf("3: this is child process\n");
        char *args[] = {NULL};
        execvp("./p2.exe",args);
    }
    
    return 0;
}