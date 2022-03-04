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

#define NUM_WAITERS 1

int cli_tcp_connect(int port);

/* stage */
int main() {
    int sfd = cli_tcp_connect(8080);

    char combo[10] = "1";
    send(sfd, combo, sizeof(combo), 0);

    char combo_items[20];
    recv(sfd, combo_items, sizeof(combo_items), 0);

    printf("Combo items: %s\n", combo_items);

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

