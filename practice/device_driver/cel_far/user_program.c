#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(){
    int fd = open("/dev/cel_far", O_RDWR);
    printf("fd = %d\n", fd);
    printf("Welcome to Temperature Converter (Celsius <-> Farenheit)\n\n");
    while(1){
        printf("1. Celsis to Farenheit;\n2. Farenheit to Celsius;\n");
        printf("Enter your choice, and value of temperature: ");
        int choice, temp;
        scanf("%d", &choice);
        scanf("%d", &temp); 

        char buf[10];
        sprintf(buf, "%d_%d", choice, temp);
        write(fd, buf, 10);

        memset(buf, 0, 10);
        read(fd, buf, 10);
        int converted_temp = atoi(buf);

        switch(choice){
            case 1:
                printf("%d in Farenheit is %d\n\n", temp, converted_temp);
                break;
            case 2:
                printf("%d in Celsius is %d\n\n", temp, converted_temp);
                break;
        }

    }
    return 0;
}