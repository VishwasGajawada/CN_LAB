#include "rpc.h"

/* this is client_stub */
/* recieve rpc hdr request packet from client, and send it via pcap_inject to server_stub */
/* recieve rpc hdr reply packet from server_stub using pcap_next, and send it to client */

int msqid;
void print_rpc_header_request(struct rpc_hdr rpc) {
    printf("\nRPC Header\n");
    printf("------------\n");
    printf("function name : %s\n",rpc.function_name);
    printf("opcode : %d\n",(rpc.opcode));
    printf("arg1 = %d\n",(rpc.arguments.arg1));
    printf("arg2 = %d\n",(rpc.arguments.arg2));
    printf("\n\n");
    fflush(stdout);
}
void print_rpc_header_reply(struct rpc_hdr rpc) {
    printf("\nRPC Header\n");
    printf("------------\n");
    printf("opcode : %d\n",(rpc.opcode));
    printf("return val : %d\n",(rpc.return_val));
    printf("\n\n");
    fflush(stdout);
}
void extract_rpc(const char *buffer, struct rpc_hdr *rpc){
    /* print rpc_hdr */
    struct rpc_hdr *temp=(struct rpc_hdr*)(buffer+sizeof(struct ethhdr)+sizeof(struct iphdr)+sizeof(struct tcphdr));
    strcpy(rpc->function_name,temp->function_name);
    rpc->opcode = ntohs(temp->opcode);
    rpc->arguments.arg1 = ntohs(temp->arguments.arg1);
    rpc->arguments.arg2 = ntohs(temp->arguments.arg2);
    rpc->return_val = ntohs(temp->return_val);
}
void send_pcap_packet(pcap_t *handle, struct rpc_hdr msg ) {
    char buff[65535];

    // ethernet header code
    struct ether_header *eth=(struct ether_header *)buff;
    uint8_t src[6],dst[6];
    src[0] = 0x01;
    src[1] = 0x02;
    src[2] = 0x03;
    src[3] = 0x04;
    src[4] = 0x05;
    src[5] = 0x06;
    dst[0] = 0x07;
    dst[1] = 0x08;
    dst[2] = 0x09;
    dst[3] = 0x10;
    dst[4] = 0x11;
    dst[5] = 0x12;
    memcpy(buff,dst,6*(sizeof (uint8_t)));
    memcpy(buff+6*(sizeof (uint8_t)),src,6*(sizeof (uint8_t)));
    buff[12] = ETH_P_ARP / 256;
    buff[13] = ETH_P_ARP % 256;
    

    // ip header code
    struct iphdr *iph=(struct iphdr *)(buff+sizeof(struct ether_header));
    char sip[]="127.0.0.1";
    char dip[]="127.0.0.2";
    iph->ihl=5;
    iph->version=4;
    iph->tot_len=sizeof(struct iphdr)+sizeof(struct tcphdr)+5;
    iph->protocol=6;
    iph->ttl=64;
    iph->saddr=inet_addr(sip);
    iph->daddr=inet_addr(dip);
    iph->check=in_cksum((unsigned short *)iph,sizeof(struct iphdr));


    // tcp header code
    struct tcphdr *tcp;
    tcp = (struct tcphdr*)(buff+sizeof(struct ether_header)+sizeof(struct iphdr));

    tcp->dest = htons(server_stub_port);//destination port;
    tcp->source = htons(client_stub_port);//source port;
    tcp->doff = 8;// length of tcp header
    tcp->seq = htonl(156851345);// sequence number;
    tcp->ack_seq = htonl(654864534);// acknowledgement number
    tcp->fin = 0;
    tcp->psh = 1;
    tcp->rst = 0;
    tcp->ack = 1;
    tcp->syn = 0;// SYN bit
    tcp->urg = 0;// urgent bit
    tcp->window = htons(1000);// window size

    // rpc header code
    struct rpc_hdr *rpc;
    rpc = (struct rpc_hdr*)(buff+sizeof(struct ether_header)+sizeof(struct iphdr)+sizeof(struct tcphdr));

    strcpy(rpc->function_name, msg.function_name);
    rpc->opcode = msg.opcode;
    rpc->arguments.arg1 = msg.arguments.arg1;
    rpc->arguments.arg2 = msg.arguments.arg2;

    /* print rpc hdr */
    printf("\nRPC Header\n");
    printf("------------\n");
    printf("function name : %s\n",rpc->function_name);
    printf("opcode : %d\n",ntohs(rpc->opcode));
    printf("arg1 = %d\n",ntohs(rpc->arguments.arg1));
    printf("arg2 = %d\n",ntohs(rpc->arguments.arg2));


    // int n = sizeof(struct ether_header)+(iph->ihl*4)+(tcp->doff*4)+(sizeof(struct rpc_hdr));
    int n = sizeof(struct ethhdr)+sizeof(struct iphdr)+sizeof(struct tcphdr) + sizeof(struct rpc_hdr);
    if(pcap_inject(handle, buff, n) == -1) {
        handle_error("pcap_inject");
    }

}

int main() {
    key_t key = ftok(client_path, client_projid);
    if(key == -1) handle_error("ftok() ");

    msqid = msgget(key, 0666 | IPC_CREAT);
    if(msqid == -1) handle_error("msgget");

    pcap_t *handle = NULL;
    char errbuf[PCAP_ERRBUF_SIZE];
    handle = pcap_open_live("lo", 65535, 1, 20, errbuf);

    if(!handle) handle_error("pcap_open_live() ");

    char filter_exp[] = "ether src 07:08:09:10:11:12"; // filter expression
    // char filter_exp[] = "dst host 127.0.0.2"; // filter expression
    bpf_u_int32 subnet_mask, ip;
    /* End the loop after this many packets are captured */
    struct bpf_program filter;
    pcap_lookupnet("lo", &ip, &subnet_mask, errbuf);
    pcap_compile(handle, &filter, filter_exp, 0, ip);
    pcap_setfilter(handle, &filter);


    pcap_set_immediate_mode(handle, 1);

    while(1) {
        struct mymsg msg;
        msg.type = 1;

        if (msgrcv(msqid, &msg, sizeof(struct rpc_hdr), 1, 0) == -1) {
            perror("msgrcv");
            exit(1);
        }
        printf("recieved rpc hdr request packet from client\n");

        send_pcap_packet(handle, msg.msg);
        printf("sent rpc hdr request packet to server_stub\n");

        struct pcap_pkthdr hdr;     /* pcap.h */

        const u_char *packet = pcap_next(handle, &hdr);
        if(packet == NULL) continue;

        printf("recieved reply packet from server_stub\n");
        /* extract the rpc header from the packet */
        struct rpc_hdr rpc;
        extract_rpc(packet, &rpc); 
        print_rpc_header_reply(rpc);
        printf("Result = %d\n", (rpc.return_val));
        
        /* send this packet back to client */
        msg.type = 4;
        msg.msg = rpc;
        if (msgsnd(msqid, &msg, sizeof(struct rpc_hdr), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
        printf("sent reply packet to client\n");
    }    
    return 0;
}