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

int p4pid=-1;
int p2pid;
int p3pid;
int grouppid;

int sigusr1_count = 0;
void sigusr1_handler(int signo, siginfo_t *info, void *context) {
    sigusr1_count++;
    if (sigusr1_count == 1) {
        p4pid = info->si_pid;
        return;
    }
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

    // sends SIGUSR1 to p2
    kill(p2pid, SIGUSR1);

    // wait for p4 signal and store its p4pid
    while(p4pid == -1)
        pause();

    // p1 recieves 3 message of type 18 and finds p2pid, p3pid, p4pid
    if (msgrcv(msqid, &msg, sizeof(msg), 18, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    if (msgrcv(msqid, &msg, sizeof(msg), 18, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    p3pid = atoi(msg.mtext);
    if (msgrcv(msqid, &msg, sizeof(msg), 18, 0) == -1) {
        perror("msgrcv error");
        return 1;
    }
    p4pid = atoi(msg.mtext);

    // P1 forms grouppid of all pids
    // using pid of p1 as grouppid
    grouppid = p1pid;
    if(setpgid(p1pid, grouppid) == -1){perror("setgpid1 error");};
    if(setpgid(p2pid, grouppid) == -1){perror("setgpid2 error");};
    if(setpgid(p3pid, grouppid) == -1){perror("setgpid3 error");};
    if(setpgid(p4pid, grouppid) == -1){perror("setgpid4 error");};

    printf("p1pid: %d\n", p1pid);
    printf("p2pid: %d\n", p2pid);
    printf("p3pid: %d\n", p3pid);
    printf("p4pid: %d\n", p4pid);
    printf("grouppid: %d\n\n", grouppid);
    fflush(stdout);

    // P1 sends group pid to P2,P3,P4
    msg.mtype = 2;
    sprintf(msg.mtext, "%d", grouppid);
    if(msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd error");
        return 1;
    }
    msg.mtype = 3;
    if(msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd error");
        return 1;
    }
    msg.mtype = 4;
    if(msgsnd(msqid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd error");
        return 1;
    }


    // // send sigusr2 to group
    // killpg(grouppid, SIGUSR2);

    char temp[100];
    scanf("%s", temp);

    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        return 1;
    }

    printf("Message Queue removed\n");
    return 0;
}