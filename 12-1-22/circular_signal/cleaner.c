#include <stdio.h>
#include <unistd.h>
/* for ftok */
#include <sys/ipc.h>
/* for msgget, msgsnd, msgrcv */
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>

struct msgbuf {
    long mtype;
    char mtext[100];
};

int main(){
    struct msgbuf msg;
    int msqid;
    key_t key;

    key = ftok("p1.c", 'b');
    if (key == -1) {perror("ftok error");return 1;    }

    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {perror("msgget error");return 1;}
    
    while(1){
        if (msgrcv(msqid, &msg, sizeof(msg), 0, 0) == -1) {
            perror("msgrcv error");
            return 1;
        }
        printf("'%s'\n", msg.mtext);
    }

    return 0;
}