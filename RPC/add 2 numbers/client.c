#include<stdio.h>
#include<rpc/rpc.h>
#include "add.h"
int main()
{
	CLIENT *c1;
	add_in in;
	add_out *outp;
	c1=clnt_create("127.0.0.1",ADD_PROG,ADD_VERS,"tcp");
	printf("Enter two numbers number\n");
	printf("Enter first number :");
	scanf("%d",&in.a);
	printf("Enter second number :");
	scanf("%d",&in.b);
	
	outp=addproc_1(&in,c1);
	if(outp!=NULL)
		printf("The sum of two numbers is %d\n",outp->sum);
	exit(0);
	return 0;
}
