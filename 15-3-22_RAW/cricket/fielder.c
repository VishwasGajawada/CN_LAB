#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>

void handler_error(char *msg, int quit) {
    perror(msg);
    if (quit!=0) exit(EXIT_FAILURE);
}


#define NUM_FIELDERS 4
int fielders[] = {10,20,30,40};
int findClosestFielder(int score) {
    int closest = -1;
    for(int i=0; i<NUM_FIELDERS; i++) {
        if(closest == -1 || (abs(score - fielders[i]) < abs(score - fielders[closest]))) {
            closest = i;
        }
    }
    return closest;
}

int main(int argc, char *argv[]) {
    int my_id = (argc < 2) ? 0 : atoi(argv[1]);

    /* rsfd1 for seeing the hit by batsman */
    int rsfd1 = socket(AF_INET, SOCK_RAW, 2);
    if(rsfd1 < 0) handler_error("socket", 1);
    /* rsfd2 for indicating which fielder caught the ball, if any */
    int rsfd2 = socket(AF_INET, SOCK_RAW, 3);
    if(rsfd2 < 0) handler_error("socket", 1);

    struct sockaddr_in cli;
    cli.sin_family = AF_INET;
    cli.sin_addr.s_addr = inet_addr("127.0.0.1");

    while(1) {
        /* recieve score from batsman */
        char buf[100] = {0};
        int len = recvfrom(rsfd1, buf, sizeof(buf), 0, NULL, NULL);
        if(len < 0) handler_error("recvfrom", 1);

        struct iphdr *ip = (struct iphdr *)buf;
        char *c_score = buf + (ip->ihl)*4;
        int score = atoi(c_score);
        printf("\nscore = %d\t", score);

        if(score%6!=0 && score%4!=0) {
            int closest = findClosestFielder(score);
            if(my_id == closest) {
                printf("I caught the ball, hurray!\n");
                char msg[100] = {0};

                /* inform umpire that I caught the ball */
                sprintf(msg,"Fielder %d caught the ball", my_id);
                sendto(rsfd2, msg, sizeof(msg), 0, (struct sockaddr *)&cli, sizeof(cli));
            }else {
                printf("%d caught the ball\n", fielders[closest]);
            }
        }
    }
    return 0;
}