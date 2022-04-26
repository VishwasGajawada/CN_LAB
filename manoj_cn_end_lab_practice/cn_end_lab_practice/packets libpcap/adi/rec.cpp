#include "cn.h"
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
#include<netinet/tcp.h>
#include<linux/if_packet.h>
#include<net/ethernet.h>
// #include "../../cn.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>       // IPPROTO_RAW, INET_ADDRSTRLEN
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <net/if.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/in.h>       // IPPROTO_RAW, INET_ADDRSTRLEN
#include <netinet/ip.h>       // IP_MAXPACKET (which is 65535)
#include <net/if.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h> 

using namespace std;

char* alloted;
FILE *logfile;

uint8_t src[6], dst[6], des[6];
void packets(u_char* args, const struct pcap_pkthdr* header, const u_char* buffer) {
    struct ethhdr* ethheader;
    ethheader = (struct ethhdr*)buffer;

    struct iphdr* ip = (struct iphdr*)(buffer + sizeof(struct ethhdr));
    struct sockaddr_in addr, addr1;
    des[0] = ethheader->h_source[0];
    des[1] = ethheader->h_source[1];
    des[2] = ethheader->h_source[2];
    des[3] = ethheader->h_source[3];
    des[4] = ethheader->h_source[4];
    des[5] = ethheader->h_source[5];


    if (des[0] == src[0] && des[1] == src[1] && des[2] == src[2] && des[3] == src[3] && des[4] == src[4] && des[5] == src[5]) {
        addr.sin_addr.s_addr = ip->daddr;
        addr1.sin_addr.s_addr = ip->saddr;
        if (ip->check == 0)
            return;
        printf("Ethernet Header\n");
        printf(" |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", ethheader->h_dest[0], ethheader->h_dest[1], ethheader->h_dest[2], ethheader->h_dest[3], ethheader->h_dest[4], ethheader->h_dest[5]);
        printf(" |-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", ethheader->h_source[0], ethheader->h_source[1], ethheader->h_source[2], ethheader->h_source[3], ethheader->h_source[4], ethheader->h_source[5]);
        // printf(" |-Protocol : %u \n",(unsigned short)ethheader->h_proto);

        printf("\nIP Header \n");
        printf("------------\n");
        printf("Check Sum : %d\n", ip->check);
        printf("Destination Addr : %s\n", (inet_ntoa(addr.sin_addr)));
        printf("Fragment Offset : %d\n", ip->frag_off);
        printf("ID : %d\n", ip->id);
        printf("IP Header Length : %d\n", (int)ip->ihl);
        printf("IP Protocol : %d\n", ip->protocol);
        printf("Source Address : %s\n", (inet_ntoa(addr1.sin_addr)));
        printf("Type of Service : %d\n", ip->tos);
        printf("Total Length : %d\n", ip->tot_len);
        printf("Time to Live : %d\n", ip->ttl);
        printf("IP Version : %d\n", ip->version);
        // PrintData((buffer+sizeof(struct ethhdr)+sizeof(struct iphdr)),header->len-(sizeof(struct ethhdr)+sizeof(struct iphdr)));
        printf("\n\n");
        alloted = (inet_ntoa(addr.sin_addr));
        cout << "New Ip alloted :" << alloted << "\n";
    }
}

void send() {
    int bytes;
    int sfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    char interface[40];
    strcpy(interface, "lo");
    if (sfd == -1) {
        perror("socket");
    }
    char* buf = (char*)malloc(1500);
    src[0] = 0x8C;
    src[1] = 0x16;
    src[2] = 0x45;
    src[3] = 0xCE;
    src[4] = 0x8B;
    src[5] = 0xE4;
    dst[0] = 0x00;
    dst[1] = 0x25;
    dst[2] = 0x83;
    dst[3] = 0x70;
    dst[4] = 0x10;
    dst[5] = 0x00;

    struct sockaddr_ll device;
    memset(&device, 0, sizeof(device));
    device.sll_ifindex = if_nametoindex(interface);
    device.sll_family = AF_PACKET;
    memcpy(device.sll_addr, dst, 6 * sizeof(uint8_t));
    device.sll_halen = 6;
    memcpy(buf, dst, 6 * (sizeof(uint8_t)));
    memcpy(buf + 6 * (sizeof(uint8_t)), src, 6 * (sizeof(uint8_t)));
    printf("%s", buf);
    // while(5){     
    sendto(sfd, buf, 42, 0, (struct sockaddr*)&device, sizeof(device));
}

