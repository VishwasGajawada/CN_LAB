#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main() {
    if(mkfifo("/tmp/aio-fifo", 0666) == -1) {
        if(errno != EEXIST) {
            perror("mkfifo");
            return 1;
        }
    }
    int fd = open("/tmp/aio-fifo", O_WRONLY);
    
    char msg[100] = "Hello_from_P2\n";
    while(1) {
        write(fd, msg, strlen(msg));
        sleep(4);
    }
    return 0;
}