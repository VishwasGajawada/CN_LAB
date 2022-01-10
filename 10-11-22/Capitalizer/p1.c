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

// gcc p1.c -lpthread -o p1.exe
void *send_thread(void *arg){
    FILE *f = popen("./p2.exe","w");
    int fd = fileno(f);

    while(1){
        char buf[1024]={0};
        scanf("%s", buf);
        int x = strlen(buf);
        buf[x] = ' ';
        buf[x+1] = '\0';
        write(fd, buf, x+1);
    }
}
void *recv_thread(void *arg){
    char *path = "green";
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
        printf("recieved in p1 : '%s'\n\n", buf);
        fflush(stdout);
    }
}
int main(){

    // create 2 threads
    pthread_t writer, reader;
    pthread_create(&writer, NULL, send_thread, NULL);
    pthread_create(&reader, NULL, recv_thread, NULL);

    pthread_join(writer, NULL);
    pthread_join(reader, NULL);

    return 0;
    
}
