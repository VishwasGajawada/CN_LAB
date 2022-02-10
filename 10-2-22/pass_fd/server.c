#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <fcntl.h>
#include <semaphore.h>

#define SOCK_PATH "mysocket"
#define sem_name1 "sem1"
#define sem_name2 "sem2"

int serv_listen() {
    int fd;
    struct sockaddr_un addr;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);
    unlink(SOCK_PATH);
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
    sem_t *sem1 = sem_open(sem_name1, O_CREAT, 0644, 0);
    sem_t *sem2 = sem_open(sem_name2, O_CREAT, 0644, 0);
    int usfd = serv_listen();
    if (usfd < 0) { return -1;}

    struct sockaddr_un cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    int unsfd = accept(usfd, (struct sockaddr *)&cli_addr, &cli_len);
    if (unsfd < 0) {
        perror("accept");
        return -1;
    }

    char *msg = "Hello, client!\n";
    send(unsfd, msg, strlen(msg), 0);

    char buff[1024];
    recv(unsfd, buff, sizeof(buff), 0);
    printf("%s", buff);


    int fd = open("./file.txt", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }
    // read 5 characters
    
    if( send_fd(unsfd, fd) < 0 ) {
        perror("send_fd");
        return -1;
    }
    while(1) {
        char c;
        for( int i = 0; i < 5; i++ ) {
            if (read(fd, &c, 1) <=0) {
                printf("EOF\n");
                sem_post(sem1);
                return -1;
            }
            printf("%c", c);
        }
        printf("\n");
        // signal another process to read fd
        sem_post(sem1);
        // continue reading after recieving signal
        sem_wait(sem2);
    }



    close(unsfd);
    close(usfd);
    unlink(SOCK_PATH);
    return 0;
}