#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <string.h>
#include<sys/socket.h>
#include<arpa/inet.h> // for inet_ntoa()

FILE *logfile; //to store the output
void printIpheader(unsigned char *Buffer, int size);
void printTcpheader(unsigned char* Buffer, int Size);
void printData(unsigned char* data, int Size);

void printIpHeader(unsigned char* Buffer, int Size) {
    struct sockaddr_in source,dest;
	unsigned short iphdrlen;
		
	struct iphdr *iph = (struct iphdr *)Buffer;
	iphdrlen =iph->ihl*4;
	
	memset(&source, 0, sizeof(source));
	source.sin_addr.s_addr = iph->saddr;
	
	memset(&dest, 0, sizeof(dest));
	dest.sin_addr.s_addr = iph->daddr;
	
	fprintf(logfile,"\n");
	fprintf(logfile,"IP Header\n");
	fprintf(logfile,"   |-IP Version        : %d\n",(unsigned int)iph->version);
	fprintf(logfile,"   |-IP Header Length  : %d DWORDS or %d Bytes\n",(unsigned int)iph->ihl,((unsigned int)(iph->ihl))*4);
	fprintf(logfile,"   |-Type Of Service   : %d\n",(unsigned int)iph->tos);
	fprintf(logfile,"   |-IP Total Length   : %d  Bytes(Size of Packet)\n",ntohs(iph->tot_len));
	fprintf(logfile,"   |-Identification    : %d\n",ntohs(iph->id));
	//fprintf(logfile,"   |-Reserved ZERO Field   : %d\n",(unsigned int)iphdr->ip_reserved_zero);
	//fprintf(logfile,"   |-Dont Fragment Field   : %d\n",(unsigned int)iphdr->ip_dont_fragment);
	//fprintf(logfile,"   |-More Fragment Field   : %d\n",(unsigned int)iphdr->ip_more_fragment);
	fprintf(logfile,"   |-TTL      : %d\n",(unsigned int)iph->ttl);
	fprintf(logfile,"   |-Protocol : %d\n",(unsigned int)iph->protocol);
	fprintf(logfile,"   |-Checksum : %d\n",ntohs(iph->check));
	fprintf(logfile,"   |-Source IP        : %s\n",inet_ntoa(source.sin_addr));
	fprintf(logfile,"   |-Destination IP   : %s\n",inet_ntoa(dest.sin_addr));
}

void printTcpPacket(unsigned char * Buffer, int Size) {
    unsigned short iphdrlen;
     
    struct iphdr *iph = (struct iphdr *)(Buffer);
    iphdrlen = iph->ihl*4;
     
    struct tcphdr *tcph=(struct tcphdr*)(Buffer + sizeof(struct iphdr));
     
    fprintf(logfile , "\n\n***********************TCP Packet*************************\n");  
         
    printIpHeader(Buffer,Size);
         
    fprintf(logfile , "\n");
    fprintf(logfile , "TCP Header\n");
    fprintf(logfile , "   |-Source Port      : %u\n",ntohs(tcph->source));
    fprintf(logfile , "   |-Destination Port : %u\n",ntohs(tcph->dest));
    fprintf(logfile , "   |-Sequence Number    : %u\n",ntohl(tcph->seq));
    fprintf(logfile , "   |-Acknowledge Number : %u\n",ntohl(tcph->ack_seq));
    fprintf(logfile , "   |-Header Length      : %d DWORDS or %d BYTES\n" ,(unsigned int)tcph->doff,(unsigned int)tcph->doff*4);
    fprintf(logfile , "   |-Urgent Flag          : %d\n",(unsigned int)tcph->urg);
    fprintf(logfile , "   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack);
    fprintf(logfile , "   |-Push Flag            : %d\n",(unsigned int)tcph->psh);
    fprintf(logfile , "   |-Reset Flag           : %d\n",(unsigned int)tcph->rst);
    fprintf(logfile , "   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn);
    fprintf(logfile , "   |-Finish Flag          : %d\n",(unsigned int)tcph->fin);
    fprintf(logfile , "   |-Window         : %d\n",ntohs(tcph->window));
    fprintf(logfile , "   |-Checksum       : %d\n",ntohs(tcph->check));
    fprintf(logfile , "   |-Urgent Pointer : %d\n",tcph->urg_ptr);
    fprintf(logfile , "\n");
    
    
    // fprintf(logfile, "Payload : %s\n", (Buffer + iphdrlen + tcph->doff*4));
    // fprintf(logfile, "Payload : %s\n", (Buffer + iphdrlen));
    fprintf(logfile, "Payload : %s\n", (Buffer + iphdrlen + sizeof(struct tcphdr)));
    
    
    // fprintf(logfile , "                        DATA Dump                         ");
    // fprintf(logfile , "\n");

    // fprintf(logfile , "IP Header\n");
    // printData(Buffer,iphdrlen);
         
    // fprintf(logfile , "TCP Header\n");
    // printData(Buffer+iphdrlen,tcph->doff*4);
         
    // fprintf(logfile , "Data Payload\n");    
    // printData(Buffer + iphdrlen + tcph->doff*4 , (Size - tcph->doff*4-iph->ihl*4) );
                         
    fprintf(logfile , "\n###########################################################");
}
 
void printData (unsigned char* data , int Size) {
	int i, j;
	for(i=0 ; i < Size ; i++)
	{
		if( i!=0 && i%16==0)   //if one line of hex printing is complete...
		{
			fprintf(logfile,"         ");
			for(j=i-16 ; j<i ; j++)
			{
				if(data[j]>=32 && data[j]<=128)
					fprintf(logfile,"%c",(unsigned char)data[j]); //if its a number or alphabet
				
				else fprintf(logfile,"."); //otherwise print a dot
			}
			fprintf(logfile,"\n");
		} 
		
		if(i%16==0) fprintf(logfile,"   ");
			fprintf(logfile," %02X",(unsigned int)data[i]);
				
		if( i==Size-1)  //print the last spaces
		{
			for(j=0;j<15-i%16;j++) fprintf(logfile,"   "); //extra spaces
			
			fprintf(logfile,"         ");
			
			for(j=i-i%16 ; j<=i ; j++)
			{
				if(data[j]>=32 && data[j]<=128) fprintf(logfile,"%c",(unsigned char)data[j]);
				else fprintf(logfile,".");
			}
			fprintf(logfile,"\n");
		}
	}
}

int main() {
    int proto = IPPROTO_TCP;
    // int proto = 2;

    int rsfd = socket(AF_INET, SOCK_RAW, proto);
    if(rsfd < 0) perror("socket");
    
    logfile=fopen("log.txt","w");
    if(logfile==NULL) {
        printf("Unable to create file.");
    }else {
        printf("Opened log file\n");
    }

    char buf[1024] = {0};
    int size = recvfrom(rsfd, buf, sizeof(buf), 0, NULL, NULL);
    if(size < 0) perror("recvfrom");

    // struct iphdr *iph = (struct iphdr *)buf;
    // printf("%s\n", buf+iph->ihl*4);
    printTcpPacket(buf, size);
    fclose(logfile);
    return 0;
}