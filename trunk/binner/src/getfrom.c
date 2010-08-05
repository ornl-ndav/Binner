/**
   \ingroup rebinner_sdk

   \file src/getfrom.c

   \brief CURRENT sdk executable to stream input data from a network port.

   $Id$
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <unistd.h>
#include "socketfun.h"


int main(int argc, char ** argv)
{
	int sock;
	int i, n;
	static char netbuf[5000];

	if (0 > (sock = request_connection(argv[1], atoi(argv[2]))))
	{
		/*perror("get: cannot get a connection");*/
		exit(111);
	}
	
	for (n = 0; (i = read(sock, netbuf, 4096)) > 0; )
		n += write(1, netbuf, i); 
	
	fprintf(stderr,"wrote %d bytes.\n", n);

	close(sock);

	return 0;
}
