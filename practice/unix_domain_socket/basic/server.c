#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */

#define SOCK_PATH "mysocket"

int serv_listen() {
    int fd;
    struct sockaddr_un addr;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCK_PATH);
    unlink(SOCK_PATH);
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }

    if (listen(fd, 5) < 0) {
        perror("listen");
        return -1;
    }

    return fd;
}

int main(){
    int usfd = serv_listen();
    if (usfd < 0) { return -1;}

    struct sockaddr_un cli_addr;
    socklen_t cli_len = sizeof(cli_addr);

    int unsfd = accept(usfd, (struct sockaddr *)&cli_addr, &cli_len);
    if (unsfd < 0) {
        perror("accept");
        return -1;
    }

    char *msg = "Hello, client!\n";
    send(unsfd, msg, strlen(msg), 0);

    char buff[1024];
    recv(unsfd, buff, sizeof(buff), 0);
    printf("%s", buff);

    close(unsfd);
    close(usfd);
    unlink(SOCK_PATH);
    return 0;
}