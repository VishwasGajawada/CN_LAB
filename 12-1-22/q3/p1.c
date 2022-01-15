
#include <stdio.h>
#include <unistd.h>
/* for semaphores */
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

int main(){
    sem_t *s12 = sem_open("S12", O_CREAT, 0666, 0);
    sem_t *s41= sem_open("S41", O_CREAT, 0666, 0);

    if(s12 == SEM_FAILED || s41 == SEM_FAILED){
        perror("sem_open");
        return 1;
    }


    char c;
    while(1){
        printf("I am P1. Enter any character to sem-signal(S12) : ");
        scanf(" %c", &c);
        printf("I am signalling semaphore signal of S12.\n");
        sem_post(s12);
        printf("I am waiting for semaphore S41.\n");
        sem_wait(s41);
        printf("I got semaphroe signalling from P4.\n\n");
    }

    return 0;
}
// gcc p1.c -lpthread -o p1.exe