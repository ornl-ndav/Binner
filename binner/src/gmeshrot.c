#include <stdlib.h>
#include <stdio.h>
#include "vcblinalg.h"
#include "binnerio.h"

int main(int argc, char ** argv)
{
	int nfacets, i, n, sliceid;
	float mat[16], v0[4], v1[4], axis[3], angle;
	float vdata[1024], * v;
	float emin, emax, hitcnt, hiterr, corners[8][4];
	
	axis[0] = (float)atof(argv[1]);
	axis[1] = (float)atof(argv[2]);
	axis[2] = (float)atof(argv[3]);
	angle   = (float)atof(argv[4]);
	vcbRotate3fv(mat, axis, angle);
	v0[3] = 1.f;

	/* v = malloc(nfacets * 4 * 3 * sizeof(float));*/

	for (nfacets = 0; (n = get_pixel_energy(&sliceid,&emin,&emax,&hitcnt,&hiterr,corners)) > 0; nfacets ++) {

		printf("%d %f %f %f %f", sliceid, emin, emax, hitcnt, hiterr);

		for (i = 0; i < 8; i ++) 
		{
			corners[i][3] = 1.f;
			vcbMatMult4fv(v1, mat, corners[i]);

			/*printvertexf(v1, 3);*/
			printf(" %f %f %f", v1[0], v1[1], v1[2]);
		}
		
		printf("\n");
	}

	/*printf("%d \n", nfacets);*/
	return nfacets;
}
