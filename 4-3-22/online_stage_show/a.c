#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <arpa/inet.h> /* inet_pton */
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>


#define NUM_JUDGES 3
#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int serv_tcp_listen(int port) ;
int pidof(char *name);
void sig_s_handler(int signo);

int jpid[NUM_JUDGES], jnsfd[NUM_JUDGES];


/* anchor */
int main() {
    printf("mypid: %d\n", getpid());
    signal(SIGUSR1, sig_s_handler);

    time_t t;
    srand((unsigned) time(&t));

    int sfd;
    struct sockaddr_in addr;

    sfd = serv_tcp_listen(8085);
    if(sfd < 0) handle_error("serv_tcp_listen");

    for(int i=0; i<NUM_JUDGES; i++) {
        int nsfd;
        struct sockaddr_in caddr;
        socklen_t len = sizeof(caddr);

        nsfd = accept(sfd, (struct sockaddr *)&caddr, &len);
        if(nsfd < 0) handle_error("accept");

        jnsfd[i] = nsfd;
        char name[50] = {0};
        sprintf(name, "./j%d.exe", i+1);
        jpid[i] = pidof(name);
        printf("judge%d.exe pid: %d\n", i+1, jpid[i]);
    }

    while(1) sleep(1);
    return 0;
}

int serv_tcp_listen(int port) {
    int fd;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int opt = 1;
    // Forcefully attaching socket to the port, so that bind already in use error doesnt occur
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        return 1;
    }
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(fd, 5) < 0) {
        perror("listen");
        return -1;
    }

    return fd;
}

int pidof(char *name) {
    char buf[100] = {0};
    sprintf(buf, "pidof %s", name);
    int fd = fileno(popen(buf, "r"));	
    char s[100];
	read(fd, &s, 1000);
	return atoi(s); 
}

void sig_s_handler(int signo) {
    // choose one random judge to ask questions
    // take scores from all judges
    int j = rand() % NUM_JUDGES;
    printf("judge %d asks questions\n", j+1);
    for(int i=0; i<NUM_JUDGES; i++) {
        if(i == j) kill(jpid[i], SIGUSR1);
        else kill(jpid[i], SIGUSR2);
    }

    int scores[NUM_JUDGES];
    for(int i=0; i<NUM_JUDGES; i++) {
        char buf[100] = {0};
        memset(buf, 0, sizeof(buf));
        int n = recv(jnsfd[i], &buf, sizeof(buf), 0);
        if(n <= 0) break;
        scores[i] = atoi(buf);
    }

    // announce scores
    for(int i=0; i<NUM_JUDGES; i++) {
        printf("judge%d score: %d\n", i+1, scores[i]);
    }
}





