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
#include<netinet/tcp.h>
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

void packets(u_char *args, const struct pcap_pkthdr *header, const u_char *Buffer){

    //printf("Entered here\n");

    struct ethhdr *ethheader;
    ethheader=(struct ethhdr *)Buffer;

    struct iphdr *ip_check=(struct iphdr *)(Buffer+sizeof(struct ether_header));
    struct sockaddr_in addr_check;
    addr_check.sin_addr.s_addr=ip_check->daddr;

    struct tcphdr *tcp;
    tcp=(struct tcphdr*)(Buffer+sizeof(struct iphdr)+sizeof(struct ethhdr));

    int Size=header->len;
    unsigned short iphdrlen;
    iphdrlen = ip_check->ihl*4;
    int header_size =  sizeof(struct ethhdr) + iphdrlen + tcp->doff*4;

    if((unsigned int)tcp->ack==1&&(unsigned int)tcp->psh==1&&(ntohs(tcp->source)==12345)){
        printf("This is a message from server to client1!\n");
        printf("The message is: ");
        
        PrintData(Buffer + header_size , Size - header_size );
    }
    else if((unsigned int)tcp->ack==1&&(unsigned int)tcp->psh==1&&(ntohs(tcp->dest)==12345)){
        printf("This is a message from c1 to server!\n");
        printf("The message is: ");
        PrintData(Buffer + header_size , Size - header_size );
    }
    else if((unsigned int)tcp->ack==1&&(unsigned int)tcp->psh==1&&(ntohs(tcp->source)==12346)){
        printf("This is a message from server to client2!\n");
        printf("The message is: ");
        PrintData(Buffer + header_size , Size - header_size );
    }
    else if((unsigned int)tcp->ack==1&&(unsigned int)tcp->psh==1&&(ntohs(tcp->dest)==12346)){
        printf("This is a message from c2 to server!\n");
        printf("The message is: ");
       PrintData(Buffer + header_size , Size - header_size );
    }
    else if((unsigned int)tcp->ack==1&&(unsigned int)tcp->psh==1&&(ntohs(tcp->source)==12347)){
        printf("This is a message from server to client3!\n");
        printf("The message is: ");
        PrintData(Buffer + header_size , Size - header_size );
    }
    else if((unsigned int)tcp->ack==1&&(unsigned int)tcp->psh==1&&(ntohs(tcp->dest)==12347)){
        printf("This is a message from c3 to server!\n");
        printf("The message is: ");
        PrintData(Buffer + header_size , Size - header_size );
    }
    
}



int main(){

    //handle for the device to be sniffed
    pcap_t *handle;

    char errbuf[100];
    handle = pcap_open_live("lo",65535,1,100,errbuf);  //opening loopback device

    pcap_loop(handle,0,packets,NULL);


    return 0;
}