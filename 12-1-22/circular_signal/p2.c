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
int p1pid;

int sigusr1_count = 0;
int sigusr2_count = 0;

void sigusr1_handler(int signo, siginfo_t *info, void *context) {
    sigusr1_count++;
    if (sigusr1_count == 1) {
        p1pid = info->si_pid;
        return;
    }
    else if(sigusr1_count>=2 && sigusr1_count<=4){
        // circular signal
        kill(p3pid, SIGUSR1);
        write(STDOUT_FILENO, "P1 -> P2 recieved\n", 18);
        if(sigusr1_count == 4)write(STDOUT_FILENO,"\n",1);
    }
}

void sigusr2_handler(int signo){
    sigusr2_count++;
    if(sigusr2_count>=1 && sigusr2_count<=3){
        kill(p1pid, SIGUSR2);
        write(STDOUT_FILENO, "P3 -> P2 recieved\n", 18);
    }else{
        kill(p3pid, SIGUSR2);
        kill(getpid(), SIGINT);
    }
}

int main(){
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sigusr1_handler;

    sigaction(SIGUSR1, &act, NULL);
    signal(SIGUSR2, sigusr2_handler);

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


    // p2 sends a SIGUSR1 to p3

    kill(p3pid, SIGUSR1);
    printf("p2pid: %d\n", p2pid);
    printf("p1pid: %d\n", p1pid);
    printf("p3pid: %d\n\n", p3pid);
    fflush(stdout);

    // sleep(5);

    // circular signal
    // reverse circular signal

    while(1){}

    return 0;
}