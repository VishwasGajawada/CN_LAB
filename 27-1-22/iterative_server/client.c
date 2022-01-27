#include <stdio.h>
#include <string.h> // strlen
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>
#include <stdlib.h> 


int main(int argc, char **argv) {
    int port = 8080;
    int sfd;
    struct sockaddr_in saddr, saddr2, faddr;
    socklen_t len;

    char localIP[20], foreignIP[20];
    int localPort, foreignPort;
    
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        return 1;
    }

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(port);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // now connect to server
    int connect_status = connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (connect_status == -1) {
        perror("connect");
        return 1;
    }


    len = sizeof(saddr2);
    getsockname(sfd, (struct sockaddr*)&saddr2, &len);
    inet_ntop(AF_INET, &saddr2.sin_addr.s_addr, localIP, sizeof(localIP));
    localPort = ntohs(saddr2.sin_port);

    printf("Local IP: %s\n", localIP);
    printf("Local Port: %d\n", localPort);

    len = sizeof(faddr);
    getpeername(sfd, (struct sockaddr*)&faddr, &len);
    inet_ntop(AF_INET, &faddr.sin_addr.s_addr, foreignIP, sizeof(foreignIP));
    foreignPort = ntohs(faddr.sin_port);

    printf("Foreign IP: %s\n", foreignIP);
    printf("Foreign Port: %d\n", foreignPort);

    printf("Enter a word : ");
    char word[100];
    scanf("%s", word);

    if(send(sfd, word, strlen(word), 0) == -1){
        perror("send");
        return 1;
    }

    close(sfd);

    return 0;
}

