#include<stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool isnum(char c){
	return (c>='0' && c<='9');
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
	int char_head = 0, num_head = 1000;
	while(read(f1,&c,1)){
		if(isnum(c)){
			lseek(f2,num_head,SEEK_SET);
			write(f2,&c,1);
			num_head++;
		}else{
			lseek(f2,char_head,SEEK_SET);
			write(f2,&c,1);
			char_head++;
			
		}
	}
	// replacing empty characters with space
	c = ' ';
	lseek(f2,char_head,SEEK_SET);
	while(char_head < 1000){
		write(f2,&c,1);
		char_head++;
	}
	lseek(f2,num_head,SEEK_SET);
	write(f2,"\n",1);
	return 0;
}
