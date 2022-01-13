/** writer **/

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

    // write to shared memory
    /* this works */
    // char *s = str;
    // *s++ = 'A';
    // for (char c = 'a'; c <= 'z'; c++)
    //     *s++ = c;
    // *s = 0;

    /* this also works */
    scanf("%s", str);

    /*** getline doesnt work because *str is reassigned to some diff address, so shmdt() throws Invalid argument error ***/
    // size_t len = 0;
    // getline(&str,&len,stdin);

    // detach from shared memory
    if(shmdt(str) < 0){perror("shmdt"); return 1;}

    return 0;
}