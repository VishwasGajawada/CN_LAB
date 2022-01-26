#include <stdio.h>
#include <string.h> // strlen
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>
int main() {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        return 1;
    }


    struct sockaddr_in saddr; // server address
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    // 74.125.235.20 - google
    // saddr.sin_addr.s_addr = inet_addr("74.125.235.20");
    if(inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr) == -1) {
        perror("inet_pton");
        return 1;
    }
    printf("%s\n", inet_ntoa(saddr.sin_addr));

    // now connect to server
    int connect_status = connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (connect_status == -1) {
        perror("connect");
        return 1;
    }
    printf("Connected to server\n");

    // send get request to server
    char *message = "hello server\n";
    int send_status = send(sfd, message, strlen(message), 0);
    if (send_status == -1) {
        perror("send");
        return 1;
    }
    printf("Message sent\n");

    char server_reply[1024];
    int recv_status = recv(sfd, server_reply, sizeof(server_reply), 0);
    if (recv_status == -1) {
        perror("recv");
        return 1;
    }
    printf("Response received\n");
    printf("%s\n", server_reply);

    close(sfd);

    return 0;
}

