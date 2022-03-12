#include <bits/stdc++.h>
#include <ace/SOCK_Connector.h>
#include <ace/SOCK_Stream.h>
#include <ace/Handle_Set.h>

using namespace std;

int main(int argc, char **argv) {
    int port = (argc > 1) ? 8080+atoi(argv[1]) : 8080;
    ACE_INET_Addr saddr(port, INADDR_LOOPBACK);
    ACE_SOCK_CONNECTOR sfd;
    ACE_SOCK_Stream stream;
    sfd.connect(stream, saddr);

    char buf[100];
    scanf("%s", buf);
    stream.send_n(buf, sizeof(buf));

    stream.recv_n(buf, sizeof(buf));
    strcat(buf, "\n");
    printf("%s", buf);
    stream.close();
    return 0;
}