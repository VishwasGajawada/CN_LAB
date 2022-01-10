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

/*** for pthread ***/
#include <pthread.h>

int main(){
    FILE *f = popen("./p2.exe","r");
    int fd = fileno(f);

    
}

// gcc p2.c -o p2.exe && gcc p1.c -o p1.exe && ./p1.exe