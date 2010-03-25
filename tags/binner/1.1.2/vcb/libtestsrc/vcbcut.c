#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vcbutils.h"

#include <math.h>

#define MAXDIMS 4

int main(int argc, char ** argv)
{
  vcbdatatype datatype;
  int ndims, orig[MAXDIMS], dsize[MAXDIMS], nattribs, i;
  int lbounds[MAXDIMS], ubounds[MAXDIMS];
  int ncuts, k, which[MAXDIMS], cutsize[MAXDIMS];
  float full;
  int interval, total;
  void * outdata;

  FILE * fp;

  int headlen;

  /* See if user passed an argument. */
  if (argc != 5) {
    fprintf(stderr, "Usage: %s datafile n k outrootname\n", argv[0]);
    exit(-1);
  }

   vcbReadHeaderBinm(argv[1], &datatype, &ndims, orig, dsize, &nattribs);

   ncuts = atoi(argv[2]);
   k     = atoi(argv[3]);

   for (i = 0; i < ndims; k /= ncuts, i ++)
     which[i] = k % ncuts;

   printf("vcbcut outputs"); 
   for (i = 0; i < ndims; i ++) {
     full = dsize[i];
     interval = (int)ceil(full/ncuts);
     lbounds[i] = interval * which[i];
     ubounds[i] = interval * (which[i]+1);/*if don't want overlap, then -1*/ 
     if (ubounds[i] >= dsize[i]) ubounds[i] = dsize[i]-1;

     cutsize[i] = ubounds[i] - lbounds[i] + 1;
     printf(": x%d [%d, %d] ",i, lbounds[i],ubounds[i]);
   }
   printf("\n");

   for (total = 1, i = 0; i < ndims; total *= cutsize[i], i++);
   outdata = malloc(total*nattribs*vcbSizeOf(datatype));

   fp = fopen(argv[1],"rb");
   headlen = (3+2*ndims)*sizeof(int)+sizeof(datatype);/*vcbSizeOf(datatype);*/

   vcbFileGrablk(fp, headlen, outdata, 
		 nattribs,
		 vcbSizeOf(datatype),
		 ndims,
		 dsize,
		 lbounds, 
		 ubounds, 
		 NULL);

   for (i = 0; i < ndims; orig[i]+=lbounds[i], i++);
   vcbGenBinm(argv[4], datatype, ndims, orig, cutsize, nattribs, outdata);
 
   free(outdata);

   fclose(fp);

   return 0;
}
