#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include<string.h>
#include <stdlib.h>

int readint(int *f,int *x){
	char buf[10],c;
	int i=0;
	while(read(*f,&c,sizeof(char))){
		if(c!=' ')break;
	}
	if(c=='-' || (c>='0' && c<='9'))buf[i++] = c;
	while(read(*f,&c,sizeof(char))){
		if(c==' '){
			break;
		}
		buf[i++] = c;
	}
	buf[i]='\0';
	if(i==0)return -1;
	*x = atoi(buf);
	return 0;
}

int main(){
	int f1 = open("f1.txt",O_RDONLY);
	int f2 = open("f2.txt",O_RDONLY);
	
	if(f1==-1 || f2==-1){
		printf("error : create 2 sorted files f1 and f2\n");
		return 0;
	}
	
	int f3 = open("f3.txt",O_WRONLY|O_CREAT|O_TRUNC,S_IRWXU);
	int n1,n2,x1,x2;
	x1 = readint(&f1,&n1);
	x2 = readint(&f2,&n2);
	int cnt = 0;
	char buf[10];
	while(x1 != -1 || x2 != -1){
		if(x1!=-1 && (x2==-1 || n1<n2)){
			sprintf (buf, "%d ", n1);
			write(f3,&buf,strlen(buf));
			x1 = readint(&f1,&n1);
		}else if(x2!=-1 && (x1==-1 || n2<=n1)){
			sprintf (buf, "%d ", n2);
			write(f3,&buf,strlen(buf));
			x2 = readint(&f2,&n2);
		}else break;
	}
	write(f3,"\n",1);
	return 0;
}
