/* p1 will write to x by appending 'A', p2 will write to y by appending B*/

#include <stdio.h>
/* for semaphores */
#include <sys/sem.h>
/* for shared memory */
#include <sys/shm.h>
/* contains definitions for ipc structures */
#include <sys/ipc.h>

int main(){
    key_t keyx = ftok("x", 'a');    
    key_t keyy = ftok("y", 'a');    
    int shmidx = shmget(keyx, sizeof(char), 0666 | IPC_CREAT);
    if(shmidx < 0){perror("shmget"); return 1;}

    int shmidy = shmget(keyx, sizeof(char), 0666 | IPC_CREAT);
    if(shmidy < 0){perror("shmget"); return 1;}

    // attach to shared memory
    char *x = shmat(shmidx, NULL, 0);
    char *y = shmat(shmidy, NULL, 0);


    


    // detach from shared memory
    if(shmdt(x) < 0){perror("shmdt");}
    if(shmdt(y) < 0){perror("shmdt"); return 1;}
    return 0;
}