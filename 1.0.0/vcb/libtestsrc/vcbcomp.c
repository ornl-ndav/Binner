#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "vcbimage.h"
#include "vcbutils.h"
#include "vbutrcast.h"

#define MAXNUMPARTITIONS 64

int main (int argc, char ** argv)
{

  unsigned char * framebuffer, * pixels;

	int             dsize[3], sz[3], orig[3], centroid[3];
	float           eye[3], coi[3];
	int             ndims,nattribs;
	vcbdatatype     rawdatatype;

	vcbVolumeImg    imgs[MAXNUMPARTITIONS];
	int             ncuts, nparts, which[4];
	int             ndigits;
	char  filename[256];
	
	int i, j, k;
	int imgw, imgh;

	if (argc != 15) {
	  fprintf(stderr,"usage: %s input xdim ydim zdim ncuts ex ey ez cx cy cz imgszx imgszy output_image\n",argv[0]);
	  fprintf(stderr,"       input will be padded with .id.bin.ub: e.g. vort.01.bin.ub, while input = vort\n"); 
	  /* vcbcomp vort.01 128 128 128 2 300 300 300 64 64 64 512 512 vorts01.bmp */
	  exit(1);
	}

	dsize[0] = atoi(argv[2]);
	dsize[1] = atoi(argv[3]);
	dsize[2] = atoi(argv[4]);
	ncuts    = atoi(argv[5]);
	eye[0]   = atof(argv[6]);
	eye[1]   = atof(argv[7]);
	eye[2]   = atof(argv[8]);
	coi[0]   = atof(argv[9]);
	coi[1]   = atof(argv[10]);
	coi[2]   = atof(argv[11]);
	imgw     = atoi(argv[12]);
	imgh     = atoi(argv[13]);

	framebuffer = (unsigned char *)malloc(imgw*imgh*4);

	ndigits = 1;
	for (nparts = 1, i = 0; i < 3; i ++)
	  nparts *= ncuts;

	if (nparts >  9) ndigits = 2;
	if (nparts > 99) ndigits = 3;

	for (i = 0; i < nparts; i ++) {
          switch (ndigits) {
		        case 1: sprintf(filename, "%s.%.1d.bin.ub", argv[1],i);
                        break;
                case 2: sprintf(filename, "%s.%.2d.bin.ub", argv[1],i);
                        break;
                case 3: sprintf(filename, "%s.%.3d.bin.ub", argv[1],i);
                        break;
          }
	  
	  if (NULL == (pixels = vcbReadBinm(filename, &rawdatatype, &ndims, 
					    orig, sz, &nattribs))){
	    fprintf(stderr, "load_data: error loading datafile\n");
	    exit(-1);
	  }

	  imgs[i].fb = pixels;
	  imgs[i].origx = orig[1];
	  imgs[i].origy = orig[0];
	  imgs[i].w     = sz[1];
	  imgs[i].h     = sz[0];

	  for (j = 0, k = i; j < 3; k /= ncuts, j ++)
	    which[j] = k % ncuts;

	  /*printf("i = %d, which[j]: %d %d %d\n",i, which[0],which[1],which[2]);*/
	  
	  for (j = 0; j < 3; j ++) {
	    int interval = (int)ceil((double)dsize[j]/ncuts);
	    imgs[i].centroid[j] = (which[j] +0.5f) * interval;
	  }
	}

	vcbCompositeRegions(eye, coi, framebuffer, 4, imgw, imgh, nparts, imgs);

	printf("%s completed\n",argv[0]);
	vcbImgWriteBMP(argv[14],framebuffer, 4, imgw, imgh);
 
	for (i = 0; i < nparts; i ++)
	  free(imgs[i].fb);

	free(framebuffer);
	
	return 0;
}
