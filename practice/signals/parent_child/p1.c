#include<stdio.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>           
#include <sys/stat.h>
#include<sys/poll.h>
#include<signal.h>
#include<sys/wait.h>

void par_handler(int sig){
  printf("Signal %d received from child\n",sig);
}
void chi_handler(int sig){
  printf("Signal %d received from parant\n",sig);
}
int main(){
    signal(SIGUSR1,par_handler);
    signal(SIGUSR2,chi_handler);
    int c=fork();
    printf("%d\n",c);
    if(c>0){
      //printf("%d\n",c);
      kill(c,SIGUSR1);
      wait(NULL);
    }
    else {
      printf("%d\n",getppid());
      kill(getppid(),SIGUSR2);
    }
    return 0;
}