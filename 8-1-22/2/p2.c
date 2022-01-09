#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char **argv){

    int pipeOutCopy = dup(1);
    int pipeInCopy = dup(0);

    int stdOutCopy = atoi(argv[1]);
    int stdInCopy = atoi(argv[2]);

    char *buf;
    size_t sz = 30;
    buf = (char *)malloc(sz* sizeof(char));
    if(getline(&buf,&sz,stdin) == -1){
        printf("Error reading\n");
        return 1;
    };
    dup2(stdOutCopy,1);
    // this printg statement sends output to screen i.e the original stdout
    printf("hello screen\n");
    // fflush(1);

    dup2(pipeOutCopy, 1);
    // this printf statement sends output to the pipe that p1 is reading from
    printf("Hello p1, recieved this message from you: '%s'",buf);



    return 0;
}