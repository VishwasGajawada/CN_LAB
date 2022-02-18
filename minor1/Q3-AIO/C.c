#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("socket");
        exit(1);
    }
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8080);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));

    char buffer[256] = {0};
    while(1) {
        int rd = recv(sockfd, buffer, 256, 0);
        if(strcmp(buffer, "exit") == 0) {
            break;
        }
        if(rd <=0 ) {
            printf("connection closed\n");
            break;
        }
        printf("%s", buffer);
    }
    return 0;
}