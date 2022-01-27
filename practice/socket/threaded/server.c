#include <stdio.h>
#include <string.h> // strlen
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>
#include <pthread.h> // link with lpthread

void* server_thread(void* arg);

int main() {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in saddr; // server address
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = INADDR_ANY;

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

    int listen_status = listen(sfd, 5);
    if (listen_status == -1) {
        perror("listen");
        return 1;
    }

    struct sockaddr_in caddr;
    socklen_t caddr_len = sizeof(caddr);

    printf("Waiting for incoming connections\n");
    int nsfd;
    while (nsfd = accept(sfd, (struct sockaddr*)&caddr, &caddr_len)) {
        if (nsfd == -1) {
            perror("accept");
            continue;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, server_thread, (void*)&nsfd);
        printf("handler assigned\n\n");
    }
    close(nsfd);
    close(sfd);
    return 0;
}

void* server_thread(void* arg) {
    int nsfd = *(int*)arg;
    char msg[1024] = { 0 };
    int recv_status = recv(nsfd, msg, sizeof(msg), 0);
    if (recv_status == -1) {
        perror("recv");
        return NULL;
    }
    printf("Response received\n");
    printf("%s", msg);
    strcpy(msg, "hello client\n");
    write(nsfd, msg, strlen(msg));

    close(nsfd);
    return NULL;
}