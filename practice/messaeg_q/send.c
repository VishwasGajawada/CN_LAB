#include <stdio.h>
/* for ftok */
#include <sys/ipc.h>
/* for msgget, msgsnd, msgrcv */
#include <sys/msg.h>
#include <string.h>

struct msgbuf {
    long mtype;
    char mtext[100];
};

int main() {
    struct msgbuf msg;
    int msqid;
    key_t key;

    key = ftok("send.c", 'a');
    if (key == -1) {
        perror("ftok error");return 1;
    }

    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {
        perror("msgget error");return 1;
    }
    msg.mtype = 1;
    printf("Enter messages: \n");

    while (fgets(msg.mtext, 100, stdin) != NULL) {
        int x = strlen(msg.mtext);
        msg.mtext[x - 1] = '\0';
        // printf("'%s' %d %d\n", msg.mtext,x, (int)strlen(msg.mtext));
        if (msgsnd(msqid, &msg, strlen(msg.mtext), 0) == -1) {
            perror("msgsnd error");
            return 1;
        }
    }
    printf("send.c : done sending messages.\n");

    return 0;
}