// server
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
    unlink("/tmp/channel");
    // if(mkfifoat(AT_FDCWD, "./channel",0666) == -1){
    
    if(mkfifo("/tmp/channel",0666) == -1){
        perror(strerror(errno));
        printf("mkfifo error\n");
        unlink("/tmp/channel");
        return 1;
    }
    int wffd = open("/tmp/channel", O_WRONLY);
    if(wffd == -1){
        perror(strerror(errno));
        printf("open error\n");
        unlink("/tmp/channel");
        return 1;
    }
    write(wffd, "hello", 5);
    close(wffd);

    int rffd = open("/tmp/channel", O_RDONLY);
    if(rffd == -1){
        perror(strerror(errno));
        printf("open error\n");
        unlink("/tmp/channel");
        return 1;
    }
    char buf[10];
    read(rffd, buf, 10);
    close(rffd);
    printf("%s\n", buf);
}

// gcc basic.c -o basic.exe && ./basic.exe 