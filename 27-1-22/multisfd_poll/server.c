#include <stdio.h>
#include <string.h> // strlen
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>
#include <sys/poll.h> // poll
#include <pthread.h>
#include <ctype.h> // toupper and tolower

void *service_thread_1(void *arg);
void *service_thread_2(void *arg);
void *service_thread_3(void *arg);


int main() {
    int n_services = 3;
    int sfd[n_services];
    struct sockaddr_in saddr[n_services];
    struct pollfd pfd[n_services];

    for(int i=0; i<n_services; i++){
        sfd[i] = socket(AF_INET, SOCK_STREAM, 0);
        saddr[i].sin_family = AF_INET;
        saddr[i].sin_port = htons(8080 + i);
        saddr[i].sin_addr.s_addr = INADDR_ANY;
        int opt = 1;
        // Forcefully attaching socket to the port, so that bind already in use error doesnt occur
        if (setsockopt(sfd[i], SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            perror("setsockopt");
            return 1;
        }
        int bind_status = bind(sfd[i], (struct sockaddr*)&saddr[i], sizeof(saddr[i]));
        if (bind_status == -1) {
            perror("bind");
            return 1;
        }
        int listen_status = listen(sfd[i], 5);
        if (listen_status == -1) {
            perror("listen");
            return 1;
        }

    
        pfd[i].fd = sfd[i];
        pfd[i].events = POLLIN;
    }

    struct sockaddr_in caddr;
    socklen_t caddr_len = sizeof(caddr);
    printf("Starting to poll..\n\n");
    while(1){
        int ready = poll(pfd, n_services, -1);
        if(ready == -1) return 0;
        for(int i=0; i<n_services; i++){
            if(pfd[i].revents == 0) continue;

            if(pfd[i].revents & POLLIN){
                int nsfd = accept(pfd[i].fd, (struct sockaddr*)&caddr, &caddr_len);
                if(nsfd == -1) continue;

                pthread_t pid;
                if(i==0){
                    pthread_create(&pid, NULL, service_thread_1, (void*)&nsfd);
                }
                else if(i==1){
                    pthread_create(&pid, NULL, service_thread_2, (void*)&nsfd);
                }
                else if(i==2){
                    pthread_create(&pid, NULL, service_thread_3, (void*)&nsfd);
                }
            }
        }
    }

    for(int i=0; i<n_services; i++){
        close(sfd[i]);
    }
    return 0;
}

void *service_thread_1(void *arg){
    printf("Service thread 1 started\n");
    int nsfd = *(int*)arg;
    char msg[1024] = "Hello from service_thread_1\n";
    send(nsfd, msg, strlen(msg), 0);

    memset(msg, 0, sizeof(msg));
    while(recv(nsfd, msg, sizeof(msg), 0) > 0){
        if(strcmp(msg, "exit") == 0){
            printf("Service_thread_1 exiting\n");
            close(nsfd);
            pthread_exit(NULL);
        }
        printf("%s\n", msg);
        memset(msg, 0, sizeof(msg));
    }
    close(nsfd);
    return NULL;
}

void *service_thread_2(void *arg){
    printf("Service thread 2 started\n");
    int nsfd = *(int*)arg;
    char msg[1024] = "Hello from service_thread_2\n";
    send(nsfd, msg, strlen(msg), 0);

    memset(msg, 0, sizeof(msg));
    recv(nsfd, msg, sizeof(msg), 0);
    // convert to uppercase
    for(int i=0; i<strlen(msg); i++){
        msg[i] = toupper(msg[i]);
    }
    send(nsfd, msg, strlen(msg), 0);

    printf("Service_thread_2 exiting\n");
    close(nsfd);
    return NULL;
}

void *service_thread_3(void *arg){
    printf("Service thread 3 started\n");
    int nsfd = *(int*)arg;
    char msg[1024] = "Hello from service_thread_3\n";
    send(nsfd, msg, strlen(msg), 0);

    memset(msg, 0, sizeof(msg));
    recv(nsfd, msg, sizeof(msg), 0);
    // convert to lowercase
    for(int i=0; i<strlen(msg); i++){
        msg[i] = tolower(msg[i]);
    }
    send(nsfd, msg, strlen(msg), 0);

    printf("Service_thread_3 exiting\n");
    close(nsfd);
    return NULL;
}
