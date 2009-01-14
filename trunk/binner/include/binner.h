#ifndef _BINNER_
#define _BINNER_

#include <math.h>

#ifndef BINNER_EPSILON
#define BINNER_EPSILON 1E-8
#endif

#define BINNER_NORM(n)	sqrt((n)[0]*(n)[0]+(n)[1]*(n)[1]+(n)[2]*(n)[2])

#ifdef __cplusplus
extern "C" {
#endif

void cal_normal(double * n, double * v);
double vec_normalize(double * n);
void bounding_box(int n, double * bound, double * v);
					
double para_volume(double * v);  /* volume of a paralleliped */

/*
 * edge + corner voxels 
 * return value: subdivision factor in the final discrete volume 
 *               for achieving the etorl.
 * all voxels are assumed to be uniformly sized in all dimensions
 *
 * iterate until meeting this criterion: 
 *    etol[0]: the maximum allowed per boundary voxel error tolerance,
 *          as a percentage of the voxel's space
 *    etol[1-3]: the final achieved max/min/avg etol
 *    double etol[4]: must be pre-allocated 
 *  
 */
int esti_res3d(	int		nfacets, 
				double *v,
				double	volume, 
				double *xyzunit, 
				double *tol_relativeE);

/*
 * upon voxelization, since we need a multi-level resolutioned representation
 * the volume is not held in a single array
 *
 * instead, we use a red-black tree to hold a pointer to all the smaller volume
 * blocks. each block can be of a different resolution, going from a different
 * origin (and hence be of different sizes)
 */ 
int bin_quad3d(	int		nfacets, 
				double *v, 
				double	volume,
				int *	orig, 
				int *	xyzsize,
				double *xyzunit, 
				int		maxres, 
				double *voxel);

double bin_para3d_150(	int		nfacets, 
						int   * nverts,
						double *v, /* the vertices */
						int *	orig, 
						int *	xyzsize,
						double  ccs, /* cubic cell size */ 
						double *voxels);

double bin_smallpara3d_150(	int		nfacets, 
						int   * nverts,
						double *v, /* the vertices */
						double *hitcnt,
						double *hiterr,
						int *	orig, 
						int *	xyzsize,
						double  ccs, /* cubic cell size, assume uniform cell size */ 
						double *voxels);

void output_with_compression(char * fname,
							int * orig,
							int * sz,
							double * vol);

void export_VTK_volume(char * fname, int * orig, int * sz, double cellsize, double * vol);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif