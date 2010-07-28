#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "vcblinalg.h"
#include "binner.h"
#include "cell.h"
#include "clip.h"

/**
 * $Id$
 *
 */

double tri_area(double *v)
{
	double a[3];
	/* compute the area of a triangle */
	cal_normal(a, v);

	return BINNER_NORM(a)/2.0;
}

void center_vertex(int n, double * v, double * ctr)
{
	/* always assume v contains all the vertices */
	/* 
	 * ctr: the center (xyz) of the paralleliped
	 *      assume 3 elements are already alloated 
	 */
	int i;

	assert(v != NULL);
	assert(ctr != NULL);

	for (i = 0; i < 3; ctr[i] = 0.0, i ++);

	for (i = 0; i < n*3; ctr[i%3] += v[i], i ++);

	for (i = 0; i < 3; ctr[i] /= n, i ++);

	/* now, ctr has the coordinate of the center */

}


double polygon_area(int n, double *v)
{
	/* 
	 * compute the area of a parallelogram affine 
	 * transformed from a 2D quad
	 *
	 * the function name: para_area might be confused to mean area of
	 * a paralleliped. hence, we use the name quad_area instead.
	 */
	int i;
	double area, tri2[9];
	
	/* assume v contains xyz coordinates of 4 vertices */
	assert(v != NULL);

	center_vertex(n, v, tri2); /* tri2[0-3]: center */

	/* now, let's build triangles */
	for (i = 0, area = 0.0; i < n-1; i ++) {
		memcpy(tri2+3, v+i*3, 6*sizeof(double));
		area += tri_area(tri2);
	}
	
	/* the last edge goes from the last vertex back to the first vertex */
	memcpy(tri2+3, v+i*3, 3*sizeof(double));
	memcpy(tri2+6, v, 3*sizeof(double));
	area += tri_area(tri2);

	return area;
}


double pyramid_volume(int nv, double * apex, double * v)
{
	/* 
	 * assume there are n vertices (xyz) in v 
	 * apex: the apex of the pyramid
	 * v: form the base polygon, so (nv) edges/vertices
	 */	
	double v1[3], n[3];
	
	double base_area, height, norm, volume;
	
	base_area = polygon_area(nv, v);
	
	cal_normal(n, v);
	/*
	v1[0] =  v[6] - v[3];
	v1[1] =  v[7] - v[4];
	v1[2] =  v[8] - v[5];

	v2[0] =  v[9] - v[3];
	v2[1] = v[10] - v[4];
	v2[2] = v[11] - v[5];

	VCB_CROSS(n, v1, v2);
	*/
	v1[0] =  apex[0] - v[0];
	v1[1] =  apex[1] - v[1];
	v1[2] =  apex[2] - v[2];

	/* norm = BINNER_NORM(n); */
	norm = vec_normalize(n); /* returns norm before normalization */

	/*
	if (norm < 1e-8) {
		n[0] = n[1] = n[2] = 0.0;
	}
	else {
		n[0] /= norm;
		n[1] /= norm;
		n[2] /= norm;
	}
	*/

	height = fabs(VCB_DOT(v1, n));
	volume = height * base_area /3.0;

	return volume;
}

double polyhedral_volume(int nfacets, int * nverts, double * v)
{
	int totalnverts;
	double ctr[3], *f;
	double volume;    /* the volume of the paralleliped */
	int i;

	/* always assume v contains all the vertices for all the facets */
	assert(v != NULL);

	for (i = 0, totalnverts = 0; i < nfacets; i ++)
		totalnverts += nverts[i];

	/* xyz of the apex for all the pyramids is the ctr of the polyhedron */
	center_vertex(totalnverts, v, ctr);
	
	/*
	 * after knowing the center, let us build nfacets number of pyramids. 
	 * each with one of the facets as the base. 
	 */
	for (i = 0, volume = 0.0, f = v; i < nfacets; i ++) {
		/*memcpy(&pyram[3], &v[i*12], sizeof(double)*12);*/
		volume += pyramid_volume(nverts[i], ctr, f);
		f += nverts[i]*3;
	}

	return volume;
}

double partialvoxel_volume(	int nwf, /* number of working facets - clipping planes */
							int * nv, /* number of vertices on each working face */
							double * wf, /* vertices on working facets */
							int * worig,
							double ccs)
{
	int	     nfacets;
	int    * nverts;
	double * cubev; /*[6*4*3];*/
	double * plane_eq, *p;
	double * w, volume;

	int i, j;

	/* first prepare the basic geometry of a cube */
	nfacets = 6;
	nverts = malloc(nfacets*sizeof(int));
	for (i = 0; i < nfacets; nverts[i] = 4, i ++);
	cubev = malloc(6*4*3*sizeof(double));
	basicCubed(cubev);
	plane_eq = malloc(nwf * 4 *sizeof(double));

	/*printf("%lf ", ccs);*/
	
	/* move and scale the cube to conform to the voxel in question */
	for (i = 0; i < 6 * 4; i ++)
		for (j = 0; j < 3; j ++) 
			cubev[i*3+j] = (cubev[i*3+j] + worig[j])*ccs;

	/* A,B,C,D in plane normal as in Ax+By+Cz+D = 0 */
	for (i = 0, p = plane_eq, w = wf; i < nwf; p+=4, i ++) {
		cal_normal(p, w);
		vec_normalize(p);
		p[3] = -VCB_DOT(w, p); 
		w += nv[i] * 3;
	}

	/*printf("%e %e %e on %d faces ", cubev[3], cubev[4], cubev[5], nfacets);*/

	/* start clipping by each clipping plane */
	for (i = 0, nfacets = 6, p = plane_eq; ((nfacets > 0) && (i < nwf)); p+= 4, i ++)
	{
		//printf("clipping polyheral, clip plane %d of %d, still %d faces left in cube\n", i, nwf, nfacets);
		nfacets = clip_polyhedral(nfacets, &nverts, &cubev, p);
	}

	/* now we have a polyhedral cell. compute its volume */
	if (nfacets > 0)
		volume = polyhedral_volume(nfacets, nverts, cubev);
	else
		volume = 0;

	//printf("partialvolume = %lf, nfacets = %d\n", volume, nfacets);

	free(plane_eq);
	if (nverts != NULL) free(nverts);
	if (cubev != NULL) free(cubev);

	return volume;
}


