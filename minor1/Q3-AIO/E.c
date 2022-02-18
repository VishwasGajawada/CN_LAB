/* echo server */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char buf[80] = {0};
    char msg[100] = "Hi_from_echo\n";
    while(1) {
        // scanf("%s", buf);
        // sprintf(msg, "echoed_%s\n", buf);
        printf("%s", msg);
        fflush(stdout);
        sleep(2);

        memset(buf, 0, sizeof(buf));
    }
    return 0;
}