/*
gcc p2.c -o p2  && gcc main.c  && ./a.out
*/
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>           
#include <sys/poll.h>
#include <stdlib.h>
#include <string.h>

int main(){
  int fds[3];
  fds[0] = 0;
  FILE* f=popen("./p2.exe","r");
  fds[1] = fileno(f);
  fds[2] = open("./temp.txt",O_RDONLY|O_CREAT,0666);

  printf("%d %d %d\n",fds[0],fds[1],fds[2]);
  
  struct pollfd pfd[3];
  for(int i=0;i<3;i++){
    pfd[i].fd = fds[i];
    pfd[i].events=POLLIN;
  }
  int nfds = 3;
  int nopenfds = 3;
  while(nopenfds>0){
    int ready = poll(pfd, nfds, -1);
    if (ready == -1)
        return 0;
    for(int i=0;i<3;i++){
      // https://www.geeksforgeeks.org/difference-between-malloc-and-calloc-with-examples/
      char *buff =(char*)malloc(100*sizeof(char));
      
      if(pfd[i].revents != 0){
        printf("fd[%d]=%d: events: %s%s%s\n", i, pfd[i].fd,
                                (pfd[i].revents & POLLIN)  ? "POLLIN "  : "",
                                (pfd[i].revents & POLLHUP) ? "POLLHUP " : "",
                                (pfd[i].revents & POLLERR) ? "POLLERR " : "");
        fflush(stdout);
        if(pfd[i].revents & POLLHUP){
          close(pfd[i].fd);
          printf("Closed1\n");
          pfd[i].fd = -1;
          nopenfds--;
        }
        else if(pfd[i].revents & POLLIN){
          int x = read(pfd[i].fd,buff,100);

          if(x == 0){
            close(pfd[i].fd); 
            printf("Closed2\n");
            pfd[i].fd = -1;
            nopenfds--;
          }
          printf("%s\n",buff);
          fflush(stdout);
        }
      }
      else{
        printf("fd[%d]=%d: revent is zero\n",i,pfd[i].fd); 
      }
      free(buff);
    }
  }
  return 0;
}
// https://man7.org/linux/man-pages/man2/poll.2.html