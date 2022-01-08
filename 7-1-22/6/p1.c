#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
    // int fd1[2]; // from p1 to p2
    
    // pipe(fd1);

    int c = fork();

    if(c>0){
        dup2(100,1);
        printf("hi\n");
        // wait(NULL);
    }else{
        dup2(100,0);
        char msg[50];
        scanf("%s",msg);
        printf("in child : %s\n",msg);
    }
    return 0;
}