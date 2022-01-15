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

int p1pid;
int p3pid=-1;
int grouppid;

int sigusr1_count = 0;
int sigusr2_count = 0;
void sigusr1_handler(int signo, siginfo_t *info, void *context) {
    sigusr1_count++;
    if (sigusr1_count == 1) {
        p3pid = info->si_pid;
        return;
    }
}
void sigusr2_handler(int signo){

    sigusr2_count++;
    kill(getpid(), SIGINT);
}

int main(){
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sigusr1_handler;

    sigaction(SIGUSR1, &act, NULL);
    signal(SIGUSR2, sigusr2_handler);

    int p4pid = getpid();
    struct msgbuf msg;
    int msqid;
    key_t key;

    key = ftok("p1.c", 'b');
    if (key == -1) {perror("ftok error");return 1;    }

    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {perror("msgget error");return 1;}
    
    // p4 sends message containing pid as p4pid, with type value 4
    msg.mtype = 4;
    sprintf(msg.mtext, "%d", p4pid);
    if(msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd error");
        return 1;
    }

    // wait for p3 signal and store its p3pid
    while(p3pid == -1)
        pause();
    
    // p4 sends message containing pid as p4pid, with type value 18
    msg.mtype = 18;
    sprintf(msg.mtext, "%d", p4pid);
    if(msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd error");
        return 1;
    }

    // p4 recieves message from msq of type1 and finds p1pid
    if(msgrcv(msqid, &msg, sizeof(msg.mtext), 1, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    p1pid = atoi(msg.mtext);

    // p4 sends signal USR1 to p1
    kill(p1pid, SIGUSR1);

    // p4 recieves message from msq and notes grouppid
    if(msgrcv(msqid, &msg, sizeof(msg.mtext), 4, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    grouppid = atoi(msg.mtext);

    printf("p4pid: %d\n", p4pid);
    // printf("p3pid: %d\n", p3pid);
    // printf("p1pid: %d\n", p1pid);
    // printf("grouppid: %d\n\n", grouppid);
    fflush(stdout);

    // // send sigusr2 to group
    // killpg(grouppid, SIGUSR2);

    // sleep(5);

    return 0;
}