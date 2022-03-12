
#include <bits/stdc++.h>
#include <ace/Signal.h>
#include <ace/Sig_Handler.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/LSOCK_Acceptor.h>
#include <ace/LSOCK_Stream.h>
#include <ace/LSOCK_Connector.h>
#include <time.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

#define NUM_WAITERS 3

int signal_recieved = -1;

class MySignalHandler : public ACE_Event_Handler {
public:
  MySignalHandler (int signum) : signum_(signum) { }

  virtual ~MySignalHandler() { }

  virtual int handle_signal (int signum, siginfo_t * = 0, ucontext_t * = 0) {
    printf("signal received\n");
    fflush(stdout);
    signal_recieved = 1;
    return 0;
  }

private:
  int signum_;
};


int serv_tcp_listen(int port) ;
int serv_uds_listen(char *path) ;
int pidof(char *name);
int getWaiter();
int send_fd(int socket, int fd_to_send);
void sigusr1_handler(int signo);


/* stage */
int main() {
    MySignalHandler h (SIGUSR1);
    ACE_Sig_Handler handler;
    handler.register_handler (SIGUSR1, &h);

    time_t t;
    srand((unsigned) time(&t));

    ACE_INET_Addr saddr(8080, INADDR_LOOPBACK);
    ACE_UNIX_Addr daddr("d");
    ACE_SOCK_Acceptor sacc(saddr);
    ACE_LSOCK_Acceptor wacc[NUM_WAITERS], dacc(daddr);
    ACE_LSOCK_Stream wstream[NUM_WAITERS], dstream;

    dacc.accept(dstream);

    // /* connection with delivery boy */
    // dsfd = serv_uds_listen("d");
    // if(dsfd < 0) handle_error("serv_uds_listen");
    // dnsfd = accept(dsfd, NULL, NULL);
    
    
    /* connection with waiters */
    for(int i = 0; i < NUM_WAITERS; i++) {
        char buf[10];
        sprintf(buf, "w%d", i+1);
        ACE_UNIX_Addr waddr(buf);
        wacc[i].accept(wstream[i]);

        // wsfd[i] = serv_uds_listen(buf);
        // if(wsfd[i] < 0) handle_error("serv_uds_listen");

        // wnsfd[i] = accept(wsfd[i], NULL, NULL);
    }

   

    ACE_SOCK_Stream nstream;
    // int nsfd;
    while(1) {
        sacc.accept(nstream);
        // nsfd = accept(sfd, NULL, NULL);
        // if(nsfd < 0) handle_error("accept");

        char combo[10];
        nstream.recv_n(combo, sizeof(combo));
        // recv(nsfd, combo, sizeof(combo), 0);
        printf("Clients combo = %s\n", combo);

        int w = getWaiter();
        // pass customer
        wstream[w].send_handle(nstream.get_handle());
        // send_fd(wnsfd[w], nsfd);

        // send combo number
        wstream[w].send_n(combo, sizeof(combo));
        // send(wnsfd[w], combo, sizeof(combo), 0);
        printf("passed customer and his combo to waiter %d\n", w+1);

        // pause for signal from waiter
        // pause();
        while(signal_recieved == -1) {}

        // set back for next iterations
        signal_recieved = -1;

        // pass customer to delivery
        dstream.send_handle(nstream.get_handle());
        // send_fd(dnsfd, nsfd);
    }


    return 0;
}

int getWaiter() {
    return rand() % NUM_WAITERS;
}
