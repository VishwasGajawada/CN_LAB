/** reader **/

#include <stdio.h>
/* for shared memory */
#include <sys/shm.h>
/* contains definitions for ipc structures */
#include <sys/ipc.h>

int main(){
    key_t key = ftok("p1.c", 'a');
    int shmid = shmget(key, sizeof(char), 0666 | IPC_CREAT);
    if(shmid < 0){perror("shmget"); return 1;}

    // attach to shared memory
    char *str = shmat(shmid, NULL, 0);
    if(str == (char *)-1){perror("shmat"); return 1;}

    printf("contents of sharedmemory = %s\n", str);

    // detach from shared memory
    if(shmdt(str) < 0){perror("shmdt"); return 1;}


    return 0;
}