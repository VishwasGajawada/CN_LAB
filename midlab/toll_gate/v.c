#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <arpa/inet.h> /* inet_pton */
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void my_perror(char *msg, int quit) {
    perror(msg);
    if(quit == 1) exit(0);
}

int cli_tcp_connect(int port);

void usage() ;

/* vehicle i vi */
int main(int argv, char **argc) {

    char  *vehicle_number, num_wheels, gate_number;
    
    if(argv < 4) usage();
    else {
        vehicle_number = argc[1];
        num_wheels = atoi(argc[2]);
        gate_number = atoi(argc[3]);
    }

    printf("my vehicle number is %s\n", vehicle_number);
    printf("I am %d wheeler\n", num_wheels);
    printf("I will go through gate %d\n\n", gate_number);

    int gate_port = 8080 + 10*(gate_number - 1) + num_wheels;
    printf("I will connect to port %d in gate %d\n", gate_port, gate_number);

    int sfd = cli_tcp_connect(gate_port);
    /* send vehicle number to gate */
    send(sfd, vehicle_number, 10, 0);

    struct sockaddr_in addr;  
    addr.sin_family = AF_INET;
    addr.sin_port = htons(gate_port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int accepted;
    recv(sfd, &accepted, sizeof(int), 0);
    if(accepted == 1) {
        printf("I am allowed to go through gate %d\n", gate_number);
    }
    else {
        printf("I am not allowed to go through gate %d\n", gate_number);
    }

    /* close fds and free buffers */
    close(sfd);
    // free(vehicle_number);


    return 0;
}

int cli_tcp_connect(int port) {
    int sfd;
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd < 0)  handle_error("socket");
    struct sockaddr_in addr;
    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // connect 
    connect(sfd, (struct sockaddr *)&addr, sizeof(addr));
    return sfd;
}

void usage(){
    printf("format : ./v.exe <vehicle_number> <num_wheels> <gate_number>\n");
    exit(0);
}


