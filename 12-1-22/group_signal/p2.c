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

int p3pid;
int p1pid=-1;
int grouppid;


void sigusr1_handler(int signo, siginfo_t *info, void *context) {
    p1pid = info->si_pid;
    return;
}

void sigusr2_handler(int signo, siginfo_t *info, void *context){
    int sent_pid = info->si_pid;
    printf("signal recieved from %d\n", sent_pid);

}

int main(){
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sigusr1_handler;
    struct sigaction act2;
    act2.sa_flags = SA_SIGINFO;
    act2.sa_sigaction = sigusr2_handler;

    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGUSR2, &act2, NULL);

    int p2pid = getpid();
    struct msgbuf msg;
    int msqid;
    key_t key;

    key = ftok("p1.c", 'b');
    if (key == -1) {perror("ftok error");return 1;    }

    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {perror("msgget error");return 1;}
    
    // p2 sends message containing pid as p2pid, with type value 2
    msg.mtype = 2;
    sprintf(msg.mtext, "%d", p2pid);
    if(msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd error");
        return 1;
    }

    // wait for p1 signal and store its p1pid
    while(p1pid == -1)
        pause();

    // p2 recieves message from ms1 and notes p3pid
    if(msgrcv(msqid, &msg, sizeof(msg.mtext), 3, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    p3pid = atoi(msg.mtext);


    // p2 sends message containing pid as p1pid, with type value 1
    msg.mtype = 1;
    sprintf(msg.mtext, "%d", p1pid);
    if(msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd error");
        return 1;
    }
    // p2 sends message containing pid as p2pid, with type value 18
    msg.mtype = 18;
    sprintf(msg.mtext, "%d", p2pid);
    if(msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd error");
        return 1;
    }


    // p2 sends a SIGUSR1 to p3
    kill(p3pid, SIGUSR1);


    // p2 recieves message from msq and notes grouppid
    if(msgrcv(msqid, &msg, sizeof(msg.mtext), 2, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    grouppid = atoi(msg.mtext);

    printf("p2pid: %d\n", p2pid);
    fflush(stdout);

    // send sigusr2 to group
    killpg(grouppid, SIGUSR2);

    return 0;
}