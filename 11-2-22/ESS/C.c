#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <arpa/inet.h> /* inet_pton */
#include <fcntl.h>
#include <stdlib.h>
#include <sys/ipc.h> /* for ftok */
#include <sys/msg.h> /* for msgget, msgsnd, msgrcv */
#include <signal.h>

#define uds_path "mysocket"
#define msgq_path "ess.c"

struct msgbuf {
    long mtype;
    char mtext[100];
};

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)


int serv_uds_listen(char *path) ;

int cli_uds_conn(char *path) ;

int recv_fd(int socket) ;

int send_fd(int socket, int fd_to_send) ;

int msgq_init() ;

void sig_usr1_handler(int signo) ;

int previous_client_ready_to_accept = -1; // will be set in signal handler

int main() {
    signal(SIGUSR1, sig_usr1_handler);

    printf("My pid is %d\n", getpid());
    int msqid, usfd, sfd;

    msqid = msgq_init();
    if(msqid < 0) handle_error("msgq_init");

    int d;
    printf("Enter any digit to add this process to message queue : ");
    scanf("%d", &d);
    /* add pid to message queue */
    struct msgbuf msg;
    msg.mtype = 1;
    sprintf(msg.mtext, "%d", getpid());
    if( msgsnd(msqid, &msg, sizeof(msg), 0) < 0)
        handle_error("msgsnd");

    while(previous_client_ready_to_accept == -1) {
        pause();
    }
    usfd = cli_uds_conn(uds_path);
    if(usfd < 0) handle_error("cli_uds_conn");

    sfd = recv_fd(usfd);
    printf("recieved sfd from previous client\n");

    /* interact with server */
    char buf[100] = {0};
    sprintf(buf, "Hello from %d", getpid());
    send(sfd, &buf, sizeof(buf), 0);
    memset(buf, 0, sizeof(buf));
    recv(sfd, buf, sizeof(buf), 0);
    printf("From server: %s\n", buf);

    close(usfd);
    // usfd = serv_uds_listen(uds_path);
    /* look for next client in the sharing */
    msg.mtype = 1;
    memset(msg.mtext, 0, sizeof(msg.mtext));
    if( msgrcv(msqid, &msg, sizeof(msg), 1, 0) < 0)
        handle_error("msgrcv");

    int next_client_pid = atoi(msg.mtext);

    /* now be ready to listen, inorder to send sfd */
    usfd = serv_uds_listen(uds_path);
    kill(next_client_pid, SIGUSR1);

    // accept only after killing, so that we are sure that we are accepting connection of the next(first) client of the message queue
    int unsfd = accept(usfd, NULL, NULL);
    /* pass sfd to it */
    send_fd(unsfd, sfd);

    printf("Sent sfd to next client(%d)\n", next_client_pid);
    printf("Exiting...\n");
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

int msgq_init() {
    int msqid;
    key_t key;

    key = ftok(msgq_path, 'a');
    if(key < 0) handle_error("ftok");

    msqid = msgget(key, IPC_CREAT | 0666);
    if(msqid < 0) handle_error("msgget");

    return msqid;
}

void sig_usr1_handler (int signo) {
    // sfd = recv_fd(usfd);
    previous_client_ready_to_accept = 1;
}






