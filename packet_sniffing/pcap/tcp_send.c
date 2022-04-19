#include <stdio.h>
#include<sys/socket.h>

int main() {
    int sfd = socket(AF_INET, SOCK_RAW, 0);
    char msg[100] = "hello world";

    
    return 0;
}