// run both p1 and p2

#include<stdio.h>
#include<unistd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>

#include <errno.h>
#include <string.h>

// https://unix.stackexchange.com/questions/564098/named-pipe-buffer-after-process-end

int main(){
    char* myFIFO = "/tmp/channel";

    if (mkfifo(myFIFO, 0666) == -1) {
        printf("mkfifo error: ");
        fflush(stdout);
        perror(strerror(errno));
        if(errno != EEXIST){return 1;}
    }
    int wffd = open(myFIFO, O_WRONLY);
    printf("Came here\n");
    if (wffd == -1) {
        printf("open error\n");
        perror(strerror(errno));
        close(wffd);
        unlink(myFIFO);
        return 1;
    }
    printf("p1 writes hello\n");
    write(wffd, "hello", 6);
    close(wffd);
}