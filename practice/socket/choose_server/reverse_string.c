#include <stdio.h>
#include <string.h>
int main(){
    printf("Enter the string: \n");
    fflush(stdout);
    
    char str[100];
    scanf("%s",str);
    int i = 0, j = strlen(str)-1;
    while(i<j){
        char temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
    printf("%s\n",str);
    return 0;
}