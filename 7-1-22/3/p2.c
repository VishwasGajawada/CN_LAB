#include <stdio.h>
#include <unistd.h>

int main(){
    int sleeptime = 10;
    printf("5-1: p2 program started\n");
    sleep(sleeptime);
    printf("5-2: p2 program finished sleeping %d seconds\n",sleeptime);
    return 0;
}