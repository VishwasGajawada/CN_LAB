
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
    char* x = shmat(shmidx, NULL, 0);
    char* y = shmat(shmidy, NULL, 0);

    /* cleaning */
    if (sem_close(s1) != 0){
        perror("sem_close s1"); ;
    }
    if (sem_unlink("S1") < 0){
        perror("sem_unlink S1"); ;
    }
    if (sem_close(s2) != 0){
        perror("sem_close s2"); ;
    }
    if (sem_unlink("S2") < 0){
        perror("sem_unlink S2"); ;
    }

    // detach from shared memory
    if (shmdt(x) < 0) { perror("shmdt"); }
    if (shmdt(y) < 0) { perror("shmdt"); }

    if(shmctl(shmidx, IPC_RMID, NULL) < 0) { printf("already removed\n"); }
    if(shmctl(shmidy, IPC_RMID, NULL) < 0) { printf("already removed\n"); return 1; }
    return 0;
}
// gcc p1.c -lpthread -o p1.exe