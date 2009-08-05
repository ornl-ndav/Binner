#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vcbvolren.h"
#include "vcbcolor.h"
#include "vbutrcast.h"


static onedTransf vbutTransf;

void vbutDefineTransfunc(int colorscheme, 
			 int numIso, 
			 float minval, float range, 
			 float * ipos, float * isize, float * iopa)
{
  int i;

  vbutTransf.colorScheme = colorscheme;
  vbutTransf.numIsoRange = numIso;
  vbutTransf.minval = minval;
  vbutTransf.range  = range;
  
  for (i = 0; i < numIso; i ++) {
    vbutTransf.posIsoRange[i]  = ipos[i];
    vbutTransf.sizeIsoRange[i] = isize[i];
    vbutTransf.opaIsoRange[i]  = iopa[i];
  }

}

void compute_trans(onedTransf * ir)
{
  int i, j;
  int lb, ub;
  float c, f, r, opa, alpha, tmp;

  if ((ir->colorScheme == 0) || (ir->colorScheme == 3))
    vcbCustomizeColorTableFullColor(ir->rgbalut, 0, 255);
  else if (ir->colorScheme == 1)
    vcbCustomizeColorTableColdHot(ir->rgbalut, 0, 255);
  else
    vcbCustomizeColorTableBlackWhite(ir->rgbalut, 0, 255);

  if (ir->colorScheme == 3) {
	  for(i = 0, j = TRANSIZE -1; i<j; i++, j--) {
		  tmp = ir->rgbalut[i*4+0];
		  ir->rgbalut[i*4+0] = ir->rgbalut[j*4+0];
		  ir->rgbalut[j*4+0] = tmp;
                  tmp = ir->rgbalut[i*4+1];
		  ir->rgbalut[i*4+1] = ir->rgbalut[j*4+1];
		  ir->rgbalut[j*4+1] = tmp;
		  tmp = ir->rgbalut[i*4+2];
		  ir->rgbalut[i*4+2] = ir->rgbalut[j*4+2];
		  ir->rgbalut[j*4+2] = tmp;
	  }

  }

  for (i = 0; i < TRANSIZE; i ++)
    ir->rgbalut[i*4+3] = 0;

  for (i = 0; i < ir->numIsoRange; i ++) {
    c   = ir->posIsoRange[i];
    r   = ir->sizeIsoRange[i];
    c   = (c - ir->minval)/ir->range*(TRANSIZE-1);
    r   = r/ir->range*(TRANSIZE-1);
    lb  = (int)ceil(c - r);
    ub  = (int)(c + r);
    opa = ir->opaIsoRange[i];

    if (lb >= TRANSIZE) continue;
    if (ub < 0)         continue;
    if (lb > ub)        continue;
    if (lb < 0)         lb = 0;
    if (ub >= TRANSIZE) ub = TRANSIZE - 1;


    for (j = lb; j <= ub; j ++) {
      f = (j < c) ? (c-j) : (j-c);
      alpha = (1.f - f/r)*opa;
      if (alpha > ir->rgbalut[j*4+3])
	ir->rgbalut[j*4+3] = alpha;
    }
  }
}

void vbutTransfuncub (float * rgba, void * vox)
{
   unsigned char * voxel = (unsigned char *) vox;
   int id = voxel[0] * 4;
   rgba[0] = vbutTransf.rgbalut[id+0];
   rgba[1] = vbutTransf.rgbalut[id+1];
   rgba[2] = vbutTransf.rgbalut[id+2];
   rgba[3] = vbutTransf.rgbalut[id+3];
}

void vbutTransfuncf (float * rgba, void * vox)
{
   int id;
   float val;
   float * voxel = (float *) vox;
   val = (voxel[0] - vbutTransf.minval)/vbutTransf.range * (TRANSIZE-1);
   if ((val < 0) || (val >= TRANSIZE)) {
     rgba[0] = rgba[1] = rgba[2] = rgba[3] = 0.f;
   }
   else {
     id = (int)(val + 0.5);
     id = id * 4;
     rgba[0] = vbutTransf.rgbalut[id+0];
     rgba[1] = vbutTransf.rgbalut[id+1];
     rgba[2] = vbutTransf.rgbalut[id+2];
     rgba[3] = vbutTransf.rgbalut[id+3];
   }
}

