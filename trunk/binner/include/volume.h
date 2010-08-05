/**
   \ingroup rebinner_core
   
   \file include/volume.h

   \brief CURRENT core API -- volume calculation function: partialvoxel_volume

   $Id$

 */


#ifndef _BINNERVOLUME_
#define _BINNERVOLUME_


#ifdef __cplusplus
extern "C" {
#endif

double tri_area(double *v);
void center_vertex(int n, double * v, double * ctr);
double polygon_area(int n, double *v);
double pyramid_volume(int n, double * apex, double * v);
double polyhedral_volume(int nfacets, int * nverts, double * v);

double partialvoxel_volume(	int nwf, /* number of working facets - clipping planes */
							int * nv, /* number of vertices on each working face */
							double * wf, /* vertices on working facets */
							int * worig,
							double cellsize);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
