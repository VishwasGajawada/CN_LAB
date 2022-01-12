#include <stdio.h>
/* for ftok */
#include <sys/ipc.h>
/* for msgget, msgsnd, msgrcv */
#include <sys/msg.h>
#include <string.h>
/* for atoi */
#include <stdlib.h>

struct msgbuf {
    long mtype;
    char mtext[1024];
};

int main(){

    struct msgbuf msg;
    struct msgbuf msg2; // for sending
    int msqid;
    key_t key;

    key = ftok("p1.c", 'a');
    if (key == -1) {
        perror("ftok error");
        return 1;
    }

    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("msgget error");
        return 1;
    }

    while (1) {
        if (msgrcv(msqid, &msg, sizeof(msg.mtext), 1, 0) == -1) {
            perror("msgrcv error");
            return 1;
        }
        int from_process = msg.mtext[0] - '0';
        char *pid_string = msg.mtext + 2;
        int pid = atoi(pid_string);
        printf("'%s' from P%d\n", pid_string, from_process);
        fflush(stdout);

        msg.mtype = pid;
        sprintf(msg.mtext, "message from P1 to P%d ", from_process);
        if (msgsnd(msqid, &msg, strlen(msg.mtext)+1, 0) == -1) {
            perror("msgsnd error");
            return 1;
        }
    }
    return 0;
}

// p2pid, p3pid, p4pid, 