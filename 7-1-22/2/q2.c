#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(){
    printf("1: this is parent processs\n");
    int c = fork();
    if(c>0){
        printf("2: this is parent process\n");
        int c2 = fork();
        int pid1,pid2;
        if(c2>0){
            pid1 = wait(NULL);
        }else{
            char *args[] = {NULL};
            execvp("./p3.exe",args);    
        }
        pid2 = wait(NULL);
        if(pid1 == c){
            printf("4: child 1(p2) terminated first, child 2(p3) terminated second\n");
        }else if(pid1 == c2){
            printf("4: child 2(p3) terminated first, child 1(p2) terminated second\n");
        }

    }else{
        printf("3: this is child 1 process\n");
        char *args[] = {NULL};
        execvp("./p2.exe",args);
        printf("6: after p2 exec system call\n");
    }
    
    return 0;
}