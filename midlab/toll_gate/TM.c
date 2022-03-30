#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <arpa/inet.h> /* inet_pton */
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/poll.h>
/* for shared memory */
#include <sys/shm.h>
/* contains definitions for ipc structures */
#include <sys/ipc.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void my_perror(char *msg, int quit) {
    perror(msg);
}
int cli_uds_conn(char *path) ;
int recv_fd(int socket) ;
void two_wheeler_handler(int signo);
void four_wheeler_handler(int signo);
int printVehicleFdDetails(int fd);

int *countMemory;
#define FT_PORT 8200

int main() {
    signal(SIGUSR1, two_wheeler_handler);
    signal(SIGUSR2, four_wheeler_handler);

    /* shared memory initialisation for counters */
    key_t key = ftok("TM.c", 'a');    
    int shmid = shmget(key, 2*sizeof(int), 0666 | IPC_CREAT);
    countMemory = shmat(shmid, NULL, 0);
    countMemory[0] = 0; /* init num 2 wheelers = 0 */
    countMemory[1] = 0; /* init num 4 wheelers = 0 */

    char gate_paths[3][100] = {"g1.uds" , "g2.uds" , "g3.uds"};
    int gate_sfds[3];
    for(int i=0; i<3; i++) {
        gate_sfds[i] = cli_uds_conn(gate_paths[i]);
    }

    struct pollfd pfd[3];
    for(int i=0; i<3; i++) {
        pfd[i].fd = gate_sfds[i];
        pfd[i].events = POLL_IN;
    }

    /* fast tag server details */
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in faddr;
    bzero(&faddr, sizeof(faddr));
    faddr.sin_family = AF_INET;
    faddr.sin_port = FT_PORT;
    faddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    while(1) {
        int ready = poll(pfd, 3, -1);
        if(ready == -1) continue;

        for(int i=0; i<3; i++) {
            if(pfd[i].revents == 0) continue;
            if(pfd[i].revents & POLLIN) {
                int vehicle_fd  = recv_fd(pfd[i].fd);
                if(vehicle_fd < 0) handle_error("recv_fd");
                char vehicle_number[10] = {0};
                recv(pfd[i].fd, vehicle_number, 10, 0);
                printf("gate %d passed details of vehicle %s\n", i+1, vehicle_number);


                int fast_tag_id = printVehicleFdDetails(vehicle_fd);

                /* send any vehicle detail to FastTag (FT) for verification */
                sendto(sfd, &fast_tag_id, sizeof(int), 0, (struct sockaddr *)&faddr, sizeof(faddr));
                sendto(sfd, vehicle_number, 10, 0, (struct sockaddr *)&faddr, sizeof(faddr));


            }
        }

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

void two_wheeler_handler(int signo){
    /* increment number of 2 wheeler passed */
    countMemory[0]++;
    printf("Number of 2 wheelers passed = %d\n", countMemory[0]);
    fflush(stdout);
}

void four_wheeler_handler(int signo){
    /* increment number of 4 wheeler passed */
    countMemory[1]++;
    printf("Number of 4 wheelers passed = %d\n", countMemory[1]);
    fflush(stdout);
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

int printVehicleFdDetails(int fd) {
    struct sockaddr_in peer;
    int peer_len;
    
    peer_len = sizeof(peer);
        
    if (getpeername(fd, (struct sockaddr*)&peer, &peer_len) == -1) {
        perror("getpeername() failed");
        return -1;
    }

        /* Print it.    */
    printf("Vehicle's IP address is: %s\n", inet_ntoa(peer.sin_addr));
    printf("Vehicle's port is: %d\n", (int) ntohs(peer.sin_port));

    printf("\n");

    return (int) ntohs(peer.sin_port);
}



