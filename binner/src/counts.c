#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#include "atypes.h"
#include "binnerio.h"

/* application specific data starts here */
int     nverts, npara;
float   vdata[1024];

/* end of application specific data */
bbox abbox;

char inputbuf[1024];

int list_id;

float xmin, xmax, ymin, ymax;

int main(int argc, char ** argv)
{
  int   i, j, n, ndims, orig, nvals, dummy;
  int   sliceid;
  float hitcnt, hiterr, corners[8][4];
  float totalhitcnt, totalhiterr;
  float maxcnt, maxerr;
  double red;

  totalhitcnt = 0;
  totalhiterr = 0;
  maxcnt = 0;
  maxerr = 0;
  
  for (npara = 0; (n = get_pixelf(&sliceid,&hitcnt,&hiterr,corners)) > 0; npara ++) {
	totalhitcnt += hitcnt;
	totalhiterr += hiterr;
	if (maxcnt < hitcnt) maxcnt = hitcnt;
	if (maxerr < hiterr) maxerr = hiterr;

	if (npara % 50000 == 0)
	{
	   printf("number of parallelipeds = %d\n",npara);
	   printf("tc: %e mc: %e te: %e me: %e\n", totalhitcnt, maxcnt, totalhiterr, maxerr);
	}	
  }

  printf("number of parallelipeds = %d\n",npara);
  printf("tc: %e mc: %e te: %e me: %e\n", totalhitcnt, maxcnt, totalhiterr, maxerr);

  return 0;
}
