struct add_in
{
	
	int a;
	int b;
};
struct add_out
{
	int sum;
};
program ADD_PROG
{
	version ADD_VERS
	{
		add_out ADDPROC(add_in)=1;
	} =1;
}= 0x3123000;
