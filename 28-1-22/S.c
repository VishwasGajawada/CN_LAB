#include <stdio.h>
#include <string.h> // strlen
#include <sys/socket.h>
#include <arpa/inet.h> // inet_pton
#include <unistd.h>
#include <sys/wait.h> // wait
#include <stdlib.h> // system
#include <fcntl.h> // open
#include <stdbool.h> // boolean

void compareTestCase(char *expected, char *actual, int nsfd) {
    int efd = open(expected, O_RDONLY);
    int afd = open(actual, O_RDONLY);

    char ebuffer[1024] = {0};
    char abuffer[1024] = {0};

    // comapre expected and actual outputs
    bool same = true;
    while(read(efd, ebuffer, sizeof(ebuffer)) > 0) {
        read(afd, abuffer, sizeof(abuffer));
        if (strcmp(ebuffer, abuffer) != 0) {
            printf("abuffer: '%s'\n", abuffer);
            printf("ebuffer: '%s'\n", ebuffer);
            printf("Actual output does not match expected output\n");

            same = false;
            break;
        }
    }

    if(same){
        char *msg = "Test case passed\n";
        send(nsfd, msg, strlen(msg), 0);        
    }else{
        char *msg = "Test case failed\n";
        send(nsfd, msg, strlen(msg), 0);
    }
    
}

int main(){
    int sfd, nsfd, itfd, poutfd;
    struct sockaddr_in saddr, caddr;
    socklen_t caddr_len;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {perror("socket");return 1;}

    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // bind
    int opt = 1;
    // so that bind already in use error doesnt occur
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        return 1;
    }

    int bind_status = bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if (bind_status == -1) {perror("bind");return 1;}

    // listen
    int listen_status = listen(sfd, 5);
    if (listen_status == -1) {perror("listen");return 1;}

    // accept
    caddr_len = sizeof(caddr);
    nsfd = accept(sfd, (struct sockaddr*)&caddr, &caddr_len);
    if (nsfd == -1) {perror("accept");return 1;}

    // recv name of file to be compiled and executed
    char file[256] = {0};
    recv(nsfd, file, sizeof(file), 0);

    // find index of '.', to get filename without extension
    for(int i=0; i < strlen(file); i++){
        if(file[i] == '.'){
            file[i] = '\0';
            break;
        }
    }
    char command[1024] = {0};
    sprintf(command, "g++ %s.cpp -o %s.exe", file, file);

    // compile P.cpp to P.exe
    system(command);

    int c = fork();
    if(c>0){
        wait(NULL);
    }else{
        itfd = open("IT.txt", O_RDONLY);
        poutfd = open("Pout.txt", O_WRONLY | O_CREAT | O_TRUNC, 0666);

        dup2(itfd, 0);
        dup2(poutfd, 1);

        char *args[] = {NULL};
        char exec_command[1024] = {0};
        sprintf(exec_command, "./%s.exe", file);

        execvp(exec_command, args);

        exit(0);
    }

    compareTestCase("OT.txt", "Pout.txt", nsfd);

    // close fds
    close(nsfd);
    close(sfd);
    close(itfd);
    close(poutfd);
    
    return 0;
}