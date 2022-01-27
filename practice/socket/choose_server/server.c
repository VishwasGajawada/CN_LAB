#include <stdio.h>
#include <string.h> // strlen
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>
#include <pthread.h> // link with lpthread
#include <stdlib.h> // atoi
#include <sys/wait.h>

void* choice_thread(void* arg);
void exec_server(int nsfd, int choice);

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
        pthread_create(&tid, NULL, choice_thread, (void*)&nsfd);
    }
    close(nsfd);
    close(sfd);
    return 0;
}

void* choice_thread(void* arg) {
    int nsfd = *(int*)arg;
    char *msg = "Please choose a server: \n1. ReverseString 1\n2. SmallToCapital 2\n3. Exit 3\n";
    send(nsfd, msg, strlen(msg), 0);

    char reply[1024];
    recv(nsfd, reply, sizeof(reply), 0);
    int choice = atoi(reply);

    printf("Choice: %d\n", choice);
    if(choice < 1 || choice > 3) {
        printf("Invalid choice\n");
        strcpy(reply, "wrong");
        send(nsfd, reply, strlen(reply), 0);
    }else if(choice == 3){
        strcpy(reply, "exit");
        send(nsfd, reply, strlen(reply), 0);
    }else{
        exec_server(nsfd, choice);
    }
}
void exec_server(int nsfd, int choice){
    int c = fork();
    if(c > 0){
        close(nsfd);
        wait(NULL);
    }else{
        dup2(nsfd, 0);
        dup2(nsfd, 1);
        char *args[]  = {NULL};
        if(choice == 1){
            execvp("./reverse_string.exe", args);
        }else if(choice == 2){
            execvp("./small_to_capital.exe", args);
        }
    }
}