#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
    int fd[2];
    pipe(fd);

    int c = fork();
    if(c > 0){
        close(fd[0]);
        
        char msg[] = "Hi child";
        printf("Sending message from parent : %s\n", msg);
        write(fd[1],msg,strlen(msg)+1);
        wait(NULL);

        close(fd[1]);
    }else{
        close(fd[1]);

        char msg[20];
        read(fd[0],msg,20);
        printf("Recieved message from parent : %s\n", msg);

        close(fd[0]);
    }
    return 0;
}