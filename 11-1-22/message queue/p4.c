#include <stdio.h>
#include <unistd.h>
/* for ftok */
#include <sys/ipc.h>
/* for msgget, msgsnd, msgrcv */
#include <sys/msg.h>
#include <string.h>
#include <sys/types.h>

struct msgbuf {
    long mtype;
    char mtext[1024];
};

int main(){
    int processnumber = 4;
    struct msgbuf msg;
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
    msg.mtype = 1;
    for(int i=0;i<1024;i++)msg.mtext[i] = '\0';
    int pid = getpid();
    char pid_string[10];
    sprintf(pid_string, "%d_%d", processnumber, pid);
    printf("my pid is %d\n", pid);

    strcpy(msg.mtext, pid_string);
    if(msgsnd(msqid, &msg, strlen(msg.mtext)+1, 0) == -1){
        perror("msgsnd error");
        return 1;
    }
    
    char buf[100];
    if(msgrcv(msqid, &msg, sizeof(msg.mtext), pid, 0) == -1){
        perror("msgrcv error");
        return 1;
    }
    printf("%s\n", msg.mtext);

    return 0;
}