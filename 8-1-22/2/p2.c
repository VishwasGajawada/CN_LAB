#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(int argc, char **argv){

    int pipeOutCopy = dup(1);
    int pipeInCopy = dup(0);

    int stdOutCopy = atoi(argv[1]);
    int stdInCopy = atoi(argv[2]);

    // since p2 should be able to read from keyboard and output to monitor, dup2 back to original stdout and stdins
    dup2(stdOutCopy,1);
    dup2(stdInCopy, 0);
    
    size_t sz = 100;
    char *p1_msg = (char *)malloc(sz* sizeof(char));
    char *p2_msg = (char *)malloc(sz* sizeof(char));

    bool firstTime = true;
    while(true){


        // read message from p1
        read(pipeInCopy,p1_msg,sz);
        if(strcmp(p1_msg,"exit")==0){
            break;
        }

        if(firstTime) {
            printf("\nI am process P2\n\n");
            firstTime = false;
        }

        // this printf statement sends output to screen i.e the original stdout
        printf("Recieved in  P2 : \"%s\"\n\n",p1_msg);
        printf("Enter in P2 : \n");

        int x = getline(&p2_msg,&sz,stdin);
        p2_msg[x-1] = '\0';

        write(pipeOutCopy,p2_msg,strlen(p2_msg)+1);
        if(strcmp(p2_msg,"exit")==0){
            break;
        }

    }


    return 0;
}