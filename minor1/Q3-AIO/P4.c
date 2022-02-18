#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int getServerPid() {
    int fd = fileno(popen("pidof ./S.exe", "r"));
    perror("popen");
    char s[1000];
    read(fd, &s, 1000);
    return atoi(s);
}

int main() {
    sleep(1);
    int s_pid = getServerPid();
    printf("s_pid is %d\n", s_pid);
    int action;
    while(1) {
        printf("Enter '1' to signal S\n");
        scanf("%d", &action);
        kill(s_pid, SIGUSR1);
    }
    return 0;
}

// gcc S.c -o S.exe
// gcc P1.c -o P1.exe
// gcc P2.c -o P2.exe
// gcc P3.c -o P3.exe
// gcc P4.c -o P4.exe
// gcc C.c -o C.exe
// gcc E.c -o E.exe
