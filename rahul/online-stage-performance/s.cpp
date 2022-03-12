#include "header.h"
int main(){
    ACE_INET_Addr addr(8080, INADDR_LOOPBACK);
	ACE_SOCK_Stream sfd;

	ACE_SOCK_Acceptor acc(addr);
	acc.accept(sfd);

    char buff[100];
    sfd.recv_n(buff,100);

    // recv(nsfd,buff,100,0);
    //while(buff[0]!='0'){
        printf("%s",buff);
        sfd.recv_n(buff,100);
        printf("%s",buff);

    //}
    //notify anchor/*
    
    int fd = fileno(popen("pidof ./a", "r"));
    char s[1000];
    read(fd, &s, 1000);
    int pidofA = atoi(s);
    ACE_OS::kill(pidofA,SIGUSR1);
    // receive answer from performer
    sfd.recv_n(buff,100);
    // recv(nsfd,buff,100,0);
    printf("%s\n",buff);
    return 0;
}