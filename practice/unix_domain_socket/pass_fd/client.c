#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <fcntl.h>
#include <semaphore.h>

#define SOCK_PATH "mysocket"
#define sem_name1 "sem1"
#define sem_name2 "sem2"

int cli_conn() {
    int fd;
    struct sockaddr_un un;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    bzero(&un, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, SOCK_PATH);
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

int main(){
    sem_t *sem1 = sem_open(sem_name1, O_CREAT, 0644, 0);
    sem_t *sem2 = sem_open(sem_name2, O_CREAT, 0644, 0);
    int usfd = cli_conn();

    char buff[1024];
    if( recv(usfd, buff, sizeof(buff), 0) < 0) {
        perror("recv");
        return -1;
    }

    printf("%s", buff);

    char *msg = "Hello, server!\n";
    send(usfd, msg, strlen(msg), 0);

    int fd = recv_fd(usfd);
    if(fd < 0) {
        perror("recv_fd");
        return -1;
    }
    while(1) {
        char c;
        sem_wait(sem1);
        for( int i = 0; i < 5; i++ ) {
            if (read(fd, &c, 1) <=0) {
                printf("EOF\n");
                sem_post(sem2);
                return -1;
            }
            printf("%c", c);
        }
        printf("\n");
        sem_post(sem2);
    }


    close(usfd);
    unlink(SOCK_PATH);
    return 0;
}