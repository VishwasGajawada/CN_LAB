
#include <stdio.h>
#include <unistd.h>
/* for semaphores */
#include <sys/sem.h>
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

int main(){
    key_t key = ftok("p1.c",'a');
    int s34 = semget(key, 1, 0666 | IPC_CREAT);
    int s41 = semget(key, 1, 0666 | IPC_CREAT);

    struct sembuf P[1] = {{ .sem_num = 0, .sem_op = -1, .sem_flg = 0}};
    struct sembuf V[1] = {{ .sem_num = 0, .sem_op = 1, .sem_flg = 0}};

    char c;
    while(1){
        printf("I am P4. I am waiting for Semaphore S34.\n");
        semop(s34, P, 1);
        printf("I got semaphore S34 signalling from P3.\n");
        printf("Enter any character to sem-signal(S41) : ");
        scanf(" %c", &c);
        printf("I am signalling semaphore signal of S41.\n\n");
        semop(s41, V, 1);
    }
    return 0;
}
// gcc p1.c -lpthread -o p1.exe