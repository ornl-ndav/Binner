/**
   \ingroup rebinner_tests
   \file src/bslicer.c
   
   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vcbimage.h"
#include "vcbutils.h"


#define MAXDIMS 4

int main(int argc, char ** argv)
{
	vcbdatatype datatype;
	int ndims, nattribs, imgsz[2];
	int orig[MAXDIMS], dsize[MAXDIMS];
	int lb[MAXDIMS], ub[MAXDIMS];
	double * rawdata;
	double minval, maxval, pixel;

	int slice_dim, slice_no, nval;
	unsigned char * img;
	int i, j, k, l, m;

	if (argc != 6) {
		fprintf(stderr, "usage: %s dataset slice_dim slice_id minval maxval\n", argv[0]);
		return -1;
	}

	slice_dim = atoi(argv[2]);
	slice_no  = atoi(argv[3]);
	minval    = atof(argv[4]);
	maxval    = atof(argv[5]);

	/*printf("minval maxval: %lf %lf\n", minval, maxval);*/

	/* input */
	rawdata = vcbReadBinm(argv[1], &datatype, &ndims, orig, dsize, &nattribs);
	if (rawdata == NULL) {
		perror("problem with loading data");
		exit(-1);
	}
	
	/*printf("%u %u \n", rawdata[1], rawdata[301]);*/

	if (nattribs != 1) {
		fprintf(stderr,
				"%s will slice the first attribute of %d attributes in %s\n", 
				argv[0], nattribs, argv[1]);
	}

	if (datatype != VCB_DOUBLE) {
		fprintf(stderr, "%s can only handle data of unsigned int type\n", argv[0]);
		free(rawdata);
		exit(-1);
	}

	for (i = 0; i < ndims; lb[i] = 0, ub[i] = dsize[i] - 1, i ++);
	lb[slice_dim] = ub[slice_dim] = slice_no;

	printf("dsize (%d,%d,%d). ", dsize[0],dsize[1],dsize[2]);
	printf("slice (%d,%d,%d) - ", lb[0], lb[1], lb[2]);
	printf("(%d,%d,%d). ", ub[0], ub[1], ub[2]);

	/*for (i = 0; i < ndims; printf("%d %d ",lb[i],ub[i]), i ++);*/

	for (i = 0, nval = 1; i < ndims; nval *= (ub[i]-lb[i]+1), i ++);
	printf("%d pixels\n", nval);
	
	img = malloc(nval*3);
	if (img == NULL) {
		perror("problem allocating img");
		exit(-1);
	}

	m = 0;
	for (i = lb[0]; i <= ub[0]; i ++)
		for (j = lb[1]; j <= ub[1]; j ++)
			for (k = lb[2]; k <= ub[2]; k ++)
			{
				l = (i * dsize[1] + j) * dsize[2] + k;
				pixel = (rawdata[l*nattribs]-minval)*255/(maxval-minval);
				if (pixel < 0) pixel = 0;
				if (pixel > 255) pixel = 255;
				img[m*3] = img[m*3+1] = img[m*3+2] = (unsigned char)pixel;
				m ++;
			}

	for (i = 0, j = 0; i < ndims; i ++) {
	  if (i != slice_dim) {
	    imgsz[j] = dsize[i];
	    j ++;
	  }
	  if (j > 1)
	    break;
	}
	
	/* output */
	vcbImgWriteBMP("bslice.bmp", img, 3, imgsz[1], imgsz[0]);

	free(img);
	free(rawdata);
	
	return 0;
}