void* sendReplies(void* args) {
    while (1) {
        cout << "Sending..\n";
        send();
        sleep(1);
    }
}
void print_packet(char *buffer){
        
    struct iphdr *ip=(struct iphdr *)(buffer+sizeof(struct ethhdr));
    fprintf(logfile, "\nIP Header \n");
	fprintf(logfile, "------------\n");
	fprintf(logfile, "Check Sum : %d\n",ip->check);
    struct sockaddr_in addr,addr1;
    addr.sin_addr.s_addr=ip->daddr;
    addr1.sin_addr.s_addr=ip->saddr;
	fprintf(logfile, "Destination Addr : %s\n",(inet_ntoa(addr.sin_addr)));
	fprintf(logfile, "Fragment Offset : %d\n",ip->frag_off);
	fprintf(logfile, "ID : %d\n",ip->id);
	fprintf(logfile, "IP Header Length : %d\n",(int)ip->ihl);
	fprintf(logfile, "IP Protocol : %d\n",ip->protocol);
	fprintf(logfile, "Source Address : %s\n",(inet_ntoa(addr1.sin_addr)));
	fprintf(logfile, "Type of Service : %d\n",ip->tos);
	fprintf(logfile, "Total Length : %d\n",ip->tot_len);
	fprintf(logfile, "Time to Live : %d\n",ip->ttl);
	fprintf(logfile, "IP Version : %d\n",ip->version);
    fprintf(logfile, "\n\n");

    if(ip->protocol==6)
    {
        struct tcphdr *tcp;
        tcp=(struct tcphdr*)(buffer+ip->ihl*4+sizeof(struct ethhdr));
        fprintf(logfile, "\nTCP Header\n");
        fprintf(logfile, "------------\n");
        fprintf(logfile, "Source Port      : %u\n",ntohs(tcp->source));
        fprintf(logfile, "Destination Port : %u\n",ntohs(tcp->dest));
        fprintf(logfile, "Sequence Number    : %u\n",ntohl(tcp->seq));
        fprintf(logfile, "Acknowledge Number : %u\n",ntohl(tcp->ack_seq));
        fprintf(logfile, "Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcp->doff,(unsigned int)tcp->doff*4);
        fprintf(logfile, "Urgent Flag          : %d\n",(unsigned int)tcp->urg);
        fprintf(logfile, "Acknowledgement Flag : %d\n",(unsigned int)tcp->ack);
        fprintf(logfile, "Push Flag            : %d\n",(unsigned int)tcp->psh);
        fprintf(logfile, "Reset Flag           : %d\n",(unsigned int)tcp->rst);
        fprintf(logfile, "Synchronise Flag     : %d\n",(unsigned int)tcp->syn);
        fprintf(logfile, "Finish Flag          : %d\n",(unsigned int)tcp->fin);
        fprintf(logfile, "Window         : %d\n",ntohs(tcp->window));
        fprintf(logfile, "Checksum       : %d\n",ntohs(tcp->check));
        fprintf(logfile, "Urgent Pointer : %d\n",tcp->urg_ptr);
        fprintf(logfile, "\n\n");
    }
    else if(ip->protocol==17)
    {
        struct udphdr *udp;
        udp=(struct udphdr*)(buffer+sizeof(struct ethhdr)+sizeof(struct iphdr));
        fprintf(logfile, "\nUDP Header\n");
        fprintf(logfile, "------------\n");
        fprintf(logfile, "Source Port:%d\n",ntohs(udp->source));
        fprintf(logfile, "Destination Port:%d\n",ntohs(udp->dest));
        fprintf(logfile, "UDP Length:%d\n",ntohs(udp->len));
        fprintf(logfile, "UDP Checksum:%d\n",ntohs(udp->check));
        fprintf(logfile, "\n\n");
    }
}
void recPacket() {
    int bytes;
    int sfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sfd == -1) {
        perror("socket");
    }
    // bind
    struct sockaddr_in addr;
    addr.sin_port = htons(2020);
    // bind(sfd, )

    char* buf = (char*)malloc(65536);
    recvfrom(sfd, buf, 65536, 0, NULL, NULL);
    print_packet(buf);
}

int main() {

    logfile=fopen("log.txt","w");
	if(logfile==NULL) 
	{
		printf("Unable to create file.");
	}
    while(1) {
        recPacket();
    }
}