#include<stdio.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>           
#include <sys/stat.h>
#include<sys/poll.h>
#include<signal.h>
#include<sys/wait.h>


void usr2_handler(int sig){
  printf("Signal %d received from child\n",sig);
  fflush(stdout);
}

int main(){
    signal(SIGUSR2,usr2_handler);
    int c=fork();
    if(c>0){
        sleep(1);
        kill(c,SIGUSR1);
        // sleeping so that we will be able to see the printf in child
        sleep(1); 
        kill(c,SIGINT);
        sleep(1); 
        kill(c,SIGINT);
        sleep(1); 
        kill(c,SIGINT);
        wait(NULL);
    }else {
        char *args[] = {"./p2.exe",NULL};
        execvp(args[0],args);
    }
    return 0;
}