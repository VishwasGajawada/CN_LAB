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
int p2pid=-1;
int grouppid;

int sigusr1_count = 0;
int sigusr2_count = 0;

void sigusr1_handler(int signo, siginfo_t *info, void *context) {
    sigusr1_count++;
    if (sigusr1_count == 1) {
        p2pid = info->si_pid;
        return;
    }
}

void sigusr2_handler(int signo){
    sigusr2_count++;
    kill(p4pid, SIGUSR2);
    kill(getpid(), SIGINT);
}

int main(){
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sigusr1_handler;

    sigaction(SIGUSR1, &act, NULL);
    signal(SIGUSR2, sigusr2_handler);

    int p3pid = getpid();
    struct msgbuf msg;
    int msqid;
    key_t key;

    key = ftok("p1.c", 'b');
    if (key == -1) {perror("ftok error");return 1;    }

    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {perror("msgget error");return 1;}
    
    // p3 sends message containing pid as p3pid, with type value 3
    msg.mtype = 3;
    sprintf(msg.mtext, "%d", p3pid);
    if(msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd error");
        return 1;
    }

    // wait for p2 signal and store its p2pid
    while(p2pid == -1)
        pause();

    // p3 sends message containing pid as p3pid, with type value 18
    msg.mtype = 18;
    sprintf(msg.mtext, "%d", p3pid);
    if(msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd error");
        return 1;
    }

    // p3 recieves message from msq of type4 and finds p4pid
    if(msgrcv(msqid, &msg, sizeof(msg.mtext), 4, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    p4pid = atoi(msg.mtext);


    // p3 sends signal USR1 to p4
    kill(p4pid, SIGUSR1);
    
    // p3 recieves message from msq and notes grouppid
    if(msgrcv(msqid, &msg, sizeof(msg.mtext), 3, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    grouppid = atoi(msg.mtext);

    printf("p3pid: %d\n", p3pid);
    // printf("p2pid: %d\n", p2pid);
    // printf("p4pid: %d\n", p4pid);
    // printf("grouppid: %d\n\n", grouppid);
    fflush(stdout);


    // // send sigusr2 to group
    // killpg(grouppid, SIGUSR2);

    // sleep(5);
    return 0;
}