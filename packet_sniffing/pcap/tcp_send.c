#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


int serv_tcp_listen(int port) {
    int fd;
    struct sockaddr_in addr;

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int opt = 1;
    // Forcefully attaching socket to the port, so that bind already in use error doesnt occur
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        return 1;
    }
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


int main() {
    char msg[100] = "hello world";

    int sfd = serv_tcp_listen(8080);

    // accept
    int cfd = accept(sfd, NULL, NULL);

    while(1) {

        // send
        send(cfd, msg, sizeof(msg), 0);

        // recieve
        char buf[100];
        recv(cfd, buf, sizeof(buf), 0);
        printf("%s\n", buf);

        sleep(1);
    }
    // close
    close(sfd);
    close(cfd);
    
    return 0;
}