#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <strings.h> /* bzero */

#include <sys/poll.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define MAX_NSFD 10
// first cum first basis order of nsfd clients
int nsfds[MAX_NSFD];
int nsfd_start = 0, nsfd_count = 0;

void sig_usr1_handler(int signo);
int sock_tcp_init();
int p1_init();
int p2_init();
int p3_init();
void p4_init();

int main() {
    signal(SIGUSR1, sig_usr1_handler);
    printf("My pid is %d\n", getpid());
    int sfd = sock_tcp_init();
    int pfd = p1_init();
    printf("p1_init() returned %d\n", pfd);
    int ffd = -1;
    ffd = p2_init();
    printf("p2_init() returned %d\n", ffd);
    int fd = p3_init();
    printf("p3_init() returned %d\n", fd);


    printf("init done\n");
    struct sockaddr_in caddr;
    socklen_t caddr_len = sizeof(caddr);

    
    int read_fds[5] = {sfd, 0, pfd, ffd, fd};
    struct pollfd ppfd[5];
    for(int i=0; i<5; i++ ){
        ppfd[i].fd = read_fds[i];
        ppfd[i].events = POLLIN;
    }
    while(1) {
        int c_start = nsfd_start, c_end = nsfd_count;
        int ready = poll(ppfd, 5, -1);
        for(int i=0; i<5; i++) {
            if(ppfd[i].revents == 0) continue;
            if(ppfd[i].revents & POLLIN) {
                if(i == 0) {
                    bzero(&caddr, sizeof(caddr));
                    int c_sfd = accept(read_fds[0], (struct sockaddr*)&caddr, &caddr_len);
                    if(c_sfd == -1) {
                        perror("accept");
                        continue;
                    }
                    printf("Client accepted\n");
                    nsfds[nsfd_count] = c_sfd;
                    nsfd_count++;
                }else {
                    char msg[100] = {0};
                    int rd = read(read_fds[i], msg, sizeof(msg));
                    if(rd == -1) {
                        perror("read");
                        continue;
                    }
                    if(rd > 0) {
                        // printf("Got %d\n", i);
                        for(int j=c_start; j<c_end; j++) {
                            // MSG_NOSIGNAL : if client is exited, dont send SIGPIPE error
                            if(send(nsfds[j], msg, sizeof(msg), 0) == -1) {
                                // perror("write");
                                continue;
                            }
                        }
                    }
                }
            }
        }
    }
    

    /*
    // similar functionality with select
    fd_set rset, copyset;
    int read_fds[5] = {sfd, 0, pfd, ffd, fd};

    int max_fd = sfd;
    for(int i = 0; i < 5; i++) {
        FD_SET(read_fds[i], &rset);
        if(read_fds[i] > max_fd)
            max_fd = read_fds[i];
    }
    while(1) {
        int c_start = nsfd_start, c_end = nsfd_count;
        FD_ZERO(&copyset);
        copyset = rset;
        // sleep(2);
        // printf("HI\n");

        
        select(max_fd+1, &copyset, NULL, NULL, NULL);
        for(int i=0; i<5; i++) {
            if(FD_ISSET(read_fds[i], &copyset)) {
                if(i == 0) {
                    bzero(&caddr, sizeof(caddr));
                    int c_sfd = accept(read_fds[0], (struct sockaddr*)&caddr, &caddr_len);
                    if(c_sfd == -1) {
                        perror("accept");
                        continue;
                    }
                    printf("Client accepted\n");
                    nsfds[nsfd_count] = c_sfd;
                    nsfd_count++;
                }else {
                    char msg[100] = {0};
                    int rd = read(read_fds[i], msg, sizeof(msg));
                    if(rd == -1) {
                        perror("read");
                        continue;
                    }
                    if(rd > 0) {
                        // printf("Got %d\n", i);
                        for(int j=c_start; j<c_end; j++) {
                            // MSG_NOSIGNAL : if client is exited, dont send SIGPIPE error
                            if(send(nsfds[j], msg, sizeof(msg), 0) == -1) {
                                // perror("write");
                                continue;
                            }
                        }
                    }
                }
            }
        }
    }
    */
    

    return 0;
}

void sig_usr1_handler(int signo) {
    // connect nsfd from nsfd array on first cum first basis to E server
    if(nsfd_start >= nsfd_count) return;
    int nsfd = nsfds[nsfd_start];
    nsfd_start++;

    // char msg[20] = "echo_start";
    // send(nsfd, msg, sizeof(msg), 0);

    int c = fork();
    if(c > 0) {
        // close(nsfd);
    }else if(c == 0) {
        // dup2(nsfd, 0);
        dup2(nsfd, 1);
        char *args[] = {NULL};
        execvp("./E.exe", args);
        exit(1);
    }
}

int sock_tcp_init() {
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd < 0) handle_error("socket");

    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(8080);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int opt = 1;
    // Forcefully attaching socket to the port, so that bind already in use error doesnt occur
    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        return 1;
    }

    int bind_status = bind(sfd, (struct sockaddr*)&saddr, sizeof(saddr));
    if(bind_status < 0) handle_error("bind");

    int listen_status = listen(sfd, 5);
    if(listen_status < 0) handle_error("listen");

    return sfd;
}

int p1_init() {
    int fd[2];
    pipe(fd);
    int c = fork();
    if(c > 0) {
    }else if(c == 0) {
        dup2(fd[1], 1);
        char *argv[] = {NULL};
        execvp("./P1.exe", argv);
        handle_error("execvp");
        exit(1);
    }
    return fd[0];
}

/* mkfifo only works when there is both reading and writing process available */
/* so first call p2.exe before S.exe */
int p2_init() {
    if(mkfifo("/tmp/aio-fifo", 0666) < 0) {
        if(errno != EEXIST)
            handle_error("mkfifo");
    }
    int ffd = open("/tmp/aio-fifo", O_RDONLY);
    if(ffd <0) handle_error("open");

    return ffd;
}

int p3_init() {
    int fd = fileno(popen("./P3.exe", "r"));
    if(fd < 0) handle_error("popen");
    return fd;
}
void p4_init() {
    return ;
}





