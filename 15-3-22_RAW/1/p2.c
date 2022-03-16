#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/ip.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int main(){
    int rsfd=socket (AF_INET, SOCK_RAW, 2);	
    if(rsfd < 0) handle_error("socket");

    char buff[100];
    printf("%d\n",rsfd);
    recvfrom(rsfd,buff,100,0,NULL,NULL);
    struct iphdr* ip;
    ip=(struct iphdr*)buff;
    printf("%s\n",buff+(ip->ihl)*4);
    return 0;
}
