/**
   \ingroup rebinner_tests
   \file src/rot3d.c

   \brief CURRENT test of core API -- binnerio.h and using vcb
   
   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include "vcblinalg.h"
#include "binnerio.h"


int main(int argc, char ** argv)
{
	int nfacets, i, n;
	float mat[16], v0[4], v1[4], axis[3], angle;
	float vdata[1024], * v;
	
	axis[0] = (float)atof(argv[1]);
	axis[1] = (float)atof(argv[2]);
	axis[2] = (float)atof(argv[3]);
	angle   = (float)atof(argv[4]);
	vcbRotate3fv(mat, axis, angle);
	v0[3] = 1.f;

	/* v = malloc(nfacets * 4 * 3 * sizeof(float));*/

	for (nfacets = 0; (n = get_polygonf(vdata)) > 0; nfacets ++) {

		printf("%d ", n);

		for (i = 0, v = vdata; i < n; v+= 3, i ++) {
			v0[0] = v[0];
			v0[1] = v[1];
			v0[2] = v[2];
			vcbMatMult4fv(v1, mat, v0);

			printvertexf(v1, 3);
		}
		
		printf("\n");
	}

	/*printf("%d \n", nfacets);*/
	return nfacets;
}
