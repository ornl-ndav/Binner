#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "socketfun.h"

#define fullsz 4*1024*1024 
char s[fullsz];

int fastinout(int in, int out)
{
  int i, l, c;

  i = 0;
  l = 0;
  c = 0;

  while((l = read(in, s+i, fullsz))!= 0) {
	i += l;
    if (i > 1024*1024) 
	{
      write(out, s, i);
	  c += i;
      i = 0;
    }
  }

  write(out, s, i);
  c += i;
  return c;
}

int inout(int in, int out)
{
  int i, l;

  i = 0;
  l = 0;
  while(read(in, s+i, 1) != 0) {
    if (s[i] == '\n') {
      write(out, s, i+1);
      l ++;
      i = 0;
    }
    else
      i++;
  }
  
  write(out, s, i);
  return l;
}

main(int argc, char **argv)
{
  char *hn, *un;
  int port, sock, fd;
  int i;
  clock_t time1, time2;
  float t;

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

  fprintf(stderr, "server running on   : %s\n", hn); 
  fprintf(stderr, "port number         : %d\n", port);

  fd = accept_connection(sock);

  time1 = clock();
  i = fastinout(fd, 1);
  time2 = clock();

  t  = (float)(time2-time1)/CLOCKS_PER_SEC;
  fprintf(stderr, "number of bytes read: %d\n", i);   
  fprintf(stderr, "time spent          : %.2f seconds\n", t);
  fprintf(stderr, "input bandwidth     : %8.4f MB/second\n", (float)i/t/1e6f);

  return 0;
}
