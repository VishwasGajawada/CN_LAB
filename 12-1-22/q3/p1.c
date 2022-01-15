
#include <stdio.h>
#include <unistd.h>
/* for semaphores */
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

int main(){

    key_t key = ftok("p1.c",'a');
    int s12 = semget(key, 1, 0666 | IPC_CREAT);
    int s41 = semget(key, 1, 0666 | IPC_CREAT);

    struct sembuf P[1] = {{ .sem_num = 0, .sem_op = -1, .sem_flg = 0}};
    struct sembuf V[1] = {{ .sem_num = 0, .sem_op = 1, .sem_flg = 0}};

    char c;
    while(1){
        printf("I am P1. Enter any character to sem-signal(S12) : ");
        scanf(" %c", &c);
        printf("I am signalling semaphore signal of S12.\n");
        semop(s12, V, 1);
        printf("I am waiting for semaphore S41.\n");
        semop(s41, P, 1);
        printf("I got semaphroe signalling from P4.\n\n");
    }

    return 0;
}
// gcc p1.c -lpthread -o p1.exe