#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <arpa/inet.h> /* inet_pton */
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/poll.h>


#define NUM_WAITERS 3

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int cli_uds_conn(char *path) ;
int serv_tcp_listen(int port) ;
int recv_fd(int socket) ;

int main() {
    int dsfd = serv_tcp_listen(8081);
    if(dsfd < 0) handle_error("serv_tcp_listen");
    int busfd = cli_uds_conn("d");


    int wnsfd[NUM_WAITERS];
    struct pollfd pfd[NUM_WAITERS];
    for(int i = 0; i < NUM_WAITERS; i++) {
        wnsfd[i] = accept(dsfd, NULL, NULL);
        pfd[i].fd = wnsfd[i];
        pfd[i].events = POLLIN;
    }

    while(1) {
        int fd = recv_fd(busfd);
        printf("fd = %d\n", fd);
        if(fd < 0) {
            printf("Shop closed\n");
            break;
        }
        char combo_items[20];
        // any waiter can send. so do poll
        poll(pfd, NUM_WAITERS, -1);
        for(int i = 0; i < NUM_WAITERS; i++) {
            if(pfd[i].revents & POLLIN) {
                recv(wnsfd[i], combo_items, sizeof(combo_items), 0);
                break;
            }
        }

        printf("Combo items: %s\n", combo_items);
        // handover parcel to customer

        send(fd, combo_items, sizeof(combo_items), 0);
    }

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




