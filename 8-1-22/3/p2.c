#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char** argv) {

    int pipeOutCopy = dup(1); // p2 to p1
    int pipeInCopy = dup(0);  // p1 to p2

    int stdOutCopy = atoi(argv[1]); // of p2
    int stdInCopy = atoi(argv[2]); // of p2

    // since p2 should be able to read from keyboard and output to monitor, dup2 back to original stdout and stdin
    dup2(stdOutCopy, 1);
    dup2(stdInCopy, 0);

    size_t sz = 100;
    char* p1_msg = (char*)malloc(sz * sizeof(char));
    char* p3_msg = (char*)malloc(sz * sizeof(char));

    int fd1[2]; // from p2 to p3
    int fd2[2]; // from p3 to p2

    pipe(fd1);
    pipe(fd2);

    int c = fork();
    if (c > 0) {
        bool firstTime = true;
        while (true) {

            // read from p1 and forward to p3
            read(pipeInCopy, p1_msg, sz);
            if (firstTime) {
                printf("\nI am process P2\n\n");
                firstTime = false;
            }
            write(fd1[1], p1_msg, strlen(p1_msg) + 1);
            if (strcmp(p1_msg, "exit") == 0) {
                break;
            }


            // read from p3 and forward to p1
            read(fd2[0], p3_msg, sz);
            write(pipeOutCopy, p3_msg, strlen(p3_msg) + 1);
            if (strcmp(p3_msg, "exit") == 0) {
                break;
            }

        }
    }
    else {
        close(fd1[1]);
        close(fd2[0]);

        /*******************/
        // storing the original stdout and stdin copies of P3 so that P3 can take input from keyboard and output to terminal
        int stdOutCopy = dup(1); // of p3
        int stdInCopy = dup(0); // of p3

        char stdOutCopyString[5], stdInCopyString[5];
        sprintf(stdOutCopyString, "%d", stdOutCopy);
        sprintf(stdInCopyString, "%d", stdInCopy);
        /*******************/

        dup2(fd1[0], 0);
        dup2(fd2[1], 1);

        char* args[] = { "./p3.exe", stdOutCopyString, stdInCopyString, NULL };
        execvp(args[0], args);
    }

    return 0;
}