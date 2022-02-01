#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(){
    int fd = open("/dev/read_write", O_RDWR);
    while(1){
        printf("1. write to kernel;\n2. read from kernel;\n3. exit\n");
        int choice;
        scanf("%d", &choice);

        char buf[1024];
        switch(choice) {
            case 1:
                printf("input: ");
                scanf("%s", buf);
                write(fd, buf, sizeof(buf));
                break;
            case 2:
                read(fd, buf, sizeof(buf));
                printf("output: %s\n", buf);
                break;
            case 3:
                close(fd);
                exit(0);
                break;
            default:
                printf("Invalid\n");
                break;
        }
    }
    return 0;
}