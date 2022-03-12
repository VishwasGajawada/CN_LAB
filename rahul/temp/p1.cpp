#include<bits/stdc++.h>
#include<ace/LSOCK_Acceptor.h>
#include<ace/LSOCK_Stream.h>
#include<ace/LSOCK_Connector.h>
using namespace std;
int main(){
    
    ACE_UNIX_Addr addr("test");
    ACE_LSOCK_Stream sfd;
    ACE_LSOCK_Acceptor acceptor(addr);
    acceptor.accept(sfd);
    
    char buffer[50]="beforerecv"; int sz;
    //while(1){
        // sfd.recv_n(buffer, 50);
        // cout<<buffer<<endl;
    //}
    ACE_HANDLE handle ;
    char buff[100];
    ssize_t *len;
    int fd=sfd.recv_handle(handle);
    read (handle, buff, sizeof buff);
    printf("%s\n",buff);
    return 0;



}