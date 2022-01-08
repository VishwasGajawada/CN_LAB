#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

int main(){
    char ch[30];
    scanf("%s",ch);
    printf("In p2 recieved: %s\n",ch);
    return 0;
}