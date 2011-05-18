/**
   \file src/socketfun.c

   \brief Jim Plank's socket functions. Released as "do whatever you want with it".
   
   $Id$
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <time.h>

#include "socketfun.h"


/*static */ 
char * inadport_decimal(struct sockaddr_in *sad)
{
        static char buf[32];
        int a;

        a = ntohl(0xffffffff & sad->sin_addr.s_addr);
        sprintf(buf, "%d.%d.%d.%d:%d",
                        0xff & (a >> 24),
                        0xff & (a >> 16),
                        0xff & (a >> 8),
                        0xff & a,
                        0xffff & (int)ntohs(sad->sin_port));
        return buf;
}


int server_setup(char * dn, int * pn)
{
	int port, i, sock;

	srandom((unsigned long)time(NULL));

	for (i = 0; i < 20; i ++)
	{
		port = 6000 + (random()&0xfff); 
		/*fprintf(stderr, "%d\n",port);*/
		sock = serve_socket(dn, port);
		*pn = port;

		if (sock > 0)
			return sock;
	}

	fprintf(stderr, "server_setup: failed to get an open port after 20 tries\n");
	return -100;
}

int serve_socket(char *hn, int port)
{

  struct sockaddr_in sn;
  int s;
  struct hostent *he;

  if (!(he = gethostbyname(hn))) {
    puts("can't gethostname");
    exit(-101);
  }

  memset(&sn, 0, sizeof(sn));

  sn.sin_family = AF_INET;
  sn.sin_port     = htons((short)port);
  sn.sin_addr = *(struct in_addr*)(he->h_addr_list[0]);

  if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    /*perror("socket()");*/
    return -102;
  }
  if (bind(s, (struct sockaddr*)&sn, sizeof(sn)) == -1) {
    /*perror("bind()");*/
    return -103;
  }

  return s;
}

int accept_connection(int s)
{
  unsigned int l;
  struct sockaddr_in sn;
  int x;

  sn.sin_family = AF_INET;

  if (listen(s, 1) == -1) {
    /*perror("listen()");*/
    return -104;
  }

  l = sizeof(sn);
  if ((x = accept(s, (struct sockaddr*)&sn, &l)) == -1) {
    /*perror("accept()");*/
    return -105;
  }
  return x;
}

int request_connection(char *hn, int port)
{
  struct sockaddr_in sn;
  int s, ok;
  struct hostent *he;

  if (!(he = gethostbyname(hn))) {
    puts("can't gethostname");
    return -106;
  }
  ok = 0;
  while (!ok) {
    sn.sin_family = AF_INET;
    sn.sin_port  = htons((short)port);
    sn.sin_addr.s_addr = *(u_long*)(he->h_addr_list[0]);

    if ((s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
      perror("socket()");
      return -107;
    }
    ok = (connect(s, (struct sockaddr*)&sn, sizeof(sn)) != -1);
    if (!ok) return -108; /*sleep (1);*/
  }
  return s;
}

