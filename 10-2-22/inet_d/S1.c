#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <fcntl.h>
#include <semaphore.h>

#define S1_PATH "mysocket1"

// gcc S1.c -o S1.exe

int cli_conn(char *path) {
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

int main() {
    int usfd = cli_conn(S1_PATH);
    if(usfd < 0) {
        printf("usfd error\n");
        return -1;
    }

    int nsfd = recv_fd(usfd);

    for(int i=0; i<3; i++){
        char *send_msg = "Hi client from S1\n";
        send(nsfd, send_msg, strlen(send_msg), 0);
        
        char buffer[1024] = {0};
        int recv_status = recv(nsfd, buffer, sizeof(buffer), 0);
        if (recv_status == -1) {perror("recv");return 1;}
        printf("%s\n", buffer);
    }

    return 0;
}