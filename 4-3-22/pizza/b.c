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

#define NUM_WAITERS 3

int serv_tcp_listen(int port) ;
int serv_uds_listen(char *path) ;
int pidof(char *name);
int getWaiter();
int send_fd(int socket, int fd_to_send);
void sigusr1_handler(int signo);


/* stage */
int main() {
    signal(SIGUSR1, sigusr1_handler);
    time_t t;
    srand((unsigned) time(&t));

    int sfd;
    int wsfd[NUM_WAITERS], wnsfd[NUM_WAITERS];
    int dsfd, dnsfd;
    struct sockaddr_in addr;

    sfd = serv_tcp_listen(8080);
    if(sfd < 0) handle_error("serv_tcp_listen");

    /* connection with delivery boy */
    dsfd = serv_uds_listen("d");
    if(dsfd < 0) handle_error("serv_uds_listen");
    dnsfd = accept(dsfd, NULL, NULL);
    
    
    /* connection with waiters */
    for(int i = 0; i < NUM_WAITERS; i++) {
        char buf[10];
        sprintf(buf, "w%d", i+1);
        wsfd[i] = serv_uds_listen(buf);
        if(wsfd[i] < 0) handle_error("serv_uds_listen");

        wnsfd[i] = accept(wsfd[i], NULL, NULL);
    }

   


    int nsfd;
    while(1) {
        nsfd = accept(sfd, NULL, NULL);
        if(nsfd < 0) handle_error("accept");

        char combo[10];
        recv(nsfd, combo, sizeof(combo), 0);
        printf("Clients combo = %s\n", combo);

        int w = getWaiter();
        // pass customer
        send_fd(wnsfd[w], nsfd);
        // send combo number
        send(wnsfd[w], combo, sizeof(combo), 0);
        printf("passed customer and his combo to waiter %d\n", w+1);

        // pause for signal from waiter
        pause();

        // pass customer to delivery
        send_fd(dnsfd, nsfd);
    }


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

int getWaiter() {
    return rand() % NUM_WAITERS;
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

void sigusr1_handler(int signo) {
    printf("signal received\n");
    fflush(stdout);
}