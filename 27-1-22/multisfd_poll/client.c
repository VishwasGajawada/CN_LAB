#include <stdio.h>
#include <string.h> // strlen
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>
#include <stdlib.h> // atoi
int main(int argc, char **argv) {

    int client_number = 1;
    if(argc > 1) client_number = atoi(argv[1]);
    printf("I am client %d, will connect to service %d of port %d\n", client_number, client_number, 8080 + client_number - 1);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        return 1;
    }
    struct sockaddr_in saddr; 
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080 + client_number - 1);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // now connect to server
    int connect_status = connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (connect_status == -1) {
        perror("connect");
        return 1;
    }
    
    char msg[100];
    recv(sfd, msg, sizeof(msg), 0);
    printf("%s\n", msg);

    if(client_number == 1){
        memset(msg, 0, sizeof(msg));
        while(1){
            printf("Enter message: ");
            fgets(msg, sizeof(msg), stdin);
            msg[strlen(msg) - 1] = '\0';
            send(sfd, msg, strlen(msg), 0);
            if(strcmp(msg, "exit") == 0) break;
            memset(msg, 0, sizeof(msg));
        }
    }else if(client_number == 2){
        printf("Enter message: ");
        memset(msg, 0, sizeof(msg));
        fgets(msg, sizeof(msg), stdin);
        send(sfd, msg, strlen(msg), 0);

        memset(msg, 0, sizeof(msg));
        recv(sfd, msg, sizeof(msg), 0);
        printf("%s\n", msg);

    }else if(client_number == 3){
        printf("Enter message: ");
        memset(msg, 0, sizeof(msg));
        fgets(msg, sizeof(msg), stdin);
        send(sfd, msg, strlen(msg), 0);

        memset(msg, 0, sizeof(msg));
        recv(sfd, msg, sizeof(msg), 0);
        printf("%s\n", msg);
    }


    close(sfd);

    return 0;
}

