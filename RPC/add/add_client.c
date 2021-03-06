/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

#include "add.h"


void
add_prog_1(char *host, int x, int y)
{
	CLIENT *clnt;
	add_out  *result_1;
	add_in  addproc_1_arg;

#ifndef	DEBUG
	clnt = clnt_create (host, ADD_PROG, ADD_VERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}
#endif	/* DEBUG */

	result_1 = addproc_1(&addproc_1_arg, clnt);
	if (result_1 == (add_out *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	else {
		printf("Sum of %d and %d is = %d\n", x, y, result_1->sum);
	}
#ifndef	DEBUG
	clnt_destroy (clnt);
#endif	 /* DEBUG */
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 4) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	add_prog_1 (host, atoi(argv[2]),atoi(argv[3]));
exit (0);
}