void * vbutrcastOrtho(void * voxels, vcbdatatype datatype, 
		      int *orig, int * dsize,
		      float * eye, float * coi, float * up,
		      int numlights, float * lightpos,
		      int w, int h,
		      int * imgbounds,
		      float hither, float yon,
		      float xmin, float xmax, 
		      float ymin, float ymax,
			  float stepsz)
{
  VCB_volren volren;
  unsigned char * framebuffer;
  float scales[3] ={1.f, 1.f, 1.f};
  int i;

  VCB_TRANSFUNC transf;

  if ((w < 0) || (w > 4096))
    return NULL;
  if ((h < 0) || (h > 4096))
    return NULL;

  framebuffer = (unsigned char *) malloc (w*h*4*sizeof(unsigned char));
  if (framebuffer == NULL) {
    printf("cannot allocated framebuffer: %d by %d\n",w,h);
    return NULL;
  }

  compute_trans(&vbutTransf);

  volren = vcbNewVolRenderer(VCB_ABSEMI);

  vcbVolrOrtho(volren, hither, yon, xmin, xmax, ymin, ymax);

  vcbVolrLookAt(volren,coi[0],coi[1],coi[2], eye[0],eye[1],eye[2], up[0],up[1],up[2]);
  for (i = 0; i < numlights; i++)
    vcbVolrSetLight(volren, i, lightpos[i*3],lightpos[i*3+1],lightpos[i*3+2]);
  vcbVolrViewPort(volren, w, h);
  if  (datatype == VCB_UNSIGNEDBYTE)
    transf = vbutTransfuncub;
  else
    transf = vbutTransfuncf;
  
  vcbVolrTransFunc(volren, transf);
  vcbVolrFramebuffer(volren, VCB_MEMORY, VCB_UNSIGNEDBYTE, framebuffer);

  vcbVolrApplyTransformation(volren);
  vcbVolrRC(volren, datatype, voxels, 4, orig, dsize, scales, stepsz, imgbounds);
  vcbVolrCloseRC(volren);
  vcbFreeVolRenderer(volren);

  return framebuffer;
}

void * vbutrcastPersp(void * voxels, vcbdatatype datatype, 
		      int *orig, int * dsize,
		      float * eye, float * coi, float * up,
		      int numlights, float * lightpos,
		      int w, int h,
		      int * imgbounds,
		      float hither, float yon,	
		      float fov, float aspect,
			  float stepsz)
	
{
  VCB_volren volren;
  unsigned char * framebuffer;
  float scales[3] ={1.f, 1.f, 1.f};
  int i;

  VCB_TRANSFUNC transf;

  if ((w < 0) || (w > 4096))
    return NULL;
  if ((h < 0) || (h > 4096))
    return NULL;

  framebuffer = (unsigned char *) malloc (w*h*4*sizeof(unsigned char));
  if (framebuffer == NULL) {
    printf("cannot allocated framebuffer: %d by %d\n",w,h);
    return NULL;
  }

  compute_trans(&vbutTransf);

  volren = vcbNewVolRenderer(VCB_ABSEMI);

  vcbVolrPerspective(volren, fov, aspect, hither, yon);

  vcbVolrLookAt(volren,coi[0],coi[1],coi[2], eye[0],eye[1],eye[2], up[0],up[1],up[2]);
  for (i = 0; i < numlights; i++)
    vcbVolrSetLight(volren, i, lightpos[i*3],lightpos[i*3+1],lightpos[i*3+2]);
  vcbVolrViewPort(volren, w, h);
  if  (datatype == VCB_UNSIGNEDBYTE)
    transf = vbutTransfuncub;
  else
    transf = vbutTransfuncf;

  vcbVolrTransFunc(volren, transf);
  vcbVolrFramebuffer(volren, VCB_MEMORY, VCB_UNSIGNEDBYTE, framebuffer);

  vcbVolrApplyTransformation(volren);
  vcbVolrRC(volren, datatype, voxels, 4, orig, dsize, scales, stepsz, imgbounds);
  vcbVolrCloseRC(volren);
  vcbFreeVolRenderer(volren);

  return framebuffer;
}


