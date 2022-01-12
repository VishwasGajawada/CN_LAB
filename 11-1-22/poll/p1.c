#include <stdio.h>
#include <unistd.h>
/* for mkfifo */
#include <sys/types.h>
#include <sys/stat.h>
// #include <fcntl.h>           /* Definition of AT_* constants */
// #include <sys/stat.h>
/* for poll */
#include <sys/poll.h>

#include <fcntl.h>   
#include <errno.h>
#include <string.h>
#include <stdlib.h>

void printerror() {
  perror(strerror(errno));
}

int main() {
  char* FIFO2 = "/tmp/FIFO2";
  char* FIFO3 = "/tmp/FIFO3";
  char* FIFO4 = "/tmp/FIFO4";


  if (mkfifo(FIFO2, 0666) == -1) { printerror(); }
  if (mkfifo(FIFO3, 0666) == -1) { printerror(); }
  if (mkfifo(FIFO4, 0666) == -1) { printerror(); }

  int fds[3];
  fds[0] = open(FIFO2, O_RDONLY);
  fds[1] = open(FIFO3, O_RDONLY);
  fds[2] = open(FIFO4, O_RDONLY);

  int nfds = 3;
  // first tryinh one by one
  // char buf[30] = {0};
  // for(int i=0;i<nfds;i++){
  //   read(fds[i], buf, 30);
  //   printf("P%d %s\n", i+1, buf);
  // }

  // try polling
  struct pollfd pfd[nfds];
  for (int i = 0; i < nfds;i++) {
    pfd[i].fd = fds[i];
    pfd[i].events = POLLIN;
  }

  int nopenfds = 3;
  while (nopenfds > 0) {
    int ready = poll(pfd, nfds, -1);
    if (ready == -1)
      return 0;
    for (int i = 0;i < 3;i++) {
      char buff[100] = { 0 };
      if (pfd[i].revents != 0) {
        if (pfd[i].revents & POLLHUP) {
          close(pfd[i].fd);
          pfd[i].fd = -1;
          nopenfds--;
        }
        else if (pfd[i].revents & POLLIN) {
          int x = read(pfd[i].fd, buff, 100);

          if (x == 0) {
            close(pfd[i].fd);
            pfd[i].fd = -1;
            nopenfds--;
          }
          printf("P%d : %s\n", i + 2, buff);
        }
      }
    }
  }

  return 0;
}