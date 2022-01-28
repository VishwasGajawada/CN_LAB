#include <stdio.h>
#include <string.h> // strlen
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>
#include <stdlib.h> 

int main(){
    int sfd;
    struct sockaddr_in saddr;
    socklen_t len;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {perror("socket");return 1;}

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // now connect to server
    int connect_status = connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (connect_status == -1) {perror("connect");return 1;}

    // send P.cpp to S
    char *file = "P.cpp";
    send(sfd, file, strlen(file), 0);

    // read test case result
    char buffer[1024];
    recv(sfd, buffer, sizeof(buffer), 0);

    printf("%s\n", buffer);

    close(sfd);
    return 0;
}