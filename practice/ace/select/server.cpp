#include <bits/stdc++.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Stream.h>
#include <ace/Handle_Set.h>
#include <stdio.h>

using namespace std;

int main() {
    ACE_INET_Addr saddr1(8080, INADDR_LOOPBACK), saddr2(8081, INADDR_LOOPBACK);
    ACE_SOCK_ACCEPTOR sfd1, sfd2;
    sfd1.open(saddr1);
    sfd2.open(saddr2);


    ACE_Handle_Set rset;
    rset.set_bit(sfd1.get_handle());
    rset.set_bit(sfd2.get_handle());

    ACE_SOCK_ACCEPTOR sfds[] = {sfd1, sfd2};
    while(1) {
        ACE_Handle_Set copyset = rset;
        ACE::select(ACE_DEFAULT_SELECT_REACTOR_SIZE, copyset);
        for(int i=0; i<2; i++) {
            if(copyset.is_set(sfds[i].get_handle())) {
                ACE_SOCK_Stream stream;
                sfds[i].accept(stream);

                int curport = 8080+i; 
                printf("port %d connected\n", curport);

                char buf[100] = {0};
                stream.recv_n(buf, sizeof(buf));
                printf("%s from port %d\n", buf, curport);

                char reply[110] = {0};
                sprintf(reply, "%s_%d", buf, curport);
                stream.send_n(reply, sizeof(reply));

                stream.close();
            }
        }
    }
    return 0;
}

// export LD_LIBRARY_PATH=:/home/vishwas/SOFTWARE/smartsoft/lib