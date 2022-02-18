#include <stdio.h>
#include <unistd.h>

int main() {
    while(1) {
        printf("Hello_from_P1\n");
        fflush(stdout);
        sleep(4);
    }
}

