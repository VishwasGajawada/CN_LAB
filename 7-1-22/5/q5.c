#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
    int fd1[2]; //from parent to child
    int fd2[2]; //from child to parent

    pipe(fd1);
    pipe(fd2);

    int c = fork();
    if(c > 0){
        close(fd1[0]);
        close(fd2[1]);
        
        char pmsg[] = "Hi beta";
        printf("Sending message from parent : %s\n", pmsg);
        write(fd1[1],pmsg,strlen(pmsg)+1);
        close(fd1[1]);

        wait(NULL); //wait till child sends msg

        char cmsg[20];
        read(fd2[0],cmsg,20);
        printf("Recieved message from child : %s\n", cmsg);

        close(fd2[0]);

    }else{
        close(fd1[1]);
        close(fd2[0]);

        char pmsg[20];
        read(fd1[0],pmsg,20);
        printf("Recieved message from parent : %s\n", pmsg);
        close(fd1[0]);

        char cmsg[] = "Good morning papa";
        printf("Sending message from child : %s\n", cmsg);
        write(fd2[1],cmsg,strlen(cmsg)+1);
        close(fd2[1]);
    }
    return 0;
}