#include <stdlib.h>
#include <stdio.h>
#include "vcblinalg.h"
#include "binnerio.h"

/**
 * $Id$
 *
 */

int main(int argc, char ** argv)
{
	int nfacets, i, n;
	float mat[16], v0[4], v1[4], t[3];
	float vdata[1024], * v;
	
	t[0] = (float)atof(argv[1]);
	t[1] = (float)atof(argv[2]);
	t[2] = (float)atof(argv[3]);
	vcbTranslate3fv(mat, t);
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
