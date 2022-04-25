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

    int sfd1;
    sfd1 = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serv_addr1;
    serv_addr1.sin_family = AF_INET;
    serv_addr1.sin_port = htons(12345);
    serv_addr1.sin_addr.s_addr = INADDR_ANY;
    bind(sfd1,(struct sockaddr*) &serv_addr1,sizeof(serv_addr1));
    listen(sfd1,10);

    int sfd2;
    sfd2 = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serv_addr2;
    serv_addr2.sin_family = AF_INET;
    serv_addr2.sin_port = htons(12346);
    serv_addr2.sin_addr.s_addr = INADDR_ANY;
    bind(sfd2,(struct sockaddr*) &serv_addr2,sizeof(serv_addr2));
    listen(sfd2,10);

    int sfd3;
    sfd3 = socket(AF_INET,SOCK_STREAM,0);
    struct sockaddr_in serv_addr3;
    serv_addr3.sin_family = AF_INET;
    serv_addr3.sin_port = htons(12347);
    serv_addr3.sin_addr.s_addr = INADDR_ANY;
    bind(sfd3,(struct sockaddr*) &serv_addr3,sizeof(serv_addr3));
    listen(sfd3,10);

    int len;
    struct sockaddr_in cli_addr;
    len = sizeof(cli_addr);
    int nsfd1;
    nsfd1 = accept(sfd1,(struct sockaddr*) &cli_addr,&len);

    len = sizeof(cli_addr);
    int nsfd2;
    nsfd2 = accept(sfd2,(struct sockaddr*) &cli_addr,&len);

    len = sizeof(cli_addr);
    int nsfd3;
    nsfd3 = accept(sfd3,(struct sockaddr*) &cli_addr,&len);

    send(nsfd1,"Hello Client 1",15,0);
    send(nsfd2,"Hello Client 2",15,0);
    send(nsfd3,"Hello Client 3",15,0);

    char buf1[1024];
    recv(nsfd1,buf1,sizeof(buf1),0);

    char buf2[1024];
    recv(nsfd2,buf2,sizeof(buf2),0);

    char buf3[1024];
    recv(nsfd3,buf3,sizeof(buf3),0);

    send(nsfd1,"Amazon",6,0);
    send(nsfd2,"Microsoft",9,0);
    send(nsfd3,"Oracle",6,0);
    

    return 0;
}