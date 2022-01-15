
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
    sem_t *s34 = sem_open("S34", O_CREAT, 0666, 0);
    sem_t *s41= sem_open("S41", O_CREAT, 0666, 0);

    if(s34 == SEM_FAILED || s41 == SEM_FAILED){
        perror("sem_open");
        return 1;
    }

    char c;
    while(1){
        printf("I am P4. I am waiting for Semaphore S34.\n");
        sem_wait(s34);
        printf("I got semaphore S34 signalling from P3.\n");
        printf("Enter any character to sem-signal(S41) :");
        scanf(" %c", &c);
        printf("I am signalling semaphore signal of S41.\n\n");
        sem_post(s41);
    }
    return 0;
}
// gcc p1.c -lpthread -o p1.exe