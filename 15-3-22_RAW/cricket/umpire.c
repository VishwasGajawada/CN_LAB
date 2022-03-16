#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <signal.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include "time.h"
#include <unistd.h> /* unlink */
#include <fcntl.h>
#include <errno.h>

#define debug(x) printf("value of \"%s\" is: %d\n", #x, x);

void handler_error(char *msg, int quit) {
    perror(msg);
    if (quit!=0) exit(EXIT_FAILURE);
}
void sig_bowler_handler(int signo) {}
void sig_batsman_handler(int signo) {}

int serv_uds_listen(char *path);
int cli_uds_conn(char *path) ;
int send_fd(int socket, int fd_to_send);
int recv_fd(int socket) ;

int main(int argc, char *argv[]) {
    /* for bowler to umpire */
    signal(SIGUSR1, sig_bowler_handler);
    /* for batsman to umpire */
    signal(SIGUSR2, sig_batsman_handler);

    int batsman_score[20] = {0};
    int current_batsman = 0;
    int total_score = 0;

    int rsfd1 = socket(AF_INET, SOCK_RAW, 2);
    if(rsfd1 < 0) handler_error("socket", 1);
    int rsfd2 = socket(AF_INET, SOCK_RAW, 3);
    if(rsfd2 < 0) handler_error("socket", 1);

    int bowl_usfd = cli_uds_conn("ump_bowl");
    int busfd = serv_uds_listen("bat_ump");
    int bunsfd = accept(busfd, NULL, NULL);
    printf("Batsman is accepted %d\n", bunsfd);

    debug(bowl_usfd); debug(busfd); debug(bunsfd);

    while(1) {
        /* wait till bowler informs that he balled, using signalling */
        pause();
        printf("Bowler has bowled the ball\n");

        sigset_t sigs;
        sigemptyset(&sigs);
        sigaddset(&sigs, SIGUSR2);
        /* if batsman doesnt reply within 3 seconds, out him */
        struct timespec timeout = { .tv_sec = 3};
        sigprocmask(SIG_BLOCK, &sigs, NULL);

        int sig_num = sigtimedwait(&sigs, NULL, &timeout);
        if (sig_num == SIGUSR2) {
            printf("Batsman has hit the ball\n");
        } else {
            if (errno == EAGAIN) {
                // it was a timeout
                printf("Batsman has missed the ball\n");
                printf("\t\t\t\tThe batsman %d is out\n\n", ++current_batsman);
            } else {
                perror("signal wait failed unexpectedly");
            }


            int fd = recv_fd(bunsfd);
            send_fd(bowl_usfd, fd);
            close(fd);

            continue;
        }

        char buf[100] = {0};
        int len = recvfrom(rsfd1, buf, sizeof(buf), 0, NULL, NULL);
        if(len < 0) handler_error("recvfrom", 1);

        struct iphdr *ip = (struct iphdr *)buf;
        char *c_score = buf + (ip->ihl)*4;
        int score = atoi(c_score);

        if(score%6!=0 && score%4!=0) {
            char caught[100] = {0};
            int len = recvfrom(rsfd2, caught, sizeof(caught), 0, NULL, NULL);
            if(len < 0) handler_error("recvfrom", 1);

            struct iphdr *ip2 = (struct iphdr *)caught;
            printf("%s\n", caught + (ip2->ihl)*4);
            printf("\t\t\t\tThe batsman %d is out\n\n", ++current_batsman);
        }else{
            printf("Number  = %d, ", score);
            if(score%6 == 0) score = 6;
            else if(score%4 == 0) score = 4;
            printf("The hit is %d\n",score);

            total_score += score;
            batsman_score[current_batsman] += score;

            printf("\t\t\t\tbatsman %d score: %d\n", current_batsman+1, batsman_score[current_batsman]);
            printf("\t\t\t\ttotal score: %d\n", total_score);
        }
        int fd = recv_fd(bunsfd);
        send_fd(bowl_usfd, fd);
        close(fd);
    }
    return 0;
}


int serv_uds_listen(char *path) {
    int fd;
    struct sockaddr_un addr;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);
    unlink(path);
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

int send_fd(int socket, int fd_to_send) {
    struct msghdr msg;
    struct cmsghdr *cmsg;
    char buf[1];
    /* at least one vector of one byte must be sent */
    buf[0] = 'F';

    struct iovec io = { .iov_base = buf, .iov_len = sizeof(buf) };
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];
    memset(&ancillary_element_buffer, 0, sizeof(ancillary_element_buffer));

    /* initializs the message header */
    memset(&msg, 0, sizeof(msg));
    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = ancillary_element_buffer;
    msg.msg_controllen = sizeof(ancillary_element_buffer);

    /* initialize a single ancillary data element for fd passing */
    cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int));
    *((int *) CMSG_DATA(cmsg)) = fd_to_send;

    /* send the message with ancillary data */
    return sendmsg(socket, &msg, 0);
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

int recv_fd(int socket) {
    int recieved_fd;
    struct msghdr msg;
    struct cmsghdr *cmsg;
    char buf[1];
    struct iovec io = { .iov_base = buf, .iov_len = sizeof(buf) };
    char ancillary_element_buffer[CMSG_SPACE(sizeof(int))];

    memset(&ancillary_element_buffer, 0, sizeof(ancillary_element_buffer));
    memset(&msg, 0, sizeof(msg));

    msg.msg_iov = &io;
    msg.msg_iovlen = 1;
    msg.msg_control = ancillary_element_buffer;
    msg.msg_controllen = sizeof(ancillary_element_buffer);

    if(recvmsg(socket, &msg, 0) < 0) {
        perror("recvmsg");
        return -1;
    }

    if(buf[0] != 'F') {
        /* this did not originate from send_fd */
        return -1;
    }

    /* iterate ancillary elements */
    for(cmsg = CMSG_FIRSTHDR(&msg); cmsg != NULL; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
        if( (cmsg->cmsg_level == SOL_SOCKET) && cmsg->cmsg_type == SCM_RIGHTS) {
            recieved_fd = *((int *) CMSG_DATA(cmsg));
            return recieved_fd;
        }
    }
    return -1;
}
