#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <arpa/inet.h> /* inet_pton */
#include <fcntl.h>
#include <stdlib.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int serv_uds_listen(char *path) ;

/* performer */
int main() {
    /* connect to sfd */
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd <0 )handle_error("socket");

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if( connect(sfd, (struct sockaddr *)&saddr, sizeof(saddr)) )
        handle_error("connect");
    
    printf("Starting my performance...\n");
    while(1) {
        char buf[100] = {0};
        fgets(buf, sizeof(buf), stdin);
        send(sfd, buf, sizeof(buf), 0);
        if(strcmp(buf, "exit\n") == 0) break;

    }
    
    // now answer questions by judges
    int usfd = serv_uds_listen("stage_perform");
    if(usfd < 0) handle_error("serv_uds_listen");

    int unsfd = accept(usfd, NULL, NULL);
    while(1) {
        char buf[100] = {0};
        recv(unsfd, buf, sizeof(buf), 0);
        if(strcmp(buf, "exit") == 0) {
            send(sfd, buf, sizeof(buf), 0);
            break;
        }
        printf("\nQuestion: %s", buf);
        if(strcmp(buf, "exit\n") == 0) {
            send(sfd, buf, sizeof(buf), 0);
            break;
        }
        printf("Answer: ");
        fgets(buf, sizeof(buf), stdin);
        send(sfd, buf, sizeof(buf), 0);
    }

    return 0;
}

int serv_uds_listen(char *path) {
    int fd;
    struct sockaddr_un addr;

    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        perror("socket");
        return -1;
    }

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, path);
    unlink(path);
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

