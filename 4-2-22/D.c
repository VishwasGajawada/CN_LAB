#include <stdio.h>
#include <stdlib.h>
int main(int argc, char **argv){

    int data_server_id = 1;
    if(argc > 1) data_server_id = atoi(argv[1]);
    printf("I am data server %d\n", data_server_id);
    return 0;
}
