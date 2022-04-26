#define __USE_BSD	/* use bsd'ish ip header */
#include <sys/socket.h>	/* these headers are for a Linux system, but */
#include <netinet/in.h>	/* the names on other systems are easy to guess.. */
#include <netinet/ip.h>
#define __FAVOR_BSD	/* use bsd'ish tcp header */
#include <netinet/tcp.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

unsigned short csum (unsigned short *buf, int nwords) {
    unsigned long sum;
    for (sum = 0; nwords > 0; nwords--)
        sum += *buf++;
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}

void print_ip_header(struct ip *iph) {
    printf ("\n\nIP HEADER\n\n");

    printf ("Version : %d\n", iph->ip_v);
    printf ("Header Length : %d\n", iph->ip_hl);
    printf ("TOS (Type of Service): %d\n", iph->ip_tos);
    printf ("IP Packet Length : %d\n", iph->ip_len);
    printf ("ID Number : %d\n", iph->ip_id);
    printf ("Fragment Offset : %d\n", iph->ip_off);
    printf ("TTL (Time to Live) : %d\n", iph->ip_ttl);
    printf ("HLP (Higher Layer Protocol) : %d\n", iph->ip_p);
    printf ("Header Checksum : %d\n", iph->ip_sum);
    printf ("Sender's IP Address : %s\n", inet_ntoa(iph->ip_src));
    printf ("Receiver's IP Address : %s\n", inet_ntoa(iph->ip_dst));
}

void print_TCP_header (char buff[], int buff_size) {
    struct ip *iph = (struct ip*) buff;

    print_ip_header (iph);

    struct tcphdr *tcph = (struct tcphdr*) (buff + (iph->ip_hl * 5));

    printf ("\n\n\nTCP HEADER\n\nSource : %u\n", ntohs (tcph->source));
    printf ("Destination Port : %u\n", ntohs (tcph->dest));
    printf ("Sequence Number : %u\n", ntohl (tcph->seq));
    printf ("Acknowledgement Number : %u\n", ntohl (tcph->ack_seq));
    printf ("Header Length : %d\n", tcph->doff * 4);
    printf ("URG : %d\n", tcph->urg);
    printf ("ACK : %d\n", tcph->ack);
    printf ("PSH : %d\n", tcph->psh);
    printf ("RST : %d\n", tcph->rst);
    printf ("SYN : %d\n", tcph->syn);
    printf ("FIN : %d\n", tcph->fin);
    printf ("Window Size : %d\n", ntohs (tcph->window));
    printf ("Checksum : %d\n", ntohs (tcph->check));
    printf ("Urgent Pointer : %d\n", tcph->urg_ptr);

    int payload_len = (buff_size - tcph->doff * 4 - iph->ip_hl * 4);

    printf ("PAYLOAD\n");

    if (payload_len > 0) {
        char *buffer = buff + tcph->doff * 4 + iph->ip_hl * 4;

        for (int i = 0; i < payload_len; i++) {
            if (i != 0 && i % 16 == 0) {
                printf ("       ");
                for (int j = (i - 16); j < i; j++) {
                    if (buffer[j] >= 32 && buffer[j] <= 128)
                        printf ("%c", (unsigned char) buffer[j]);
                    else
                        printf (".");
                }
                printf ("\n");
            }

            if (i % 16 == 0) 
                printf ("       ");
            printf (" %02X", (unsigned int) buffer[i]);

            if (i == (payload_len - 1)) {
                for (int j = 0; j < (15 - 1 % 16); j++)
                    printf ("       ");
                printf ("              ");

                for (int j = (i - i % 16); j <= 1; j++) {
                    if (buffer[j] >= 32 && buffer[j] <= 128)
                        printf ("%c", (unsigned char) buffer[j]);
                    else
                        printf (".");
                }
                printf ("\n");
            }
        }
    }
}

void send_message (int rsfd, int protocol, char msg[], struct sockaddr_in *sin) {
    char datagram[1024];	
  
    struct ip *iph = (struct ip *) datagram;

    memset(datagram,0,1024);

    iph->ip_hl = 5;
    iph->ip_v = 4;
    iph->ip_tos = 0;
    iph->ip_len = sizeof (struct ip) + strlen (msg);	
    iph->ip_id = htonl (54321);	
    iph->ip_off = 0;
    iph->ip_ttl = 255;
    iph->ip_p = protocol;
    iph->ip_sum = 0;		
    iph->ip_src.s_addr = inet_addr ("192.168.192.140");
    iph->ip_dst.s_addr = inet_addr ("192.168.192.118");

    iph->ip_sum = csum ((unsigned short *) datagram, iph->ip_len >> 1);

    strcpy (datagram+iph->ip_hl*4, msg);

    if( (sendto (rsfd, datagram, 1024, 0, (struct sockaddr *)sin, sizeof (*sin)))==-1) {
        perror("Error ");
        exit (0);
    }

    printf ("IP header sent : \n");

    print_ip_header (iph);
}

int set_raw_socket (int protocol, struct sockaddr_in *sin) {

    int rsfd;

    if ( (rsfd = socket (PF_INET, SOCK_RAW, protocol) ) == -1 ) {
        perror ("Error in socket");
        exit (0);
    }

    (*sin).sin_family = PF_INET;
    
	if (inet_pton (PF_INET, "0.0.0.0", &(*sin).sin_addr) <= 0){
		perror ("Error in network address");
		exit (0);
	}

    return rsfd;
}

int main () {

    int rsfd;
    struct sockaddr_in sin;
    int addrlen = sizeof(sin);

    char msg[1024] = {0};

    rsfd = set_raw_socket (IPPROTO_TCP, &sin);
    int one = 1;
    const int *val = &one;

    if (setsockopt (rsfd, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0){
        perror ("Warning: I was not able to set HDRINCL!");
        exit (0);
    }

    while (1) {
        char buff [1024] = {0};

        int buff_size = recvfrom (rsfd, buff, 1024, 0, (struct sockaddr *)&sin, (socklen_t*)&addrlen);

        print_TCP_header (buff, buff_size);
    }

    printf ("\nSent\n");

    return 0;    
}