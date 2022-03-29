#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <arpa/inet.h> /* inet_pton */
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/poll.h>
#include <stdbool.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void my_perror(char *msg, int quit) {
    perror(msg);
}

#define FT_PORT 8200

int main() {
    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sfd < 0) handle_error("socket");

    int port = FT_PORT;
    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));

    saddr.sin_family = AF_INET;
    saddr.sin_port = port;
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(sfd,(struct sockaddr *) &saddr,sizeof(saddr))==-1)
	    perror("\n bind : ");

    /* raw socket init */
    int rsfd =  socket(AF_INET, SOCK_RAW, 2);
    if(rsfd < 0) my_perror("rsfd socket", 0);

    struct sockaddr_in client;
    client.sin_family = AF_INET;
    client.sin_addr.s_addr = INADDR_ANY;

    while(1) {
        int fast_tag_id;
        char vehicle_number[1024] = {0};

        recvfrom(sfd, &fast_tag_id, sizeof(int), 0, NULL, NULL);
        recvfrom(sfd, vehicle_number, 10, 0, NULL, NULL);

        printf("Vehicle number = %s, fasttagid = %d\n", vehicle_number, fast_tag_id);

        int valid_fast_tag = (fast_tag_id %3 == 0);
        printf("valid = %d\n", valid_fast_tag);

        // send(rsfd, vehicle_number, 10, 0);
        // send(rsfd, &valid_fast_tag, sizeof(int), 0);
        sendto(rsfd, vehicle_number, 1024, 0, (struct sockaddr *)&client, sizeof(client));
        char valid[1024] = {0};
        sprintf(valid,"%d", valid_fast_tag);
        sendto(rsfd, &valid, 1024, 0, (struct sockaddr *)&client, sizeof(client));
    }

    return 0;
}
