#include <stdio.h>
#include <stdlib.h>

#include "vcbutils.h"

#define MAXDIMS 4

int main (int argc, char ** argv)
{
  vcbdatatype datatype;
  int ndims, orig[MAXDIMS], dsize[MAXDIMS], nattribs, i;

  /* See if user passed an argument. */
   if (argc != 2) {
      fprintf(stderr, "Usage: %s datafile\n", argv[0]);
      exit(1);
   }

   vcbReadHeaderBinm(argv[1], &datatype, &ndims, orig, dsize, &nattribs);

   printf("vcbInfo  : %s\n",argv[1]);
   printf("datatype : %s\n",vcbStrDataType(datatype));
   printf("num. dims: %d\n",ndims);
   printf("num. vals: %d\n", nattribs);
   printf("origin   :");
   for (i = 0; i < ndims; i ++)
     printf(" %d",orig[i]);
   printf("\n");

   printf("dimenions:");
   for (i = 0; i < ndims; i ++)
     printf(" %d",dsize[i]);
   printf("\n");


   return 0;
}
