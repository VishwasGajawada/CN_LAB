#include<stdio.h>
#include<unistd.h>

int main(){
    FILE *f = popen("./p3.exe","w");
    int fd = fileno(f);
    int screen = dup(1);
    dup2(fd, 1);
    while(1){
        char buf[1024]={0};
        scanf("%s", buf);
        dprintf(screen, "in p2 : '%s'\n", buf);
        printf("%s ", buf);
        fflush(stdout);
    }
}
