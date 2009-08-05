#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "vcbutils.h"
#include "vbutrcast.h"
#include "vcbimage.h"

/*unsigned char * voxels;*/
float  *voxels;
int             sz[3], orig[3];
int             ndims,nattribs;
vcbdatatype     rawdatatype;

char *          binfile;
int             binfile_len;

/* application specific data starts here */


/* end of application specific data */



void load_data(char * dataname)
{
	int i, j, k;
	float minv, maxv;
	
	if ((voxels = vcbReadBinm(dataname, &rawdatatype, &ndims, orig, sz, &nattribs)) 
	    == NULL) {
		fprintf(stderr, "load_data: error loading datafile\n");
		exit(-1);
	}


	/*printf("loadBinm succeeded\n");*/

	/*
	minv = 1e6;
	maxv = -1e6;
	for (i = 0; i < 128*128*128; i++) {
	  if (minv > voxels[i]) minv = voxels[i];
	  if (maxv < voxels[i]) maxv = voxels[i];
	}
	*/
	/*
	printf("in load_data: minv %f, maxv %f\n",minv, maxv);
	printf("after vcbLoadBinm %d datatype, %d dims, %d sz, %d attribs\n",
	       rawdatatype,ndims,sz[0],nattribs);
	*/

}


int main (int argc, char ** argv)
{
  clock_t time1, time2;
  int   base = 0;
  char * dst;

  float ipos[MAXNUMISORANGE], irange[MAXNUMISORANGE], iopa[MAXNUMISORANGE];
  float minval, maxval;
  unsigned char * fbuffer;
  float eye[3] = {160.f, 32.f, 160.f};
  float coi[3] = {432.f, 432.f, 432.f};
  float up[3]  = {0.f, 1.f, 0.f};
  int   numlights = 1, numiso, colorscheme;
  float lightpos[3] = {32.f, 32.f, 160.f};
  int   w, h, imgbounds[4];
  int   i, cnt;
  float hither, yon, fov, aspect, xmin, xmax, ymin, ymax;
  int   whichview;
  float radius;
  float stepsz;
  int  imgsz[2], regionsz[2];
  unsigned char * frame;

  if (argc < 16) {
    fprintf(stderr,"usage: %s dataset color minv maxv ex ey ez cx cy cz imgsz frustumsz stepsz [ipos irange ipa] \n",argv[0]);
    exit(1);
  }

  load_data(argv[1]);
  colorscheme = atoi(argv[2]);
  minval      = atof(argv[3]);
  maxval      = atof(argv[4]);
  eye[0]      = atoi(argv[5]);
  eye[1]      = atof(argv[6]);
  eye[2]      = atof(argv[7]);
  coi[0]      = atoi(argv[8]);
  coi[1]      = atof(argv[9]);
  coi[2]      = atof(argv[10]);
  if (minval >= maxval) {
    fprintf(stderr,"minval needs to be smaller than maxval. quitting ...\n");
    exit(1);
  }

  for (i = 14, cnt = 0; (i+2) < argc; i += 3) {
    ipos[cnt]   = atof(argv[i  ]);
    irange[cnt] = atof(argv[i+1]);
    iopa[cnt]   = atof(argv[i+2]);
    cnt ++;
    if (cnt >= MAXNUMISORANGE)
      break;
  }
  numiso = cnt;
  if (numiso < 1) {
    fprintf(stderr,"cannot proceed since number of intervals is: %d\n",numiso);
    exit(1);
  }

  printf("%s, maxval %.3f, %d intervals, ", argv[1],maxval, numiso);
  stepsz = atof(argv[13]);
  printf("%.2f stepsz: ",stepsz);
  fflush(stdout);

  vbutDefineTransfunc(colorscheme, numiso, minval, maxval-minval, ipos, irange, iopa);
  
  radius = atof(argv[12]);
  /*if (radius < sz[1]) radius = sz[1];*/
  /*if (radius < sz[2]) radius = sz[2];*/
  lightpos[0] = eye[0];
  lightpos[1] = eye[1];
  lightpos[2] = eye[2];
  time1 = clock();

  w = h = atoi(argv[11]);
  hither = 1.f;   yon    = 1000.f;
  xmin   = -radius*1.f; xmax   = radius*1.f;
  ymin   = -radius*1.f; ymax   = radius*1.f;

  fbuffer = vbutrcastOrtho(voxels,rawdatatype, 
			   orig, sz, 
			   eye, coi, up, 
			   numlights, lightpos, 
			   w, h, 
			   imgbounds, 
			   hither, yon, xmin, xmax, ymin, ymax,stepsz);

  if (fbuffer != NULL)
    vcbImgWriteBMP("rcast.bmp",fbuffer, 4, w, h);

  imgsz[0] = w;
  imgsz[1] = h;
  regionsz[0] = imgbounds[2] - imgbounds[0] + 1;
  regionsz[1] = imgbounds[3] - imgbounds[1] + 1;

  frame = (unsigned char *) malloc(regionsz[0]*regionsz[1]*4);
  vcbGrablk(fbuffer, frame, 4, 1, 2, imgsz, imgbounds, imgbounds+2, NULL);
  free(fbuffer);

  vcbGenBinm("rcast.bin.ub", VCB_UNSIGNEDBYTE,2,imgbounds, regionsz,4,frame);
  free(frame);

  time2 = clock();
  printf(" took %.2f sec\n",(float)(time2-time1)/CLOCKS_PER_SEC);
  return 0;
}
