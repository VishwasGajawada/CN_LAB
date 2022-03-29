#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <arpa/inet.h> /* inet_pton */
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/ip.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void my_perror(char *msg, int quit) {
    // handle_error(msg);
    perror(msg);
}

int serv_tcp_listen(int port) ;
int send_fd(int socket, int fd_to_send) ;
int serv_uds_listen(char *path);
int pidof(char *name);

#define uds_path "g3.uds"

int main() {
    int gate_number = 3;
    int w2_port = 8080 + 10*(gate_number - 1) + 2; /* 2 wheeler */
    int w4_port = 8080 + 10*(gate_number - 1) + 4; /* 4 wheeler */

    int w2_sfd = serv_tcp_listen(w2_port);
    my_perror("w2_sfd", 1);
    int w4_sfd = serv_tcp_listen(w4_port);
    my_perror("w4_sfd", 1);

    /* connect with toll manager */
    int usfd = serv_uds_listen(uds_path);
    my_perror("usfd", 1);
    int tm_nsfd = accept(usfd, NULL, NULL);
    my_perror("tm_nsfd accept", 1);

    int tm_pid = pidof("./TM.exe");


    /* raw socket init */
    int rsfd =  socket(AF_INET, SOCK_RAW, 2);
    if(rsfd < 0) my_perror("rsfd socket", 0);


    struct pollfd pfd[2];
    pfd[0].fd = w2_sfd;
    pfd[0].events = POLLIN;

    pfd[1].fd = w4_sfd;
    pfd[1].events = POLLIN;

    while(1) {
        int ready = poll(pfd, 2, -1);
        if(ready == -1) continue;

        for(int i=0; i<2; i++) {
            if(pfd[i].revents == 0) continue;
            if(pfd[i].revents & POLLIN) {
                int nsfd = accept(pfd[i].fd, NULL, NULL);
                if(nsfd == -1) {
                    my_perror("accept", 0);
                    continue;
                }
                char vehicle_number[10] = {0};
                recv(nsfd, vehicle_number, 10, 0);
                printf("Vehicle %s came to my gate\n", vehicle_number);

                /* send vehicle fd to TM */
                send_fd(tm_nsfd, nsfd);
                send(tm_nsfd, vehicle_number, 10, 0);

                /* now wait till FT.c approves or rejects this vehicle */
                int valid;
                while(1) {
                    char ft_vehicle[1024] = {0};
                    char is_valid[1024] = {0};

                    // recv(rsfd, vehicle_number, 10, 0);
                    // recv(rsfd, &valid, sizeof(int), 0);
                    recvfrom(rsfd, ft_vehicle, 1024, 0, NULL, NULL);
                    recvfrom(rsfd, is_valid, 1024, 0, NULL, NULL);

                    // printf("vehicle %s is valid = %d\n", ft_vehicle, valid);
                    struct iphdr *ip = (struct iphdr *)ft_vehicle;
                    // printf("'%s'\n", ft_vehicle+ip->ihl*4);
                    // ip = (struct iphdr *)is_valid;
                    // printf("'%s'\n", is_valid+ip->ihl*4);

                    char *ft_vehicle_val = ft_vehicle + ip->ihl*4;
                    // printf("ft_vehicle_val = '%s', vehicle_number = '%s'\n", ft_vehicle_val, vehicle_number);
                    if(strcmp(ft_vehicle_val, vehicle_number) == 0) {
                        valid = atoi(is_valid + ip->ihl*4);
                        break;
                    }
                }
                printf("valid = %d\n", valid);
                send(nsfd, &valid, sizeof(int), 0);
                if(valid) {
                    if(i == 0) {
                        // 2 wheeler
                        kill(tm_pid, SIGUSR1);
                    }else if(i == 1){
                        // 4 wwheeler
                        kill(tm_pid, SIGUSR2);
                    }
                }
            }
        }
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

int pidof(char *name) {
    char buf[100] = {0};
    sprintf(buf, "pidof %s", name);
    int fd = fileno(popen(buf, "r"));	
    char s[100];
	read(fd, &s, 1000);
	return atoi(s); 
}
