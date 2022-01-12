#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int pressed = 0;
// program terminates on 6th ctrl+c
void sigint_handler(int signum) {
    pressed++;
    printf("***********\n");
    if(pressed == 5){
        signal(SIGINT, SIG_DFL);
    }
}

int main(){
    signal(SIGINT, sigint_handler);
    while(1){
        sleep(1);
    }
    return 0;
}