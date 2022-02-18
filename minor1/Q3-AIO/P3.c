#include <stdio.h>
#include <unistd.h>

int main() {
    while(1) {
        printf("Hello_from_P3\n");
        fflush(stdout);
        sleep(4);
    }
    return 0;
}

