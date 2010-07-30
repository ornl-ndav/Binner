/**
   \ingroup rebinner_tests
   \file src/scale3d.c
   
   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include "vcblinalg.h"
#include "binnerio.h"


int main(int argc, char ** argv)
{
	int nfacets, i, n;
	float mat[16], v0[4], v1[4], s[3];
	float vdata[1024], * v;
	
	s[0] = (float)atof(argv[1]);
	s[1] = (float)atof(argv[2]);
	s[2] = (float)atof(argv[3]);
	vcbScale3fv(mat, s);
	v0[3] = 1.f;

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
