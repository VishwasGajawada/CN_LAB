#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
    // since p2 does not know about the pipes we are connecting here, we can directly dup2 its input and output fds. Since ever process has them.

    int fd1[2]; // from p1 to p2
    int fd2[2]; // from p2 to p1
    
    pipe(fd1);
    pipe(fd2);

    int c = fork();

    if(c>0){
        close(fd1[0]);
        close(fd2[1]);

        char p1_msg[] = "Hello from p1";
        printf("Sending from p1: %s\n",p1_msg);
        write(fd1[1],p1_msg,strlen(p1_msg)+1);
        close(fd1[1]);


        // now read message written from p2
        char p2_msg[70] = {0};
        read(fd2[0],p2_msg,71);
        printf("Received from p2: \"%s.\"\n",p2_msg);
        close(fd2[0]);
        
        wait(NULL);
    }else{
        close(fd1[1]);
        close(fd2[0]);

        // storing the original stdout and stdin copies so that P2 can take input from keyboard and output to terminal
        int stdOutCopy = dup(1);
        int stdInCopy = dup(0);

        char stdOutCopyString[5], stdInCopyString[5];
        sprintf(stdOutCopyString,"%d",stdOutCopy);
        sprintf(stdInCopyString,"%d",stdInCopy);

        dup2(fd1[0],0);
        dup2(fd2[1],1);

        char *args[] = {"./p2.exe", stdOutCopyString, stdInCopyString, NULL};
        execvp(args[0],args);
    }
    return 0;
}
