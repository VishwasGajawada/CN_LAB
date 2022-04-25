/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "add.h"

add_out *
addproc_1_svc(add_in *argp, struct svc_req *rqstp)
{
	static add_out  result;

	/*
	 * insert server code here
	 */
	result.sum = argp->a + argp->b;
	printf ("%d + %d = %d", argp->a, argp->b, result.sum);
	return &result;
}
