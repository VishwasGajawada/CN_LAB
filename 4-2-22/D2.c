#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
int main(int argc, char **argv){

    int data_server_id = 2;
    if(argc > 1) data_server_id = atoi(argv[1]);
    while(1){
        printf("I am data server %d\n", data_server_id);
        fflush(stdout);
        sleep(3);
    }
    return 0;
}
