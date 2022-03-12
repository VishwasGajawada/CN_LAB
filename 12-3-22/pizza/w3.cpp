
#include <bits/stdc++.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/LSOCK_Acceptor.h>
#include <ace/LSOCK_Stream.h>
#include <ace/LSOCK_Connector.h>
#include <time.h>


#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int pidof(char *name);

int main() {
    ACE_UNIX_Addr baddr("w3");
    ACE_LSOCK_Connector bcon;
    ACE_LSOCK_Stream bstream;
    bcon.connect(bstream, baddr);
    // int busfd = cli_uds_conn("w1");

    ACE_INET_Addr daddr(8081, INADDR_LOOPBACK);
    ACE_SOCK_Connector dcon;
    ACE_SOCK_Stream dstream;
    dcon.connect(dstream, daddr);
    // int dsfd = cli_tcp_connect(8081);
    int bpid = pidof("b.exe");

    while(1) {
        ACE_HANDLE h;
        int fd = bstream.recv_handle(h);
        // int fd = recv_fd(busfd);
        printf("fd = %d\n", fd);
        if(fd < 0) {
            printf("Shop closed\n");
            break;
        }

        char combo[10];
        bstream.recv_n(combo, sizeof(combo));
        // recv(busfd, combo, sizeof(combo), 0);
        printf("combo = %s\n", combo);

        char combo_items[20];
        // sprintf(combo_items, "%s_items", combo);
        recv(fd, combo_items, sizeof(combo_items), 0);
        printf("combo_items = %s\n", combo_items);

        sleep(1);
        kill(bpid, SIGUSR1);

        dstream.send_n(combo_items, sizeof(combo_items));
        // send(dsfd, combo_items, sizeof(combo_items), 0);
    }


    return 0;
}

int pidof(char *name) {
    char buf[100] = {0};
    sprintf(buf, "pidof %s", name);
    int fd = fileno(popen(buf, "r"));	
    char s[100];
	read(fd, &s, 1000);
	return atoi(s); 
}

