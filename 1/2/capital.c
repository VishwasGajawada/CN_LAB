#include<stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

char capitalize(char c){
	if(c>='a' && c<='z'){
		c = c-'a' + 'A';
	}
	return c;
}

int main(){
	int f1 = open("f1.txt",O_RDONLY);
	
	if(f1==-1){
		printf("error\n");
		return 0;
	}
	
	int f2 = open("f2.txt",O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
	char c;
	bool firstletter = true;
	while(read(f1,&c,1)){
		if(c==' '){
			firstletter = true;
		}else{
			if(firstletter){
				c = capitalize(c);
			}
		}
		write(f2,&c,1);
	}
	write(f2,"\n",1);
	return 0;
}
