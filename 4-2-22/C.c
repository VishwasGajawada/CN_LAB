#include <stdio.h>
#include <string.h> // strlen
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>
#include <stdlib.h> // atoi

int main(int argc, char **argv) {

    int client_number = 1;
    if(argc > 1) client_number = atoi(argv[1]);
    printf("I am client %d\n", client_number);
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in saddr; 
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // now connect to server
    int connect_status = connect(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (connect_status == -1) {
        perror("connect");
        return 1;
    }
    
    printf("Enter the data server you want to connect to (1-3): ");
    char c;
    scanf("%c", &c);
    send(sfd, &c, 1, 0);

    while(1){
        char msg[100]={0};
        if(recv(sfd, msg, sizeof(msg), 0) <=0 ){
            printf("Server closed connection. Exiting ...\n");
            return 1;
        }
        printf("From D%c : %s\n", c, msg);
    }


    close(sfd);

    return 0;
}

