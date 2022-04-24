#include<stdio.h>
#include<rpc/rpc.h>
#include "add.h"
add_out *
addproc_1_svc(add_in *inp,struct svc_req *rqstp)
{
	static add_out out;
	out.sum=inp->a+inp->b;
	return (&out);
}
