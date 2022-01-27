#include <stdio.h>
#include <string.h> // strlen
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>
int main() {
    int sfd;
    struct sockaddr_in saddr, saddr2, caddr;
    socklen_t len;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        return 1;
    }

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int opt = 1;
    // Forcefully attaching socket to the port, so that bind already in use error doesnt occur
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        return 1;
    }

    int bind_status = bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (bind_status == -1) {
        perror("bind");
        return 1;
    }
    len = sizeof(saddr2);
    getsockname(sfd, (struct sockaddr*)&saddr2, &len);
    char myIP[20];
    inet_ntop(AF_INET, &saddr2.sin_addr.s_addr, myIP, sizeof(myIP));
    int myPort = ntohs(saddr2.sin_port);

    printf("Server IP: %s\n", myIP);
    printf("Server Port: %d\n", myPort);



    int listen_status = listen(sfd, 5);
    if (listen_status == -1) {
        perror("listen");
        return 1;
    }

    socklen_t caddr_len = sizeof(caddr);
    int nsfd;
    while(nsfd = accept(sfd, (struct sockaddr*)&caddr, &caddr_len)){
        if (nsfd == -1) {
            perror("accept");
            return 1;
        }
        char clientIP[20];
        int clientPort = ntohs(caddr.sin_port);
        caddr_len = sizeof(caddr);
        getpeername(nsfd, (struct sockaddr*)&caddr, &caddr_len);
        inet_ntop(AF_INET, &caddr.sin_addr.s_addr, clientIP, sizeof(clientIP));

        printf("Foreign IP: %s\n", clientIP);
        printf("Foreign Port: %d\n", clientPort);

        char word[100] = {0};
        if(recv(nsfd, word, sizeof(word), 0) == -1){
            perror("recv");
            return 1;
        }
        printf("Recieved word: %s\n", word);
        printf("\n");
    }




    return 0;
}