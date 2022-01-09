#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
    int fd1[2]; //from P to P'(child)
    int fd2[2]; //from P' to P

    pipe(fd1);
    pipe(fd2);

    printf("Enter 'exit' to exit\n\n");

    int c = fork();
    if(c > 0){
        close(fd1[0]);
        close(fd2[1]);
        
        size_t sz = 100;
        char *msg = (char *)malloc(sz* sizeof(char));
        while(true){
            printf("Enter in P :\n");
            int x = getline(&msg,&sz,stdin);
            msg[x-1] = '\0';

            write(fd1[1],msg,strlen(msg)+1);

            if(strcmp(msg,"exit") == 0){
                break;
            }

            read(fd2[0],msg,100);
            if(strcmp(msg,"exit") == 0){
                break;
            }
            printf("Recieved in P : '%s'\n\n",msg);
        }

        close(fd1[1]);
        close(fd2[0]);

        wait(NULL);
    }else{
        close(fd1[1]);
        close(fd2[0]);

        size_t sz = 100;
        char *msg = (char *)malloc(sz* sizeof(char));

        while(true){
            read(fd1[0],msg,20);
            if(strcmp(msg,"exit") == 0){
                break;
            }
            printf("Recieved in P' : '%s'\n\n",msg);

            printf("Enter in P' :\n");
            int x = getline(&msg,&sz,stdin);
            msg[x-1] = '\0';
            write(fd2[1],msg,strlen(msg)+1);
        }
        close(fd1[0]);
        close(fd2[1]);
    }
    return 0;
}