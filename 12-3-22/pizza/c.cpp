
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


/* stage */
int main() {
    ACE_INET_Addr addr(8080, INADDR_LOOPBACK);
    ACE_SOCK_Stream sstream;
    ACE_SOCK_Connector con;
    con.connect(sstream, addr);
    // int sfd = cli_tcp_connect(8080);

    char combo[10] = {0};
    printf("Enter combo number (1-3) : ");
    scanf("%s", combo);
    sstream.send_n(combo, sizeof(combo));
    // send(sfd, combo, sizeof(combo), 0);

    printf("Enter your items (%s) : ", combo);
    char items[100] = "biryani";
    scanf("%s", items);
    sstream.send_n(items, sizeof(items));
    // send(sfd, items, sizeof(items), 0);

    char combo_items[20];
    sstream.recv_n(combo_items, sizeof(combo_items));
    // recv(sfd, combo_items, sizeof(combo_items), 0);

    printf("Combo items: %s\n", combo_items);


    return 0;
}
