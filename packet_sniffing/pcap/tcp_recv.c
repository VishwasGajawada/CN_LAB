#include <stdio.h>
#include <string.h> // strlen
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

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


int main() {
    int sfd = cli_tcp_connect(8080);
    char msg[100] = "hello world";

    while(1) {

        // recv 
        char buf[100];
        recv(sfd, buf, sizeof(buf), 0);

        printf("%s\n", buf);
        
        // send
        send(sfd, msg, sizeof(msg), 0);
    }

    // close
    close(sfd);

}