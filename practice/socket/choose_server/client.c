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
    if (inet_pton(AF_INET, "127.0.0.1", &saddr.sin_addr) == -1) {
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

    char msg[1024]={0}, reply[1024]={0};
    // choose server
    recv(sfd, reply, sizeof(reply), 0);
    printf("%s\n", reply);

    scanf("%s", msg);
    send(sfd, msg, strlen(msg), 0);

    memset(reply, 0, sizeof(reply));
    // "Enter the string" or error or exit
    recv(sfd, reply, sizeof(reply), 0);
    printf("%s\n", reply);

    scanf("%s", msg);
    strcat(msg, " ");
    send(sfd, msg, strlen(msg), 0);

    // processed string
    memset(reply, 0, sizeof(reply));
    recv(sfd, reply, sizeof(reply), 0);
    printf("%s\n", reply);

    close(sfd);

    return 0;
}

