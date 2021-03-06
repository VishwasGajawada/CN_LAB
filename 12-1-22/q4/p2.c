
#include <stdio.h>
#include <unistd.h>
/* for semaphores */
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
/* for shared memory */
#include <sys/shm.h>
/* contains definitions for ipc structures */
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

int main(){
    /* shared memory initialization */
    key_t keyx = ftok("p1.c", 'a');
    key_t keyy = ftok("p2.c", 'a');
    int shmidx = shmget(keyx, sizeof(char), 0666 | IPC_CREAT);
    if (shmidx < 0) { perror("shmget"); return 1; }

    int shmidy = shmget(keyy, sizeof(char), 0666 | IPC_CREAT);
    if (shmidy < 0) { perror("shmget"); return 1; }

    sem_t *s1 = sem_open("S1", O_CREAT, 0666, 0);
    sem_t *s2 = sem_open("S2", O_CREAT, 0666, 0);

    if(s1 == SEM_FAILED || s2 == SEM_FAILED){
        perror("sem_open");
        return 1;
    }

    // attach to shared memory
    int* x = (int *)shmat(shmidx, NULL, 0);
    int* y = (int *)shmat(shmidy, NULL, 0);

    *x = 1;
    *y = 1;

    int limit = 5;
    while(1){
        printf("\n");
        if((*x) >= limit){
            printf("length of x exceeded %d, stopping\n", limit);
            break;
        }
        printf("I am waiting for S1\n");
        sem_wait(s1);

        printf("I am reading shm x\n");
        // read(x)
        printf("x = %d\n", *x);

        // make y = x+1
        *y = *x+1;

        printf("Enter any char to signal S2 : ");
        char c[10];
        scanf("%s", c);

        // signal(S2)
        sem_post(s2);
    }

    sleep(1);
    printf("x=%d and y=%d\n", *x, *y);

    /* cleaning */
    if (sem_close(s1) != 0){
        perror("sem_close s1"); ;
    }
    if (sem_unlink("S1") < 0){
        // printf("already removed\n");
    }
    if (sem_close(s2) != 0){
        perror("sem_close s2"); ;
    }
    if (sem_unlink("S2") < 0){
        // printf("already removed\n");
    }

    // detach from shared memory
    if (shmdt(x) < 0) { perror("shmdt"); }
    if (shmdt(y) < 0) { perror("shmdt"); }

    if(shmctl(shmidx, IPC_RMID, NULL) < 0) { /* printf("already removed\n"); */ }
    if(shmctl(shmidy, IPC_RMID, NULL) < 0) { /* printf("already removed\n"); */ }
    return 0;
}
// gcc p2.c -lpthread -o p2.exe