#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <arpa/inet.h> // inet_pton
#include <fcntl.h>
#include <stdlib.h>

#define S1_PATH "mysocket1"
#define S2_PATH "mysocket2"

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

int main() {
    int usfd[2] = {serv_uds_listen(S1_PATH), serv_uds_listen(S2_PATH)};
    // if (usfd < 0) { return -1;}

    int sfd = serv_tcp_listen(8080);
    if (sfd < 0) { return -1;}

    struct sockaddr_un cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    while(1) {
        int nsfd = accept(sfd, (struct sockaddr *)&cli_addr, &cli_len);
        if (nsfd < 0) {
            perror("accept");
            return -1;
        }
        char client_choice[3] = {0};
        recv(nsfd, client_choice, sizeof(client_choice), 0);
        int choice = atoi(client_choice);
        printf("Clients choice is %d\n", choice);
        if(choice!=1 && choice!=2) {
            printf("Invalid choice\n");
            continue;
        }

        int unsfd = accept(usfd[choice-1], (struct sockaddr *)&cli_addr, &cli_len);
        if( send_fd(unsfd, nsfd) < 0) {
            perror("send_fd");
            continue;
        }
        printf("Sent client fd to Si\n\n");
        
    }

    close(usfd[0]);
    close(usfd[1]);
    unlink(S1_PATH);
    unlink(S2_PATH);
    return 0;
}