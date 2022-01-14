#include <stdio.h>
#include <unistd.h>
#include <signal.h>
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

int p4pid;
int p2pid;

int main(){
    int p1pid = getpid();
    struct msgbuf msg;
    int msqid;
    key_t key;

    key = ftok("p1.c", 'a');
    if (key == -1) {perror("ftok error");return 1;    }

    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {perror("msgget error");return 1;}
    
    // p1 recieves message and finds pid2
    if (msgrcv(msqid, &msg, sizeof(msg), 2, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    p2pid = atoi(msg.mtext);

    // sends SIGUSR1 to p2
    kill(p2pid, SIGUSR1);

    printf("p1pid: %d\n", p1pid);
    printf("p2pid: %d\n", p2pid);


    return 0;
}