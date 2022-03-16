#include <stdio.h>
#include <stdlib.h>
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
int send_fd(int socket, int fd_to_send);
int recv_fd(int socket) ;
int pidof(char *name);

int main() {
    srand(time(0));
    int umpire_pid = -1;

    int uusfd = serv_uds_listen("ump_bowl");
    int uunsfd = accept(uusfd, NULL, NULL);
    printf("umpire is accepted. %d\n", uunsfd);

    int busfd = serv_uds_listen("bat_bowl");
    int bunsfd = accept(busfd, NULL, NULL);
    printf("batsman is accepted. %d\n", bunsfd);

    debug(uusfd);debug(uunsfd);debug(busfd);debug(bunsfd);

    int fd = open("ball.txt", O_RDWR|O_CREAT|O_TRUNC,S_IRWXU);
    if(fd < 0) handler_error("open", 1);
    while(1) {
        sleep(5);
        // send ball to batsman
        int speed = rand()%10+1;
        int spin = rand()%20+1;

        long pointer=lseek(fd, 0, SEEK_CUR);
        write(fd, &speed, sizeof(speed));
        write(fd, &spin, sizeof(spin));
        printf("speed = %d, spin = %d\n", speed, spin);
        // lseek(fd, -2*sizeof(int),  SEEK_CUR);

        // seek to the position before writing speed so that batsman can read them
        lseek(fd, pointer, SEEK_SET);
        send_fd(bunsfd, fd);

        /* inform umpire that ball is sent */
        if(umpire_pid == -1) {
            umpire_pid = pidof("./umpire.exe");
        }
        kill(umpire_pid, SIGUSR1);

        // recv ball from umpire
        fd = recv_fd(uunsfd);

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

