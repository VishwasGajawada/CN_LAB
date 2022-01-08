#include <stdio.h>
#include <unistd.h>

int main(){
    int sleeptime = 5;
    printf("7-1: p3 program started\n");
    sleep(5);
    printf("7-2: p3 program finished sleeping %d seconds\n",sleeptime);
    return 0;
}