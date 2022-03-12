#include "header.h"
int main(){
    ACE_INET_Addr addr(8080, INADDR_LOOPBACK);
	ACE_SOCK_Stream sfd;
	ACE_SOCK_Connector con;

	con.connect(sfd, addr);
    sfd.send_n("I am performing on the stage....\n",100);
    sfd.send_n("Performance done!!..\n",100);

    // wait for a judge to ask question
    char buff[100] = {0};
    ACE_INET_Addr addr1(8081, INADDR_LOOPBACK);
	ACE_SOCK_Stream sfd1;

	ACE_SOCK_Acceptor acc1(addr1);
	acc1.accept(sfd1);
    int sz = sfd1.recv_n(buff,100);
    buff[sz] = '\0';
    printf("%s\n",buff);
    sfd.send_n("My name is Performer",100);
    return 0;
}


