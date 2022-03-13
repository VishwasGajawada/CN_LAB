
#include <bits/stdc++.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/LSOCK_Acceptor.h>
#include <ace/LSOCK_Stream.h>
#include <ace/LSOCK_Connector.h>
#include <ace/Handle_Set.h>
#include <time.h>



#define NUM_WAITERS 3

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int cli_uds_conn(char *path) ;
int serv_tcp_listen(int port) ;
int recv_fd(int socket) ;

int main() {
    ACE_INET_Addr daddr(8081, INADDR_LOOPBACK);
    ACE_SOCK_Acceptor dacc(daddr);
    // int dsfd = serv_tcp_listen(8081);
    // if(dsfd < 0) handle_error("serv_tcp_listen");

    ACE_UNIX_Addr baddr("d");
    ACE_LSOCK_Connector bcon;
    ACE_LSOCK_Stream bstream;
    if( bcon.connect(bstream, baddr) == -1) handle_error("");
    // int busfd = cli_uds_conn("d");

    ACE_SOCK_Stream wstream[NUM_WAITERS];
    // int wnsfd[NUM_WAITERS];

    ACE_Handle_Set rset;
    // struct pollfd pfd[NUM_WAITERS];
    for(int i = 0; i < NUM_WAITERS; i++) {
        dacc.accept(wstream[i]);
        // wnsfd[i] = accept(dsfd, NULL, NULL);
        rset.set_bit(wstream[i].get_handle());
        // pfd[i].fd = wnsfd[i];
        // pfd[i].events = POLLIN;
    }

    while(1) {
        ACE_HANDLE h;
        bstream.recv_handle(h);
        // int fd = recv_fd(busfd);
        char combo_items[20];
        // any waiter can send. so do poll
        ACE_Handle_Set copyset = rset;
        ACE::select(ACE_DEFAULT_SELECT_REACTOR_SIZE, copyset);
        for(int i=0; i<NUM_WAITERS; i++) {
            if(copyset.is_set(wstream[i].get_handle())) {

                char buf[100] = {0};
                wstream[i].recv_n(buf, sizeof(buf));
                break;
            }
        }
        // poll(pfd, NUM_WAITERS, -1);
        // for(int i = 0; i < NUM_WAITERS; i++) {
        //     if(pfd[i].revents & POLLIN) {
        //         recv(wnsfd[i], combo_items, sizeof(combo_items), 0);
        //         break;
        //     }
        // }

        printf("Combo items: %s\n", combo_items);
        // handover parcel to customer

        ACE_SOCK_Stream stream (h);
        stream.send_n(combo_items, sizeof(combo_items));
        // send(h, combo_items, sizeof(combo_items), 0);
    }

    return 0;
}


