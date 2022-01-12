// run both p1 and p2

#include<stdio.h>
#include<unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>

#include <errno.h>
#include <string.h>


int main(){
    char* FIFO4 = "/tmp/FIFO4";
    if (mkfifo(FIFO4, 0666) == -1) {
        printf("mkfifo error: ");
        fflush(stdout);
        perror(strerror(errno));
        if(errno != EEXIST){return 1;}
    }
    int wffd = open(FIFO4, O_WRONLY);
    if (wffd == -1) {
        printf("open error\n");
        perror(strerror(errno));
        close(wffd);
        unlink(FIFO4);
        return 1;
    }
    char buff[100];
    while(fgets(buff, 100, stdin) != NULL){
        write(wffd, buff, strlen(buff)+1);
    }

    close(wffd);
}