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

void sigusr1_handler(int signo, siginfo_t *info, void *context) {
    p3pid = info->si_pid;
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
    fflush(stdout);

    // send sigusr2 to group
    killpg(grouppid, SIGUSR2);

    return 0;
}