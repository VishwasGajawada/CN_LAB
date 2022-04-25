#include<time.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<sys/select.h>
#include<pthread.h>
#include<signal.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/shm.h>
#include<unistd.h>
#include<sys/un.h>
#include<netinet/ip.h>
#include<arpa/inet.h>
#include<pcap.h>
#include<errno.h>
#include<netinet/if_ether.h>
#include<net/ethernet.h>
#include<netinet/ether.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>

int main(){

    int sfd;    
    sfd = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(12347);
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(sfd,(struct sockaddr*) &serv_addr,sizeof(serv_addr));

    char buf1[1024];
    recv(sfd,buf1,sizeof(buf1),0);
    printf("Message in c3 is: %s\n",buf1);

    send(sfd,"Hi Server, Im client 3",22,0);

    char buf2[1024];
    recv(sfd,buf2,sizeof(buf2),0);
    
    return 0;
}