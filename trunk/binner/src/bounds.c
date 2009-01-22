#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#include "atypes.h"
#include "cell.h"
#include "binnerio.h"
#include "vcbutils.h"

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
  double red;

  abbox.low[0] = abbox.low[1] = abbox.low[2] = 1e6;
  abbox.high[0] = abbox.high[1] = abbox.high[2] = -1e6;

  for (npara = 0; (n = get_pixelf(&sliceid,&hitcnt,&hiterr,corners)) > 0; ) {

	correctCornersf3d(corners);
	realCubef(corners, vdata);

    for (i = 0; i < 6*4; i ++) {
		abbox.low [0] = VCB_MINVAL(abbox.low [0],vdata[i*4]);
		abbox.high[0] = VCB_MAXVAL(abbox.high[0],vdata[i*4]);
		abbox.low [1] = VCB_MINVAL(abbox.low [1],vdata[i*4+1]);
		abbox.high[1] = VCB_MAXVAL(abbox.high[1],vdata[i*4+1]);
		abbox.low [2] = VCB_MINVAL(abbox.low [2],vdata[i*4+2]);
		abbox.high[2] = VCB_MAXVAL(abbox.high[2],vdata[i*4+2]);
    }

	npara ++;

	if (argc > 1)
		if (npara % 50000 == 0)
		{
		  printf("number of parallelipeds = %d\n",npara);
		  printf("bounding box: (%f %f %f) (%f %f %f)\n",
				abbox.low[0], abbox.low[1], abbox.low[2], 
				abbox.high[0],abbox.high[1],abbox.high[2]);
		}
  }

  printf("number of parallelipeds = %d\n",npara);
  printf("bounding box: (%f %f %f) (%f %f %f)\n",abbox.low[0], abbox.low[1],
	 abbox.low[2], abbox.high[0],abbox.high[1],abbox.high[2]);
  
  return 0;
}
