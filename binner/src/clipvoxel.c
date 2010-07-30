/**
   \ingroup rebinner_tests
   \file src/clipvoxel.c
   
   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include "vcblinalg.h"
#include "binner.h"
#include "clip.h"
#include "binnerio.h"
#include "volume.h"


int main(int argc, char ** argv)
{
	double * vdata, * v, * voxels, ccs, voxel_volume;
	int    * nverts;        /* at most 200 facets */
	int      dim, i, j, k, nfacets, n, coord[3];

	/* 
	 * this code cuts the space of a 1.0 height, width, depth cube 
	 * to dim pieces in each direction
	 */
	dim = atoi(argv[1]);
	ccs = 1.0/dim;
	voxels = malloc(dim*dim*dim* sizeof(double));

	/* input has at most 1k vertices */
	vdata = malloc(1024 * 3 * sizeof(double)); 
	/* input has at most 200 facets */
	nverts = malloc(200 * sizeof(int));

	for (nfacets = 0, v = vdata; (n = get_polygond(v)) > 0; nfacets ++) {

		nverts[nfacets] = n;
		v += n * 3;
	}

	voxel_volume = 0;
	for (i = 0; i < dim; i ++)
		for (j = 0; j < dim; j ++)
			for (k = 0; k < dim; k ++)
			{
				coord[0] = i;
				coord[1] = j;
				coord[2] = k;

				voxels[(i*dim + j)*dim + k]  = partialvoxel_volume(nfacets, nverts, vdata, coord, ccs);
				
				voxel_volume += voxels[(i*dim + j)*dim + k];
				printf("%d %d %d: %lf\n", i, j, k, voxels[(i*dim + j)*dim + k]);
			}
	
	printf("total volume: %lf\n", voxel_volume);
	//nfacets = clip_polyhedral(nfacets, &nverts, &vdata, plane);
	//binnerout_phcd(nfacets, nverts, vdata);

	/*printf("%d \n", nfacets);*/
	free(nverts);
	free(vdata);
	free(voxels);

	return nfacets;
}
