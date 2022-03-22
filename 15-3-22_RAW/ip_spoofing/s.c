#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <netinet/ether.h>

/* checksum code to calculate UDP checksum
* Code taken from Unix network programming – Richard stevens*/
unsigned short csum (uint16_t * addr, int len) {
    int nleft = len;
    unsigned int sum = 0;
    unsigned short *w = addr;
    unsigned short answer = 0;

    /* Our algorithm is simple, using a 32 bit accumulator (sum), we add
    * sequential 16 bit words to it, and at the end, fold back all the
    * carry bits from the top 16 bits into the lower 16 bits.
    */
    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nleft == 1) {
        *(unsigned char *) (&answer) = * (unsigned char *) w;
        sum += answer;
    }

    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff); /* add hi 16 to low 16 */
    sum += (sum >> 16); /* add carry */
    answer = (unsigned short) ~sum; /* truncate to 16 bits */
    return (answer);
}

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

int init_raw_socket(int proto, struct sockaddr_in *saddr) {
    int rsfd;

    rsfd = socket(AF_INET, SOCK_RAW, proto);
    if(rsfd < 0) {
        perror("rsfd");
        exit(0);
    }

    (*saddr).sin_family = AF_INET;
    (*saddr).sin_addr.s_addr = inet_addr("172.30.143.26"); /* found by running ifconfig */

    return rsfd;
}

void send_packet(int rsfd, int proto, char buf[], struct sockaddr_in *saddr) {
    char packet[1024] = {0}; 

    struct ip *iph = (struct ip *)packet;

    iph->ip_hl = 5;
    iph->ip_v = 4;
    iph->ip_tos = 0;
    iph->ip_len = sizeof(struct ip) + strlen(buf);
    iph->ip_id = htons(54321);
    iph->ip_off = 0;
    iph->ip_ttl = 255;
    iph->ip_p = proto;
    iph->ip_sum = 0;
    iph->ip_src.s_addr = inet_addr("1.2.3.4"); // spoofed
    iph->ip_dst.s_addr = (*saddr).sin_addr.s_addr; // target

    iph->ip_sum = csum((unsigned short *)packet, iph->ip_len >> 1);

    char *message = (char *)(iph)+(iph->ip_hl*4);
    strcpy(message, buf);

    if(sendto(rsfd, packet, iph->ip_len, 0, (struct sockaddr *)saddr, sizeof(*saddr)) < 0) {
        perror("sendto");
        exit(0);
    }

}

int main(){
    int rsfd;
    struct sockaddr_in saddr;

    rsfd = init_raw_socket(2, &saddr);

    int op  = 1;
    if(setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &op, sizeof(op)) < 0) {
        perror("setsockopt");
        exit(0);
    }

    char buf[100] = "Hello World";

    // while(1)
    send_packet(rsfd, 2, buf, &saddr);

    return 0;
}

