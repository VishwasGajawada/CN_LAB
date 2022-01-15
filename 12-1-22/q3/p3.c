
#include <stdio.h>
#include <unistd.h>
/* for semaphores */
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

int main(){
    key_t key = ftok("p1.c",'a');
    int s23 = semget(key, 1, 0666 | IPC_CREAT);
    int s34 = semget(key, 1, 0666 | IPC_CREAT);

    struct sembuf P[1] = {{ .sem_num = 0, .sem_op = -1, .sem_flg = 0}};
    struct sembuf V[1] = {{ .sem_num = 0, .sem_op = 1, .sem_flg = 0}};

    char c;
    while(1){
        printf("I am P3. I am waiting for Semaphore S23.\n");
        semop(s23, P, 1);
        printf("I got semaphore S23 signalling from P2.\n");
        printf("Enter any character to sem-signal(S34) : ");
        scanf(" %c", &c);
        printf("I am signalling semaphore signal of S34.\n\n");
        semop(s34, V, 1);
    }
    return 0;
}
// gcc p1.c -lpthread -o p1.exe