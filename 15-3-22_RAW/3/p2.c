#include <stdio.h>
#include <stdlib.h>
#include<arpa/inet.h> // for inet_ntoa()
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/ip.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void print_ipheader(struct iphdr* ip) {
	printf("---------------------\n");
	printf("Printing IP header.\n");
	printf("IP version:%u\n",(unsigned int)ip->version);
	printf("IP header length:%u\n",(unsigned int)ip->ihl);
	printf("Type of service:%u\n",(unsigned int)ip->tos);
	printf("Total ip packet length:%d\n",ntohs(ip->tot_len));
	printf("Packet id:%d\n",ntohs(ip->id));
	printf("Time to leave :%d\n",(unsigned int)ip->ttl);
	printf("Protocol:%d\n",(unsigned int)ip->protocol);
	printf("Check:%d\n",ip->check);
	printf("Source ip:%s\n",inet_ntoa(*(struct in_addr*)&ip->saddr));
	//printf("%pI4\n",&ip->saddr );
	printf("Destination ip:%s\n",inet_ntoa(*(struct in_addr*)&ip->daddr));
	printf("End of IP header\n");
	printf("---------------------\n");
}

int main(){
    int rsfd=socket (AF_INET, SOCK_RAW, 2);	
    if(rsfd < 0) handle_error("socket");

    char buff[100];
    printf("%d\n",rsfd);
    recvfrom(rsfd,buff,100,0,NULL,NULL);
    struct iphdr* ip;
    ip=(struct iphdr*)buff;
    printf("%s\n",buff+(ip->ihl)*4);
    print_ipheader(ip);
    return 0;
}
