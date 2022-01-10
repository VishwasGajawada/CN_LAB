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
    char *myFIFO = "/tmp/channel";
    if (mkfifo(myFIFO, 0666) == -1) {
        printf("mkfifo error: ");
        fflush(stdout);
        perror(strerror(errno));
        if(errno != EEXIST){return 1;}
    }
    int rffd = open(myFIFO, O_RDONLY);
    if (rffd == -1) {
        printf("open error\n");
        perror(strerror(errno));
        return 1;
    }
    char buf[10]={0};
    read(rffd, buf, 5);
    printf("p2 read %s\n", buf);
    close(rffd);
}