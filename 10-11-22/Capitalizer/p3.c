// run both p1 and p2

#include<stdio.h>
#include<unistd.h>

/*** for mkfifo() ***/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>

/** for errno **/
#include <errno.h>
#include <string.h>

/** for toupper() **/
#include <ctype.h>

// https://unix.stackexchange.com/questions/564098/named-pipe-buffer-after-process-end

int main(){
    char* path = "green";

    if (mkfifo(path, 0666) == -1) {
        if(errno != EEXIST){
            printf("mkfifo error: ");
            perror(strerror(errno));
            fflush(stdout);
            return 1;
        }
    }
    int wffd = open(path, O_WRONLY);
    while(1){
        char buf[1024]={0};
        scanf("%s", buf);
        // now capitalize buf
        for(int i=0; i<strlen(buf); i++){
            buf[i] = toupper(buf[i]);
        }
        printf("in p3 : '%s'\n", buf);
        fflush(stdout);
        sprintf(buf, "%s_fromP3", buf);
        write(wffd, buf, strlen(buf));
    }
    close(wffd);
}