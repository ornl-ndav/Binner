#ifndef _VBUT_RCAST_H 
#define _VBUT_RCAST_H


#define MAXNUMISORANGE 8
#define TRANSIZE       256

typedef struct transfunc_s {

  int colorScheme;

  int   numIsoRange;
  float minval, range;
  float posIsoRange[MAXNUMISORANGE];
  float sizeIsoRange[MAXNUMISORANGE];
  float opaIsoRange[MAXNUMISORANGE];
  float rgbalut[TRANSIZE*4];
} onedTransf;

typedef struct composite_regions {
  unsigned char * fb;
  float centroid[3];
  int origx, origy;
  int w, h;
} vcbVolumeImg;


/* start function declarations */

#ifdef __cplusplus
extern "C" {
#endif

int composite_region(unsigned char * fbuffer, 
		     int w, int h, int nchannels,
		     unsigned char * region, 
		     int lx, int ly, int ux, int uy);

int vcbCompositeRegions(float * eye, float * coi, 
			unsigned char * framebuffer, 
			int nchannels,
			int w, int h,
			int numVolumeImgs, vcbVolumeImg * bufs);

/*
 * vbut: Visualization cookBook Utility Toolkit
 * vbutDefineTransfunc
 *   this function defines tranfer functions in more conventional
 *   fashion, specifically, using interval ranges
 *
 * Arguments:
 * int colorscheme: the color scheme, i.e. color map, of RGB
 *                  channels can be chosen as:
 *                   0 - Full Color, full color range in HSV space
 *                   1 - Cold to Hot, i.e. blue to red
 *                   2 - BlackWhite, i.e. black to white
 *                   3 - Full Color, colorscheme 0 inverted
 *
 * int numIso:      number of interval ranges
 * float minval:    all data values < minval will be culled
 * float range:     all data values > minval+range will be culled
 * float * ipos:    position of triangle in value space for each
 *                  interval range
 * float * isize:   size of triangle in value space for each
 *                  interval range
 * float * iopa:    opacity of apex (triangle) for each interval
 *                  range
 *
 * return value:    void
 */
void vbutDefineTransfunc(int colorscheme, 
			 int numIso, 
			 float minval, float range, 
			 float * ipos, float * isize, float * iopa);

/*
 * vbut:  Visualization cookBook Utility Toolkit
 * vbutrcastPersp
 *   this function provides a one-call convenience to get
 *   a voluem rendered perspectively with a transfer 
 *   function already specified using:
 *           vbutDefineTransfunc.
 *
 * Arguments:
 *   void * voxels: pointer to the first byte of the volume
 *   vcbdatatype datatype: type of each voxel value
 *   int  * orig:   origin of the volume to render
 *   int  * dsize:  size of the  volume
 *   float * eye:   position of eye
 *   float * coi:   position of center of interest
 *   float * up:    direction of the up vector
 *   int   numlights: number of light sources
 *   float * lightpos: a sequence of coordinates of light sources
 *   int   w,h:     width, height of the viewport
 *   int * imgbounds: output, describing the size of the
 *                  footprint of the  rendering
 *                  (xmin, ymin, xmax, ymax)
 *   float hither,yon: near and far Z
 *   float fov:     field of view in Y direction
 *   float aspect:  aspect ratio (Y/X)
 *   float stepsz:  step size used in ray traversal
 *
 * Return Value:
 *   internally allocated framebuffer is returned as void *.
 *   on failure, NULL is returned.
 */
void * vbutrcastPersp(void * voxels, vcbdatatype datatype, 
		      int *orig, int * dsize,
		      float * eye, float * coi, float * up,
		      int numlights, float * lightpos,
		      int w, int h,
		      int * imgbounds,
		      float hither, float yon,	
		      float fov, float aspect,
			  float stepsz);

/*
 * vbut:  Visualization cookBook Utility Toolkit
 * vbutrcastOrtho
 *   this function provides a one-call convenience to get
 *   a voluem rendered orthogonally with a transfer 
 *   function already specified using:
 *           vbutDefineTransfunc.
 *
 * Arguments:
 *   void * voxels: pointer to the first byte of the volume
 *   vcbdatatype datatype: type of each voxel value
 *   int  * orig:   origin of the volume to render
 *   int  * dsize:  size of the  volume
 *   float * eye:   position of eye
 *   float * coi:   position of center of interest
 *   float * up:    direction of the up vector
 *   int   numlights: number of light sources
 *   float * lightpos: a sequence of coordinates of light sources
 *   int   w,h:     width, height of the viewport
 *   int * imgbounds: output, describing the size of the
 *                  footprint of the  rendering
 *                  (xmin, ymin, xmax, ymax)
 *   float hither,yon: near and far Z
 *   float xmin, xmax: left and right of frustum
 *   float ymin, ymax: bottom and top of frustum
 *   float stepsz: step size used in ray traversal
 *
 * Return Value:
 *   internally allocated framebuffer is returned as void *.
 *   on failure, NULL is returned.
 */
void * vbutrcastOrtho(void * voxels, vcbdatatype datatype, 
		      int *orig, int * dsize,
		      float * eye, float * coi, float * up,
		      int numlights, float * lightpos,
		      int w, int h,
		      int * imgbounds,
		      float hither, float yon,
		      float xmin, float xmax, 
		      float ymin, float ymax,
			  float stepsz);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
