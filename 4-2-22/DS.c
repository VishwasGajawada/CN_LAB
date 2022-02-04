#include <stdio.h>
#include <string.h> // strlen
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>
#include <fcntl.h> // open
#include <stdbool.h> // boolean
#include <sys/poll.h>

/* DS server */

int main(){
    printf("I am Multi_Data Server\n");
    int dsfd;
    struct sockaddr_in dsaddr, caddr;
    socklen_t len;

    dsfd = socket(AF_INET, SOCK_STREAM, 0);
    if (dsfd == -1) {perror("socket");return 1;}

    dsaddr.sin_family = AF_INET;
    dsaddr.sin_port = htons(8080);
    dsaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // bind
    int opt = 1;
    // so that bind already in use error doesnt occur
    if (setsockopt(dsfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        return 1;
    }
    bind(dsfd, (struct sockaddr*)&dsaddr, sizeof(dsaddr));

    // listen
    listen(dsfd, 10);

    // nsfds of all clients requested to Di
    int clients_connected[10][5];
    int clients_connected_count[10] = {0};
    int d_number[10] = {0}; // which Di's fd is in pfds[i]

    int n_pfds = 1; // number of fds to be polled

    // initially only DS server is available
    struct pollfd pfds[10];
    pfds[0].fd = dsfd;
    pfds[0].events = POLLIN;

    // poll on DS server for incoming connecionts, and on fds of Di for data output
    while(1){
        int ready = poll(pfds, n_pfds, -1);
        if(ready == -1) return 0;
        for(int i=0; i<n_pfds; i++){
            if(pfds[i].revents == 0) continue;

            if(pfds[i].revents && POLLIN){
                // i==0 means this is pfd of DS server, so accept new connection
                if(i == 0){
                    len = sizeof(caddr);
                    int nsfd = accept(dsfd, (struct sockaddr *)&caddr, &len);
                    if (nsfd == -1) {perror("accept");return 1;}

                    char option;
                    recv(nsfd, &option, sizeof(option), 0);

                    int optr = option - '0';
                    
                    printf("Client requested D%d\n", optr);

                    // first time calling Di
                    if(clients_connected_count[optr] == 0){
                        char process[10]={0};
                        sprintf(process,"./D%c.exe",option);
                        int d_fd = fileno(popen(process, "r"));
                        if(d_fd == -1){perror("popen");return 1;}

                        pfds[n_pfds].fd = d_fd;
                        pfds[n_pfds].events = POLLIN;

                        printf("created D%d\n", optr);
                        d_number[n_pfds] = optr;
                        n_pfds++;
                    }else{
                        // Di already exists
                        printf("D%d already exists\n", optr);
                    }
                    
                    clients_connected[optr][clients_connected_count[optr]++] = nsfd;
                }else{
                    // i>=1, means Di is sending some data output
                    char msg[100]={0};
                    if(read(pfds[i].fd, msg, sizeof(msg)) == -1){
                        perror("read");
                        return 1;
                    }

                    // send this message to all clients which connected to Di
                    int d_i = d_number[i];
                    for(int j=0; j<clients_connected_count[d_i]; j++){
                        // MSG_NOSIGNAL : if client is exited, dont send SIGPIPE error
                        if(send(clients_connected[d_i][j], msg, sizeof(msg), MSG_NOSIGNAL) == -1){
                            printf("send error\n");
                            perror("send");
                            continue;
                        }
                    }
                }
            }
        }
    }



    return 0;
}