#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
    char *buf;
    size_t sz = 30;
    buf = (char *)malloc(sz* sizeof(char));
    if(getline(&buf,&sz,stdin) == -1){
        printf("Error reading\n");
        return 1;
    };

    printf("Hello p1, recieved this message from you: '%s.'",buf);
    return 0;
}