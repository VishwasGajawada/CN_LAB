#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main() {
    // since p2 does not know about the pipes we are connecting here, we can directly dup2 its input and output fds. Since ever process has them.

    int fd1[2]; // from p1 to p2
    int fd2[2]; // from p2 to p1

    pipe(fd1);
    pipe(fd2);

    printf("Enter 'exit' to exit\n\n");

    int c = fork();

    if (c > 0) {
        close(fd1[0]);
        close(fd2[1]);

        size_t sz = 100;
        char* p1_msg = (char*)malloc(sz * sizeof(char));
        char* p2_msg = (char*)malloc(sz * sizeof(char));

        while (true) {

            printf("Enter in P1 : \n");
            int x = getline(&p1_msg, &sz, stdin);
            p1_msg[x - 1] = '\0';
            write(fd1[1], p1_msg, strlen(p1_msg) + 1);

            if (strcmp(p1_msg, "exit") == 0) {
                break;
            }

            // now read message written from p2
            read(fd2[0], p2_msg, sz);
            if (strcmp(p2_msg, "exit") == 0) {
                break;
            }
            printf("Received in P1: \"%s\"\n\n", p2_msg);
        }
        close(fd1[1]);
        close(fd2[0]);

        wait(NULL);
    }
    else {
        close(fd1[1]);
        close(fd2[0]);

        /*******************/
        // storing the original stdout and stdin copies of P2 so that P2 can take input from keyboard and output to terminal
        int stdOutCopy = dup(1);
        int stdInCopy = dup(0);

        char stdOutCopyString[5], stdInCopyString[5];
        sprintf(stdOutCopyString, "%d", stdOutCopy);
        sprintf(stdInCopyString, "%d", stdInCopy);
        /*******************/

        dup2(fd1[0], 0);
        dup2(fd2[1], 1);

        char* args[] = { "./p2.exe", stdOutCopyString, stdInCopyString, NULL };
        execvp(args[0], args);
    }
    return 0;
}
