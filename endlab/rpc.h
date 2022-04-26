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

#define client_path "client.c"
#define client_projid 20
#define client_stub_port 8080

#define server_path "server.c"
#define server_projid 30
#define server_stub_port 8081

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)
    
struct rpc_args {
    int arg1, arg2;
}; 

struct rpc_hdr {
    char function_name[10];
    /*
     opcode = 1 : request
     opcode = 2 : reply
    */
    int opcode;
    int return_val; /* return value filled by server */
    struct rpc_args arguments;
};

/* message struct for message queue */
struct mymsg {
    long type;
    struct rpc_hdr msg;
};




unsigned short in_cksum(unsigned short *addr, int len) {
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;
    while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
    if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return (answer);
}

