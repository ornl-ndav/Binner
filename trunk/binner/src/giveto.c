#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <unistd.h>
#include <signal.h>
#include "socketfun.h"

/**
 * $Id$
 *
 */

int sock, n = 0;
char netbuf[5000];

void sigpipe_handler(int dummy)
{
	/* keep on reading until done */
	for ( ; read(0, netbuf, 4096) > 0; ); 
	
	close (sock);
	exit(2);
}


int main(int argc, char ** argv)
{
	int i, n;

	signal(SIGPIPE, sigpipe_handler);
  
	if (0 > (sock = request_connection(argv[1], atoi(argv[2]))))
	{
		perror("giveto: cannot get a connection");
		exit(1);
	}
	
	for (n = 0; (i = read(0, netbuf, 4096)) > 0; )
		n += write(sock, netbuf, i); 
		
	close(sock);

	return 0;
}
