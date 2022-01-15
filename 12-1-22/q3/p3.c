
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
    sem_t *s23 = sem_open("S23", O_CREAT, 0666, 0);
    sem_t *s34= sem_open("S34", O_CREAT, 0666, 0);

    if(s23 == SEM_FAILED || s34 == SEM_FAILED){
        perror("sem_open");
        return 1;
    }

    
    char c;
    while(1){
        printf("I am P3. I am waiting for Semaphore S23.\n");
        sem_wait(s23);
        printf("I got semaphore S23 signalling from P2.\n");
        printf("Enter any character to sem-signal(S34) : ");
        scanf(" %c", &c);
        printf("I am signalling semaphore signal of S34.\n\n");
        sem_post(s34);
    }
    return 0;
}
// gcc p1.c -lpthread -o p1.exe