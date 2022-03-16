#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ether.h>

int main(){
    int rsfd=socket (AF_INET, SOCK_RAW, 2);	
    int opt=1;
    printf("%d\n",htons(ETH_P_IP));
    //setsockopt(rsfd, IPPROTO_IP, SO_BROADCAST, &opt, sizeof(int));
    struct sockaddr_in client;
    client.sin_family=AF_INET;
    client.sin_addr.s_addr=INADDR_ANY;
    char buff[100]="hello";
    socklen_t clilen=sizeof(client);
    sendto(rsfd,buff,100,0,(struct sockaddr*)&client,(socklen_t)clilen);
    return 0;
}