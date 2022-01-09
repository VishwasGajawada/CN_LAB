#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char** argv) {

    int pipeOutCopy = dup(1);
    int pipeInCopy = dup(0);

    int stdOutCopy = atoi(argv[1]);
    int stdInCopy = atoi(argv[2]);

    // since p3 should be able to read from keyboard and output to monitor, dup2 back to original stdout and stdin
    dup2(stdOutCopy, 1);
    dup2(stdInCopy, 0);

    size_t sz = 100;
    char* p2_msg = (char*)malloc(sz * sizeof(char));
    char* p3_msg = (char*)malloc(sz * sizeof(char));

    bool firstTime = true;
    while (true) {
        // read message from p1
        read(pipeInCopy, p2_msg, sz);
        if (strcmp(p2_msg, "exit") == 0) {
            break;
        }

        if (firstTime) {
            printf("\nI am process P3\n\n");
            firstTime = false;
        }

        // this printf statement sends output to screen i.e the original stdout
        printf("Recieved in  P3 : \"%s\"\n\n", p2_msg);
        printf("Enter in P3 : \n");

        int x = getline(&p3_msg, &sz, stdin);
        p3_msg[x - 1] = '\0';

        write(pipeOutCopy, p3_msg, strlen(p3_msg) + 1);
        if (strcmp(p3_msg, "exit") == 0) {
            break;
        }

    }

    return 0;
}