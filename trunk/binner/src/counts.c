#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#include "atypes.h"
#include "binnerio.h"

/**
 * $Id$
 *
 */

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
  int   n, nzero, sliceid;
  float hitcnt, hiterr, corners[8][4];
  float totalhitcnt, totalhiterr;
  float maxcnt, maxerr, mincnt, minerr;

  nzero = 0;
  totalhitcnt = 0;
  totalhiterr = 0;
  maxcnt = 0;
  maxerr = 0;
  mincnt = 1e6;
  minerr = 1e6;
  
  for (npara = 0; (n = get_pixelf(&sliceid,&hitcnt,&hiterr,corners)) > 0; ) {
	if (hitcnt > 1e-16) 
	{
		nzero ++;

		totalhitcnt += hitcnt;
		totalhiterr += hiterr;
		if (maxcnt < hitcnt) maxcnt = hitcnt;
		if (maxerr < hiterr) maxerr = hiterr;
		if (mincnt > hitcnt) mincnt = hitcnt;
		if (minerr > hiterr) minerr = hiterr;

	}

	npara ++;

	if (argc > 1)
		if (npara % 50000 == 0)
		{
			   printf("number of parallelipeds = %d, nonempty = %d\n",npara, nzero);
			   printf("tc: %e c: [%e, %e]\nte: %e e: [%e, %e]\n", 
					  totalhitcnt, mincnt, maxcnt, totalhiterr, minerr, maxerr);
		}
  }

  printf("number of parallelipeds = %d, nonempty = %d\n",npara, nzero);
  printf("tc: %e c: [%e, %e]\nte: %e e: [%e, %e]\n", 
         totalhitcnt, mincnt, maxcnt, totalhiterr, minerr, maxerr);

  return 0;
}
