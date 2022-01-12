#include<stdio.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>           
#include <sys/stat.h>
#include<sys/poll.h>
#include<signal.h>
#include<sys/wait.h>

void usr1_handler(int sig){
  printf("Signal %d received from parent\n",sig);
  fflush(stdout);

}
int cnt = 0;
void int_handler(int sig){
  cnt++;
  if(cnt == 2)
    signal(SIGINT,SIG_DFL);
}
int main(){
    
    signal(SIGUSR1,usr1_handler);
    signal(SIGINT,int_handler);
    kill(getppid(),SIGUSR2);
    while(1){}
    // printf("parents pid : %d\n",getppid());
}