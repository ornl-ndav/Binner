#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vcbutils.h"

#include <math.h>

#define MAXDIMS 8

float castfrom_ub(char * p)
{
  unsigned char * ptr = (unsigned char *) p;
  return (float) *ptr;
}

float castfrom_b(char * p)
{
  char * ptr = (char *) p;
  return (float) *ptr;
}

float castfrom_us(char * p)
{
  unsigned short * ptr = (unsigned short *) p;
  return (float) *ptr;
}

float castfrom_s(char * p)
{
  short * ptr = (short *) p;
  return (float) *ptr;
}

float castfrom_ui(char * p)
{
  unsigned int * ptr = (unsigned int *) p;
  return (float) *ptr;
}

float castfrom_i(char * p)
{
  int * ptr = (int *) p;
  return (float) *ptr;
}

float castfrom_f(char * p)
{
  float * ptr = (float *) p;
  return (float) *ptr;
}

float castfrom_d(char * p)
{
  double * ptr = (double *) p;
  return (float) *ptr;
}

typedef float (*cast_funcptr) (char * data);

cast_funcptr func_tab[9] = {castfrom_ub,          /* unsigned byte */
			    castfrom_b,           /* byte */
			    castfrom_us,          /* unsigned short */
			    castfrom_s,           /* short */
			    castfrom_ui,          /* unsigned int */
			    castfrom_i,           /* int */
			    castfrom_f,           /* float */
			    castfrom_d,           /* double */
			    NULL};                /* bitfield */


int main(int argc, char ** argv)
{
  vcbdatatype datatype;
  int ndims, orig[MAXDIMS], dsize[MAXDIMS], nattribs, endian, i, j;
  int lbounds[MAXDIMS], ubounds[MAXDIMS];
  int ncuts, k, which, cutsize[MAXDIMS];
  float full;
  int interval, total, thistime;
  void * outdata;
  char * cp;
  int nbytes;

  float minval, maxval, val;
  FILE * fp;
  int headlen;

  cast_funcptr castval;


  /* See if user passed an argument. */
  if (argc != 4) {
    fprintf(stderr, "Usage: %s datafile ncuts attri_id \n", argv[0]);
    exit(-1);
  }

  if (0 > vcbReadHeaderBinm(argv[1], &datatype, &ndims, orig, dsize, &nattribs)) {
    /*fprintf(stderr,"%s cannot be opened\n",argv[1]);*/
    exit(-1);
  }

   castval = func_tab[datatype];
   if (castval == NULL) {
     fprintf(stderr,"%s cannot handle this data type. quitting ...\n",argv[0]);
     exit(0);
   }

   ncuts = atoi(argv[2]);
   if (ncuts < 1) {
     fprintf(stderr,"vcbrange cannot do it within %d times\n",ncuts);
     exit(-1);
   }

   which = atoi(argv[3]);
   nbytes = vcbSizeOf(datatype);

   /*printf("ncuts %d  which %d\n",ncuts, which);*/
   if (which >= nattribs) {
      printf("attribute %d does not exist, since nattribs = %d\n", which,nattribs);
      exit(-1);
   }

   fp = fopen(argv[1],"rb");
   headlen = (3+2*ndims) * sizeof(int) + sizeof(datatype);/*nbytes;*/
   fread(&endian, sizeof(int), 1, fp);

   for (i = 0; i < ndims - 1; i ++) {
     lbounds[i] = 0;
     ubounds[i] = dsize[i] -1;
     /*printf(": %d %d", lbounds[i],ubounds[i]);*/
   }
   /*printf("\n");*/

   full = dsize[ndims-1];
   interval = (int)ceil(full/ncuts);
   for (total = 1, i = 0; i < ndims-1; total *= dsize[i], i++);

   /*printf("total = %d, interval = %d\n", total, interval);*/

   k = ndims - 1;
   minval =  1e8;
   maxval = -1e8;
   /*printf("k = %d, ndims = %d\n",k, ndims);*/

   printf("%s attribute %d valued between ",argv[1],which);

   outdata = malloc(total*interval*nattribs*nbytes*2);
   if (outdata == NULL) {
     perror(" outdata allocating failed");
     exit(-1);
   }

   for (i = 0; i < ncuts; i ++) {
     /*printf("i = %d\n",i);*/

     lbounds[k] = interval * i;
     ubounds[k] = interval * (i+1) - 1;

     if (ubounds[k] >= dsize[k]) 
       ubounds[k] = dsize[k] - 1;

     /*printf("l-u: %d %d\n",lbounds[k], ubounds[k]);*/

     vcbFileGrablk(fp, headlen, outdata, 
		   nattribs,
		   nbytes,
		   ndims,
		   dsize,
		   lbounds, 
		   ubounds, 
		   NULL);

     thistime = total * (ubounds[k] - lbounds[k] + 1);
     cp = outdata;

     if (endian == 1) {
       for (j = 0, cp += which * nbytes; j < thistime; cp += nattribs*nbytes, j ++) {
	 val = castval(cp);
	 if (minval > val) minval = val;
	 if (maxval < val) maxval = val;
       }
     }
     else {
       for (j = 0, cp += which * nbytes; j < thistime; cp += nattribs*nbytes, j ++) {
	 vcbToggleEndian(cp, nbytes);
	 val = castval(cp);
	 if (minval > val) minval = val;
	 if (maxval < val) maxval = val;
       }
     }
   }
 
   free(outdata);
   printf("%f and %f\n",minval, maxval);

   fclose(fp);

   return 0;
}
