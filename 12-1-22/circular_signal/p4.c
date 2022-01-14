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
int p3pid;

int sigusr1_count = 0;
void sigusr1_handler(int signo, siginfo_t *info, void *context) {
    sigusr1_count++;
    if (sigusr1_count == 1) {
        p3pid = info->si_pid;
        return;
    }
    // else if(sigusr1_count>=2 && sigusr1_count<=4){
    //     // circular signal
    //     write(STDOUT_FILENO,"P4 -> P1\n",9);
    //     kill(p1pid, SIGUSR1);
    // }
}

int main(){
    struct sigaction act;
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = sigusr1_handler;

    sigaction(SIGUSR1, &act, NULL);

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
    pause();
    // printf("p3pid: %d\n", p3pid);
    // fflush(stdout);
    
    // p4 recieves message from msq of type1 and finds p1pid
    if(msgrcv(msqid, &msg, sizeof(msg.mtext), 1, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    p1pid = atoi(msg.mtext);
    // printf("p1pid: %d\n", p1pid);
    // fflush(stdout);

    // p4 sends signal USR1 to p1
    kill(p1pid, SIGUSR1);

    printf("p4pid: %d\n", p4pid);
    printf("p3pid: %d\n", p3pid);
    printf("p1pid: %d\n\n", p1pid);
    fflush(stdout);


    sleep(5);
    
    // circular signal
    // printf("circular signalling 3 times\n");
    // fflush(stdout);

    // for(int i=0;i<3;i++){
    //     pause();
    // }

    return 0;
}