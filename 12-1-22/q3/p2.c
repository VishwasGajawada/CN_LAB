
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
    sem_t *s23= sem_open("S23", O_CREAT, 0666, 0);

    if(s12 == SEM_FAILED || s23 == SEM_FAILED){
        perror("sem_open");
        return 1;
    }

    char c;
    while(1){
        printf("I am P2. I am waiting for Semaphore S12.\n");
        sem_wait(s12);
        printf("I got semaphore S12 signalling from P1.\n");
        printf("Enter any character to sem-signal(S23) : ");
        scanf(" %c", &c);
        printf("I am signalling semaphore signal of S23.\n\n");
        sem_post(s23);
    }
    return 0;
}
// gcc p1.c -lpthread -o p1.exe