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
#include <stdbool.h>
/*** for pthread ***/
#include <pthread.h>

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

bool i_am_running = false;

void* send_worker(void *arg) {
    // send that I am client i via raw socket
    int rsfd =  -1;

    while(rsfd < 0) {
        rsfd =  socket(AF_INET, SOCK_RAW, 2);
        if(rsfd < 0) perror("socket");
    }

    printf("send_worker\n");
    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = INADDR_ANY;
    while(1) {
        if(i_am_running) {

            char send_msg[100] = {0};
            sprintf(send_msg, "I am client 1, I am running currently\n");
            sendto(rsfd, send_msg, sizeof(send_msg), 0, (struct sockaddr *)&client, sizeof(client));
            sleep(1);
        }
    }
}

int main() {
    printf("My pid is %d\n", getpid());
    int msqid;
    int usfd;

    msqid = msgq_init();
    if(msqid < 0) handle_error("msgq_init");

    /* connect to sfd */
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd <0 )handle_error("socket");

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if( connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) )
        handle_error("connect");


    i_am_running = true;

    /* do some work */
    /* interact with server */
    char buf[100] = {0};
    sprintf(buf, "Hello from %d", getpid());
    send(sfd, &buf, sizeof(buf), 0);
    memset(buf, 0, sizeof(buf));
    recv(sfd, buf, sizeof(buf), 0);
    printf("From server: %s\n", buf);

    pthread_t pid;
    pthread_create(&pid, NULL, send_worker, NULL);
    
    /* do some work for 4 seconds */
    sleep(4);

    i_am_running = false;

    /* look for next client in the sharing, and notes its pid */
    struct msgbuf msg;
    msg.mtype = 1;
    memset(msg.mtext, 0, sizeof(msg.mtext));
    if( msgrcv(msqid, &msg, sizeof(msg), 1, 0) < 0)
        handle_error("msgrcv");
    int next_client_pid = atoi(msg.mtext);

    

    /* now be ready to listen, inorder to send sfd */
    usfd = serv_uds_listen(uds_path);
    kill(next_client_pid, SIGUSR1);
    perror("kill");

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





