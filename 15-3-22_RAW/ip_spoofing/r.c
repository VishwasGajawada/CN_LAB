#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

void print_ip_header(struct ip *iph) {
    printf("header length:%d\n", iph->ip_hl);
    printf("version:%d\n", iph->ip_v);
    printf("TOS:%d\n", iph->ip_tos);
    printf("IP Len:%d\n", iph->ip_len);
    printf("id: %d\n", iph->ip_id);
    printf("offest:%d\n", iph->ip_off);
    printf("TTL:%d\n", iph->ip_ttl);
    printf("Prot No:%d\n", iph->ip_p);
    printf("Sum:%d\n", iph->ip_sum);
    printf("Src addr:%s\n", inet_ntoa(iph->ip_src));
    printf("Dest addr:%s\n", inet_ntoa(iph->ip_dst));
    printf("TOS:%d\n", iph->ip_tos);

    char *message = (char *)(iph)+(iph->ip_hl<<2);

    printf ("Message : %s\n\n\n", message);
}


int main(){
    int rsfd=socket (AF_INET, SOCK_RAW, 2);	
    if(rsfd < 0) handle_error("socket");

    char buff[1024];
    recvfrom(rsfd,buff,sizeof(buff),0,NULL,NULL);
    struct ip* ip;
    ip=(struct ip*)buff;

    print_ip_header(ip);
    return 0;
}