int composite_region(unsigned char * fbuffer, 
		     int w, int h, int nchannels,
		     unsigned char * region, 
		     int lx, int ly, int ux, int uy)
{
  int  i, j, k, l, c, szx, szy;
  float ralpha,falpha;
  float rgba[4];
  unsigned char * fpixel, * rpixel;
  
  if ((fbuffer == NULL) || (region  == NULL)) {
    fprintf(stderr,"composite_region got NULL pointer\n");
    return -1;
  }

  if ((lx < 0) || (ly < 0) || (ux >= w) || (uy >= h)) {
    fprintf(stderr,"composite_region can't handle regions larger than framebuffer\n");
    return -2;
  }

  szx = ux - lx + 1;
  szy = uy - ly + 1;
  for (i = lx, k = 0; i <= ux; i ++, k++) {
    for (j = ly, l = 0; j <= uy; j ++, l ++) {
      fpixel = &fbuffer[(i*w   + j)* nchannels];
      rpixel = &region [(l*szx + k)* nchannels];

	  falpha = fpixel[nchannels-1]/255.f;
      ralpha = rpixel[nchannels-1]/255.f;

#if 0 /* FTB order */
      for (c = 0; c < nchannels-1; c++)
	      rgba[c] = (fpixel[c] +rpixel[c]*(1.f-falpha))/255.f;
	  rgba[c] = falpha + (1-falpha)*ralpha;
#else /* BTF order */
      for (c = 0; c < nchannels-1; c++)
	      rgba[c] = (rpixel[c] +fpixel[c]*(1.f-ralpha))/255.f;
#endif

	  for (c = 0; c < nchannels; c++)
	      fpixel[c] = (unsigned char) ((rgba[c] > 1.0)   ? 255 : (rgba[c] * 255.f+0.5));
    }
  }
  return 0;
}

typedef struct regionSort {
  int id;
  float dist;
} _region;

static int sortfunc (const void * x, const void * y)
{
  _region * p, * q;
  p = (_region *) x;
  q = (_region *) y;
  if (p->dist < q-> dist) return 1;
  if (p->dist > q-> dist) return -1;
  return 0;
}

int vcbCompositeRegions(float * eye, float * coi, 
			unsigned char * framebuffer, 
			int nchannels,
			int w, int h,
			int numVolumeImgs, vcbVolumeImg * bufs)
{
  _region * regions;
  float n[4];
  int i, j;
  vcbVolumeImg * vi;

  regions = (_region *) malloc (sizeof(_region) * numVolumeImgs);
  
  for (i = 0; i < 3; i ++)
    n[i] = coi[i] - eye[i];
	
  n[3] = -(n[0]*eye[0]+n[1]*eye[1]+n[2]*eye[2]); /* n[4]: (Ax+By+Cz+D==0)*/

  for (i = 0; i < numVolumeImgs; i ++) {
    regions[i].id = i;
    for (j = 0, regions[i].dist = 0; j < 3; j ++) /* used to start from 0 */
      regions[i].dist += bufs[i].centroid[j] * n[j];
  }
  qsort(regions, numVolumeImgs, sizeof(_region), sortfunc);

  for (i = 0; i < numVolumeImgs; i++) {
    /*printf("regions id [%d] = %d  ",i, regions[i].id);*/
	
    vi = &bufs[regions[i].id];
    if (vi->fb != NULL)
	/*printf(" centroid = %f,%f,%f\n", vi->centroid[0],vi->centroid[1],vi->centroid[2]);*/
	
	composite_region(framebuffer, w, h, nchannels,
		     vi->fb,
		     vi->origx, vi->origy,
		     vi->origx + vi->w - 1,
		     vi->origy + vi->h - 1);
  }

  free(regions);
  return 0;
}
