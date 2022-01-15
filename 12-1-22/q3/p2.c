
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
    int s23 = semget(key, 1, 0666 | IPC_CREAT);

    struct sembuf P[1] = {{ .sem_num = 0, .sem_op = -1, .sem_flg = 0}};
    struct sembuf V[1] = {{ .sem_num = 0, .sem_op = 1, .sem_flg = 0}};

    char c;
    while(1){
        printf("I am P2. I am waiting for Semaphore S12.\n");
        semop(s12, P, 1);
        printf("I got semaphore S12 signalling from P1.\n");
        printf("Enter any character to sem-signal(S23) : ");
        scanf(" %c", &c);
        printf("I am signalling semaphore signal of S23.\n\n");
        semop(s23, V, 1);
    }
    return 0;
}
// gcc p1.c -lpthread -o p1.exe