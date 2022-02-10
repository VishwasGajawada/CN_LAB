#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */

#define SOCK_PATH "mysocket"

int cli_conn() {
    int fd;
    struct sockaddr_un un;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    bzero(&un, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, SOCK_PATH);
    if(connect(fd, (struct sockaddr *)&un, sizeof(un)) < 0) {
        perror("connect");
        return -1;
    }

    return fd;
}

int main(){
    int usfd = cli_conn();

    char buff[1024];
    if( recv(usfd, buff, sizeof(buff), 0) < 0) {
        perror("recv");
        return -1;
    }

    printf("%s", buff);

    char *msg = "Hello, server!\n";
    send(usfd, msg, strlen(msg), 0);


    close(usfd);
    unlink(SOCK_PATH);
    return 0;
}