#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "socketfun.h"

#define fullsz 4*1024*1024 
char s[fullsz];

int inout(int in, int out)
{
  int i, l, last;

  i = 0;
  last = 0;
  l = 0;
  while(read(in, s+i, 1) != 0) {
    if (s[i] == '\n') {
      write(out, s+last, i+1 - last);
	  last = i+1;
	  l ++;
    }
    i++;
  }
  return l;
}

main(int argc, char **argv)
{
  char *hn, *un;
  int port, sock, fd;
  int i;

  if (argc != 3) {
    fprintf(stderr, "usage: %s hostname port\n",argv[0]);
    exit(1);
  }

  hn = argv[1];
  port = atoi(argv[2]);
  if (port < 5000) {
    fprintf(stderr, "usage: %s hostname port\n",argv[0]);
    fprintf(stderr, "       port must be > 5000\n");
    exit(1);
  }

  sock = serve_socket(hn, port);

  fprintf(stderr,"%s up running on %s at port %d until killed ...\n", argv[0], hn, port);

  fd = accept_connection(sock);
  
  fprintf(stderr,"done reading %d lines\n",inout(fd, 1));
  
  return 0;
}
