// run both p1 and p2

#include<stdio.h>
#include<unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>

#include <errno.h>
#include <string.h>

// gcc server.c -o server.exe

int main(){
    printf("ChatServer\n\n");
    char* fifo1 = "channel1";
    char* fifo2 = "channel2";

    if (mkfifo(fifo1, 0666) == -1) {
        if(errno != EEXIST){
            printf("mkfifo channel1 error: ");
            fflush(stdout);
            perror(strerror(errno));
            return 1;
        }
    }
    if (mkfifo(fifo2, 0666) == -1) {
        if(errno != EEXIST){
            printf("mkfifo channel2 error: ");
            fflush(stdout);
            perror(strerror(errno));
            return 1;
        }
    }
    int wffd = open(fifo1, O_WRONLY);
    int rffd = open(fifo2, O_RDONLY);
    

    char buf[1024] = {0};
    while(1){
        read(rffd, buf, 1024);
        printf("%s\n", buf);
        fflush(stdout);
        write(wffd, buf, 1024);
    }
    close(wffd);
    close(wffd);
}