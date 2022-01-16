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


void sigusr1_handler(int signo, siginfo_t *info, void *context) {
    p2pid = info->si_pid;
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
    fflush(stdout);


    // // send sigusr2 to group
    killpg(grouppid, SIGUSR2);

    return 0;
}