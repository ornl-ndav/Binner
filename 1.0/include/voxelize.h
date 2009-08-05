#ifndef _BINNERVOXELIZE_
#define _BINNERVOXELIZE_


#ifdef __cplusplus
extern "C" {
#endif

double dist_plane(double * normal, double * pnt, double * vert0);

void voxelize_poly(	int n, double * v, /* the polygon */
					unsigned int * wvol, /* working volume, contain tags when done */
					int * orig,
					int * xyzsize,
					double cs,
					int base, /* base is the max val of polyid */
					int polyid); /* cell size */


#ifdef __cplusplus
}  /* extern C */
#endif

#endif
