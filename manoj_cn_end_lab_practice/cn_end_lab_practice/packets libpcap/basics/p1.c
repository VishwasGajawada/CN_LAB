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
#include<netinet/tcp.h>
#include<netinet/udp.h>
#include<sys/ipc.h>
#include<sys/msg.h>
void PrintData (const u_char * data , int Size)
{
    int i , j;
    for(i=0 ; i < Size ; i++)
    {
        if( i!=0 && i%16==0)
        {
            for(j=i-16 ; j<i ; j++)
            {
                if(data[j]>=32 && data[j]<=128)
                    printf("%c",(unsigned char)data[j]);
                 
                else printf(".");
            }
        }     
        if( i==Size-1)
        {
            for(j=i-i%16 ; j<=i ; j++)
            {
                if(data[j]>=32 && data[j]<=128) 
                {
                  printf("%c",(unsigned char)data[j]);
                }
                else
                {
                  printf(".");
                }
            }
             
            printf("\n" );
        }
    }
}
void packets(u_char *args, const struct pcap_pkthdr *header, const u_char *buffer)
{
    struct ethhdr *ethheader;
    ethheader=(struct ethhdr *)buffer;
    struct iphdr *ip1=(struct iphdr *)(buffer+sizeof(struct ether_header));
    struct sockaddr_in addr2;
    addr2.sin_addr.s_addr=ip1->daddr;
    if(strcmp(inet_ntoa(addr2.sin_addr),"84.30.20.1"))
    {
        return;
    }
    printf("Ethernet Header\n");
    printf("   |-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n",ethheader->h_dest[0] , ethheader->h_dest[1] , ethheader->h_dest[2] , ethheader->h_dest[3] , ethheader->h_dest[4] , ethheader->h_dest[5] );
    printf("   |-Source Address      : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X \n", ethheader->h_source[0] , ethheader->h_source[1] , ethheader->h_source[2] , ethheader->h_source[3] , ethheader->h_source[4] , ethheader->h_source[5] );
    printf("   |-Protocol            : %u \n",(unsigned short)ethheader->h_proto);
    
    struct iphdr *ip=(struct iphdr *)(buffer+sizeof(struct ethhdr));
    printf("\nIP Header \n");
	printf("------------\n");
	printf("Check Sum : %d\n",ip->check);
    struct sockaddr_in addr,addr1;
    addr.sin_addr.s_addr=ip->daddr;
    addr1.sin_addr.s_addr=ip->saddr;
	printf("Destination Addr : %s\n",(inet_ntoa(addr.sin_addr)));
	printf("Fragment Offset : %d\n",ip->frag_off);
	printf("ID : %d\n",ip->id);
	printf("IP Header Length : %d\n",(int)ip->ihl);
	printf("IP Protocol : %d\n",ip->protocol);
	printf("Source Address : %s\n",(inet_ntoa(addr1.sin_addr)));
	printf("Type of Service : %d\n",ip->tos);
	printf("Total Length : %d\n",ip->tot_len);
	printf("Time to Live : %d\n",ip->ttl);
	printf("IP Version : %d\n",ip->version);
    printf("\n\n");

    if(ip->protocol==6)
    {
        struct tcphdr *tcp;
        tcp=(struct tcphdr*)(buffer+ip->ihl*4+sizeof(struct ethhdr));
        printf("\nTCP Header\n");
        printf("------------\n");
        printf("Source Port      : %u\n",ntohs(tcp->source));
        printf("Destination Port : %u\n",ntohs(tcp->dest));
        printf("Sequence Number    : %u\n",ntohl(tcp->seq));
        printf("Acknowledge Number : %u\n",ntohl(tcp->ack_seq));
        printf("Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcp->doff,(unsigned int)tcp->doff*4);
        printf("Urgent Flag          : %d\n",(unsigned int)tcp->urg);
        printf("Acknowledgement Flag : %d\n",(unsigned int)tcp->ack);
        printf("Push Flag            : %d\n",(unsigned int)tcp->psh);
        printf("Reset Flag           : %d\n",(unsigned int)tcp->rst);
        printf("Synchronise Flag     : %d\n",(unsigned int)tcp->syn);
        printf("Finish Flag          : %d\n",(unsigned int)tcp->fin);
        printf("Window         : %d\n",ntohs(tcp->window));
        printf("Checksum       : %d\n",ntohs(tcp->check));
        printf("Urgent Pointer : %d\n",tcp->urg_ptr);
        PrintData((buffer+sizeof(struct ethhdr)+sizeof(struct iphdr)+sizeof(struct tcphdr)),header->len-(sizeof(struct ethhdr)+sizeof(struct iphdr)+sizeof(struct tcphdr)));
        printf("\n\n");
    }
    else if(ip->protocol==17)
    {
        struct udphdr *udp;
        udp=(struct udphdr*)(buffer+sizeof(struct ethhdr)+sizeof(struct iphdr));
        printf("\nUDP Header\n");
        printf("------------\n");
        printf("Source Port:%d\n",ntohs(udp->source));
        printf("Destination Port:%d\n",ntohs(udp->dest));
        printf("UDP Length:%d\n",ntohs(udp->len));
        printf("UDP Checksum:%d\n",ntohs(udp->check));
        PrintData((buffer+sizeof(struct ethhdr)+sizeof(struct iphdr)+sizeof(struct udphdr)),header->len-(sizeof(struct ethhdr)+sizeof(struct iphdr)+sizeof(struct udphdr)));
        printf("\n\n");
    }
}
int main()
{
    pcap_if_t *alldevs,*device;
    pcap_t *handle;
    char errbuf[1000];
    pcap_findalldevs(&alldevs,errbuf);
    int i=1;
    char *dev[1000];
    for(device=alldevs;device!=NULL;device=device->next)
    {
        printf("%d.  %s\n",i,device->name);
        dev[i]=device->name;
        i++;
    }
    int d;
    printf("Enter the device no that you want to open : ");
    scanf("%d",&d);
    handle = pcap_open_live(dev[d],65536,1,10,errbuf);
    perror("open device ");
    while(1)
    pcap_loop(handle,1,packets,NULL);
    return 0;
}