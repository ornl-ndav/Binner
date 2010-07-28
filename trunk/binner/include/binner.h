/**
 * $Id$
 *
 * \file include/binner.h
 */

#ifndef _BINNER_
#define _BINNER_

#include <math.h>
 
/*! Rebinner truncates all numbers smaller than threshold to zero.*/
#ifndef BINNER_EPSILON
#define BINNER_EPSILON 1e-6
#endif

/*! A convenience macro to compute the norm of a vector \a n.*/
#define BINNER_NORM(n)	sqrt((n)[0]*(n)[0]+(n)[1]*(n)[1]+(n)[2]*(n)[2])

#ifdef __cplusplus
extern "C" {
#endif

void cal_normal(double * n, double * v);
double vec_normalize(double * n);
void bounding_box(int n, double * bound, double * v);
					
double para_volume(double * v);  /* volume of a paralleliped */

/**
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

/**
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

double bin_para_3dvoxelize(	int		nfacets, 
						int   * nverts,
						double *v, /* the vertices */
						int *	orig, 
						int *	xyzsize,
						double  ccs, /* cubic cell size */ 
						double *voxels);

/**
    This function return the total volume of the rebinned parallelipeds.
    There is no limit on how many parallelipeds can be processed together.
    
    \param[in]  v Pointer to vertices array describing the parallelipeds.
    \param[in]  hitcnt Pointer to hit counts array for the parallelipeds.
    \param[in]  hiterr Pointer to hit error metrics array for the parallelipeds.
    \param[out] voxels If this is a NULL pointer, the function dumps the
                      rebinned voxels directly through pipedump;
                  otherwise voxels is assumed to have enough memory space
                  to store xyzsize[0]*xyzsize[1]*xyzsize[2] double values.
*/

double bin_para_3dclip(
						int     sliceid,
						int		nfacets, 
						int   * nverts,
						double *v, /* the vertices */
						double *hitcnt,
						double *hiterr,
						int *	orig, 
						int *	xyzsize,
						double  ccs, /* cubic cell size, assume uniform cell size */ 
						double *voxels,
						double emin, double emax);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
