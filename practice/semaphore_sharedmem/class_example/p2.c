/* p1 will write to x by appending 'A', p2 will write to y by appending B*/

#include <stdio.h>
#include <unistd.h>
/* for semaphores */
#include <sys/sem.h>
/* for shared memory */
#include <sys/shm.h>
/* contains definitions for ipc structures */
#include <sys/ipc.h>
#include <string.h>
#include <errno.h>

/* to use in semctl inorder to initialze semaphores */
union semun {
    int val;
    struct semid_ds* buf;
    unsigned short* array;
} arg;

// 3 so that we can use 1 and 2 numbers for clarity
#define NUMSEMS 3

int main() {
    /* shared memory initialization */
    key_t keyx = ftok("p1.c", 'a');
    key_t keyy = ftok("p2.c", 'a');
    int shmidx = shmget(keyx, sizeof(char), 0666 | IPC_CREAT);
    if (shmidx < 0) { perror("shmget"); return 1; }

    int shmidy = shmget(keyy, sizeof(char), 0666 | IPC_CREAT);
    if (shmidy < 0) { perror("shmget"); return 1; }

    /* semaphore initialization */
    key_t semkey = ftok("p1.c", 'a');
    int semid = semget(semkey, NUMSEMS, 0666 | IPC_CREAT | IPC_EXCL);
    if (semid < 0) {
        perror("semget");
        if (errno == EEXIST) {
            semid = semget(semkey, NUMSEMS, 0666);
            if (semid < 0) { perror("semget"); return 1; }
        }
        else {
            return 1;
        }
    }

    // attach to shared memory
    char* x = shmat(shmidx, NULL, 0);
    char* y = shmat(shmidy, NULL, 0);

    arg.val = 1;
    /* initialize semaphore1 to 1 */
    if (semctl(semid, 1, SETVAL, arg) < 0) { perror("semctl"); return 1; }
    arg.val = 1;
    /* initialize semaphore2 to 1 */
    if (semctl(semid, 2, SETVAL, arg) < 0) { perror("semctl"); return 1; }


    struct sembuf operations[1];
    strcpy(x, "");
    strcpy(y, "");
    printf("all initialized correctly\n\n");
    sleep(3);
    int limit = 5;
    char localcopy[100] = {0};  // of x
    while (1) {
        if (strlen(x) > limit) {
            printf("length of x exceeded %d characters\n", limit);
            break;
        }
        sleep(1);
        
        // like sem_wait(s1)
        operations[0].sem_num = 1; // operate on sem1
        operations[0].sem_op = 0; // wait for sem1 to be 0
        operations[0].sem_flg = 0; // no flags
        if (semop(semid, operations, 1) < 0) { perror("semop1"); return 1; }

        // read(x)
        strcpy(localcopy, x);

        arg.val = 1;
        /* set semaphore1 back to 1 */
        if (semctl(semid, 1, SETVAL, arg) < 0) { perror("semctl1"); return 1; }
        printf("\tread from x = %s\n", localcopy);

        // write(y)
        strcpy(y, localcopy);
        strcat(y, "B");
        printf("wrote to y = %s\n\n", y);

        // like sem_post(s2)
        operations[0].sem_num = 2; // operate on sem2
        operations[0].sem_op = -1; // make sem2 0 so that p1 can wread y
        operations[0].sem_flg = 0; // no flags
        if (semop(semid, operations, 1) < 0) { perror("semop2"); return 1; }
    }

    sleep(2);
    printf("x = %s\n", x);
    printf("y = %s\n", y);

    /* cleaning */

    if(semctl(semid, 0, IPC_RMID, NULL) < 0) { printf("already removed\n"); }

    // detach from shared memory
    if (shmdt(x) < 0) { perror("shmdt"); }
    if (shmdt(y) < 0) { perror("shmdt"); }

    if(shmctl(shmidx, IPC_RMID, NULL) < 0) { printf("already removed\n"); }
    if(shmctl(shmidy, IPC_RMID, NULL) < 0) { printf("already removed\n"); return 1; }
    return 0;
}

// wait(s1) (semop 0)
// read(x)
// - post(s1) (set s1 = 1)
// write(y) 
// post(s2)  (semop -1)