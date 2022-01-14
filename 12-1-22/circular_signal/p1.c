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
int p2pid;

int sigusr1_count = 0;
void sigusr1_handler(int signo, siginfo_t *info, void *context) {
    sigusr1_count++;
    if (sigusr1_count == 1) {
        p4pid = info->si_pid;
        return;
    }
    // else if(sigusr1_count>=2 && sigusr1_count<=4){
    //     // circular signal
    // }
}

int main(){
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sigusr1_handler;

    sigaction(SIGUSR1, &act, NULL);

    int p1pid = getpid();
    struct msgbuf msg;
    int msqid;
    key_t key;

    key = ftok("p1.c", 'b');
    if (key == -1) {perror("ftok error");return 1;    }

    msqid = msgget(key, 0666 | IPC_CREAT);
    if (msqid == -1) {perror("msgget error");return 1;}
    
    // p1 recieves message and finds pid2
    if (msgrcv(msqid, &msg, sizeof(msg), 2, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    p2pid = atoi(msg.mtext);
    // printf("p2pid: %d\n", p2pid);
    // fflush(stdout);

    // sends SIGUSR1 to p2
    kill(p2pid, SIGUSR1);

    // wait for p4 signal and store its p4pid
    pause();
    // printf("p4pid: %d\n", p4pid);
    // fflush(stdout);

    printf("p1pid: %d\n", p1pid);
    printf("p2pid: %d\n", p2pid);
    printf("p4pid: %d\n\n", p4pid);
    fflush(stdout);

    sleep(5);
    // circular signalling p1->p2->p3->p4->p1 3 times
    // printf("circular signalling 3 times\n");
    // fflush(stdout);
    // for(int i=0;i<3;i++){
    //     kill(p2pid, SIGUSR1);
    //     write(STDOUT_FILENO, "P1 -> P2\n", 9);
    //     // send next ciruclar signal after this round finishes
    //     pause();
    // }

    return 0;
}