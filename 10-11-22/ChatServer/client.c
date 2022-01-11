#include<stdio.h>
#include<unistd.h>

/*** for mkfifo() ***/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>           /* Definition of AT_* constants */
#include <sys/stat.h>

/** for errno **/
#include <errno.h>
#include <string.h>

/*** for pthread ***/
#include <pthread.h>

/** for atoi() **/
#include<stdlib.h>

// gcc client.c -lpthread -o client.exe
// ./client.exe <number>

void *send_thread(void *arg){
    int client_no = *(int *)arg;
    char *path = "channel2";
    if (mkfifo(path, 0666) == -1) {
        if(errno != EEXIST){
            printf("mkfifo error: ");
            fflush(stdout);
            perror(strerror(errno));
            return NULL;
        }
    }
    int wffd = open(path, O_WRONLY);

    while(1){
        char buf[1024]={0};
        scanf("%s", buf);
        char msg[1024] = {0};
        sprintf(msg,"%d_%s", client_no, buf);
        int x = strlen(msg);
        msg[x] = ' ';
        msg[x+1] = '\0';
        write(wffd, msg, strlen(msg)+1);
    }
}
void *recv_thread(void *arg){
    int client_no = *(int *)arg;
    char *path = "channel1";
    if (mkfifo(path, 0666) == -1) {
        if(errno != EEXIST){
            printf("mkfifo error: ");
            fflush(stdout);
            perror(strerror(errno));
            return NULL;
        }
    }
    int ffd = open(path, O_RDONLY);
    while(1){
        char buf[1024]={0};
        read(ffd, buf, 1024);
        // example : 1_hello
        int buf_client = buf[0]-'0';
        char *msg;
        msg = buf+2;

        if(buf_client != client_no){
            printf("sent by client%d : %s\n", buf_client, msg);
            fflush(stdout);
        }
    }
}
int main(int argc, char **argv){

    int client_no = atoi(argv[1]);
    printf("Client number : %d\n\n", client_no);
    // create 2 threads
    pthread_t writer, reader;
    pthread_create(&writer, NULL, send_thread, &client_no);
    pthread_create(&reader, NULL, recv_thread, &client_no);

    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    return 0;
    
}
