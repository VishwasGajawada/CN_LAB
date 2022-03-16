#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include "time.h"
#include <unistd.h> /* unlink */
#include <fcntl.h>
#include <signal.h>

#define debug(x) printf("value of \"%s\" is: %d\n", #x, x);

void handler_error(char *msg, int quit) {
    perror(msg);
    if (quit!=0) exit(EXIT_FAILURE);
}

int serv_uds_listen(char *path);
int cli_uds_conn(char *path) ;
int send_fd(int socket, int fd_to_send);
int recv_fd(int socket) ;
int pidof(char *name);

int main(int argc, char *argv[]) {
    int umpire_pid = -1;

    /* open raw socket */
    int rsfd = socket(AF_INET, SOCK_RAW, 2);
    if(rsfd < 0) handler_error("socket", 1);

    int bowl_usfd = cli_uds_conn("bat_bowl");
    int ump_usfd = cli_uds_conn("bat_ump");

    debug(bowl_usfd); debug(ump_usfd);

    struct sockaddr_in cli;
    cli.sin_family = AF_INET;
    cli.sin_addr.s_addr = inet_addr("127.0.0.1");

    while(1) {
        int fd = recv_fd(bowl_usfd);
        // debug(fd);

        int speed, spin;
        read(fd, &speed, sizeof(speed));
        // perror("read1");
        read(fd, &spin, sizeof(spin));
        // perror("read2");
        printf("speed = %d, spin = %d\n", speed, spin);

        bool miss = ((speed + spin)%4 == 0);
        if(miss) {
            printf("missed the ball!!\n\n");
            /* miss the ball by not replying to umpire */
            // sleep(5);
        }else {
            /* reply to umpire */
            if(umpire_pid == -1) umpire_pid = pidof("./umpire.exe");
            kill(umpire_pid, SIGUSR2);

            int score = rand()%(41);
            printf("generated score = %d\n\n", score);
            char c_score[10];
            sprintf(c_score, "%d", score);
            sendto(rsfd, c_score, sizeof(c_score), 0, (struct sockaddr *)&cli, sizeof(cli));
        }

        /* return ball to umpire */
        send_fd(ump_usfd, fd);
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

int pidof(char *name) {
    char buf[100] = {0};
    sprintf(buf, "pidof %s", name);
    int fd = fileno(popen(buf, "r"));	
    char s[100];
	read(fd, &s, 1000);
	return atoi(s); 
}

