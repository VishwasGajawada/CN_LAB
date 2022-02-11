#include <stdio.h>
/* for ftok */
#include <sys/ipc.h>
/* for msgget, msgsnd, msgrcv */
#include <sys/msg.h>

struct msgbuf {
    long mtype;
    char mtext[100];
};

int main() {
    struct msgbuf msg;
    int msqid;
    key_t key;

    key = ftok("ess.c", 'a');
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
        if (msgrcv(msqid, &msg, sizeof(msg), 1, 0) == -1) {
            perror("msgrcv error");
            return 1;
        }
        printf("'%s'\n", msg.mtext);
    }
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        return 1;
    }
    printf("recieve.c : done recieving messages.\n");

    return 0;
}