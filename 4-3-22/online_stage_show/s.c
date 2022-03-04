#include <stdio.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h> /* unlink */
#include <arpa/inet.h> /* inet_pton */
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int serv_tcp_listen(int port) ;
int pidof(char *name);

/* stage */
int main() {
    int sfd;
    struct sockaddr_in addr;

    sfd = serv_tcp_listen(8080);
    if(sfd < 0) handle_error("serv_tcp_listen");

    int apid = pidof("a.exe");

    int nsfd;
    struct sockaddr_in caddr;
    socklen_t len = sizeof(caddr);

    while(1) {
        nsfd = accept(sfd, (struct sockaddr *)&caddr, &len);
        if(nsfd < 0) handle_error("accept");

        printf("performance started\n\n");
        char buf[100] = {0};
        while(1) {
            memset(buf, 0, sizeof(buf));
            int n = recv(nsfd, &buf, sizeof(buf), 0);
            if(n <= 0) break;
            if(strcmp(buf, "exit\n") == 0) break;
            printf("%s", buf);
        }
        sleep(1);
        // signal Anchor 
        kill(apid, SIGUSR1);

        // print P's answers
        while(1) {
            memset(buf, 0, sizeof(buf));
            int n = recv(nsfd, &buf, sizeof(buf), 0);
            if(n <= 0) break;
            if(strcmp(buf, "exit") == 0) break;
            printf("%s", buf);
        }

    }


    return 0;
}

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

int pidof(char *name) {
    char buf[100] = {0};
    sprintf(buf, "pidof %s", name);
    int fd = fileno(popen(buf, "r"));	
    char s[100];
	read(fd, &s, 1000);
	return atoi(s); 
}

