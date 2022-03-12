#include<bits/stdc++.h>
#include<ace/LSOCK_Acceptor.h>
#include<ace/LSOCK_Stream.h>
#include<ace/LSOCK_Connector.h>

using namespace std;
int main(){
    
    ACE_UNIX_Addr addr("test");
    ACE_LSOCK_Stream sfd;
    ACE_LSOCK_Connector connector;
    connector.connect(sfd, ACE_UNIX_Addr("test"));
    
    char buffer[50]="haofha"; int sz;
    int fd=open("hi.txt",O_RDWR);
    sfd.send_handle(fd);

    
        
    return 0;



}