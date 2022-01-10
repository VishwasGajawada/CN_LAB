#include<stdio.h>
#include<unistd.h>

int main(){
    FILE *f = popen("./p2.exe","r");
    int fd = fileno(f);

    char buf[1024];
    read(fd,buf,1024);

    printf("in p1 : %s\n",buf);
}

// gcc p2.c -o p2.exe && gcc p1.c -o p1.exe && ./p1.exe