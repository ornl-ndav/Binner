#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "binnerio.h"

void printvertexf(float *v, int ndim)
{
	int i;
	assert(ndim <= 4);
	for (i = 0; i < ndim; printf("%f ",v[i]), i ++);
}

void printvertexd(double *v, int ndim)
{
	int i;
	assert(ndim <= 4);
	for (i = 0; i < ndim; printf("%f ",(float)(v[i])), i ++);
}

int getquada3d(float * vdata)
/* 
 * return value: num_vertices read in successfully
 *               -1 on failure
 * getquad_a_: 'a' for ascii
 */
{
	int i;
	
	/* assume a quad in 3d space. 4 vertices, x-y-z coord each */	
	for (i = 0; i < 4; scanf("\n"), vdata += 3, i ++)
		if (3 > scanf("%f %f %f", &vdata[0], &vdata[1], &vdata[2]))
			break;
	
	return i;
}

int binnerin3d(int nfacets, float *vdata)
{
	int i;
	for (i = 0; i < nfacets; i ++)
		getquada3d(&vdata[i*4*3]);

	return nfacets;
}

void binnerout3d(int nfacets, float *v)
{
	int i, j;

	for (i = 0; i < nfacets; i ++) {
		for (j = 0; j < 4; j ++)
			printvertexf(&v[i*4*3+j*3], 3); 
		printf("\n");
	}
}

int get_polygonf(float * v)
/* 
 * return value: num_vertices actually read in successfully
 */
{
	int i, n;
	
	if (scanf("%d", &n) <= 0) return -1; 

	/* assume a quad in 3d space. 4 vertices, x-y-z coord each */	
	for (i = 0; i < n; v += 3, i ++)
		if (3 > scanf("%f %f %f", v, v+1, v+2))
			break;
			
	scanf("\n");

	return i;
}

int get_pixelf(int * id, float * cnt, float * err, float (* v)[4])
/* 
 * always read in 8 vertices and 3 preceding values: sliceID, cnt, err
 * return value: num_vertices actually read in successfully
 */
{
	int i, n;
	
	if (scanf("%d", id) <= 0) return -1; 
	if (scanf("%f", cnt) <= 0) return -1; 
	if (scanf("%f", err) <= 0) return -1; 

	/* assume a 8 vertices, x-y-z coord each */	
	for (i = 0; i < 8; v ++, i ++)
	{
		if (3 > scanf("%f %f %f", &v[0][0], &v[0][1], &v[0][2]))
			break;
		v[0][3] = 0.f;
	}
			
	scanf("\n");

	return i;
}

int binnerin_phcf(int nfacets, int * nverts, float * v)
{
	int i, total;

	total = 0;
	for (i = 0; i < nfacets; v += nverts[i]*3, i ++) {
		nverts[i] = get_polygonf(v);
		total += nverts[i];
	}

	return total;
}


int get_polygond(double * v)
/* 
 * return value: num_vertices actually read in successfully
 */
{
	int i, n;
	
	scanf("%d", &n);

	/* assume a quad in 3d space. 4 vertices, x-y-z coord each */	
	for (i = 0; i < n; v += 3, i ++)
		if (3 > scanf("%lf %lf %lf", v, v+1, v+2))
			break;
			
	scanf("\n");

	return i;
}

int binnerin_phcd(int nfacets, int * nverts, double * v)
{
	int i, total;

	total = 0;
	for (i = 0; i < nfacets; v += nverts[i]*3, i ++) {
		nverts[i] = get_polygond(v);
		total += nverts[i];
	}

	return total;
}

void binnerout_phcf(int nfacets, int * nverts, float *v)
{
	int i, j;

	for (i = 0; i < nfacets; i ++) {

		printf("%d ", nverts[i]);

		for (j = 0; j < nverts[i]; j ++)
		{
			printvertexf(v, 3); 
			v += 3;
		}
		printf("\n");
	}
}

void binnerout_phcd(int nfacets, int * nverts, double *v)
{
	int i, j;

	for (i = 0; i < nfacets; i ++) {

		printf("%d ", nverts[i]);

		for (j = 0; j < nverts[i]; j ++)
		{
			printvertexd(v, 3); 
			v += 3;
		}
		printf("\n");
	}
}

