#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <arpa/inet.h> /* inet_pton */
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int cli_uds_conn(char *path) ;
void sig_A1_handler(int signo);
void sig_A2_handler(int signo);

int sfd;
/* judge */
int main() {
    printf("mypid: %d\n", getpid());
    signal(SIGUSR1, sig_A1_handler);
    signal(SIGUSR2, sig_A2_handler);

    time_t t;
    srand((unsigned) time(&t));
    /* connect to anchor  */
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd <0 )handle_error("socket");

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8085);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if( connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) )
        handle_error("connect");
    printf("connected to anchor\n");

    while(1) {};

    return 0;
}


int cli_uds_conn(char *path) {
    int fd;
    struct sockaddr_un un;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    bzero(&un, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, path);
    if(connect(fd, (struct sockaddr *)&un, sizeof(un)) < 0) {
        perror("connect");
        return -1;
    }

    return fd;
}

void sig_A1_handler(int signo) {
    // ask questions to P and give back score
    printf("Anchor asking me to ask questions and give score\n");
    fflush(stdout);

    int usfd = cli_uds_conn("stage_perform");
    if(usfd < 0) handle_error("cli_uds_conn");
    printf("P connected\n");

    char buf[100];
    while(1) {
        printf("Enter question: ");
        fgets(buf, sizeof(buf), stdin);
        send(usfd, buf, sizeof(buf), 0);
        if(strcmp(buf, "exit\n") == 0) break;
    }

    int score = rand()%10 + 1;
    char score_str[10];
    sprintf(score_str, "%d", score);
    send(sfd, score_str, sizeof(score_str), 0);
    printf("score = %d\n", score);
}
void sig_A2_handler(int signo) {
    // give back score
    printf("Anchor asking me to give score\n");
    fflush(stdout);

    int score = rand()%10 + 1;
    char score_str[10];
    sprintf(score_str, "%d", score);
    send(sfd, score_str, sizeof(score_str), 0);
    printf("score = %d\n", score);
}