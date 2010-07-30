/**
   \ingroup rebinner_tests
   \file src/clip3d.c
   
   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include "vcblinalg.h"
#include "binner.h"
#include "clip.h"
#include "binnerio.h"


int main(int argc, char ** argv)
{
	int nfacets, n;
	double * vdata, * v, norm;
	double plane[4]; 
	int    * nverts;        /* at most 200 facets */
	
	plane[0] = (double)atof(argv[1]);
	plane[1] = (double)atof(argv[2]);
	plane[2] = (double)atof(argv[3]);
	plane[3] = (double)atof(argv[4]);

	/* normalize the normal. adjust d accordingly */
	norm = vec_normalize(plane);
	plane[3] /= norm;

	/* input has at most 1k vertices */
	vdata = malloc(1024 * 3 * sizeof(double)); 
	/* input has at most 200 facets */
	nverts = malloc(200 * sizeof(int));

	for (nfacets = 0, v = vdata; (n = get_polygond(v)) > 0; nfacets ++) {

		nverts[nfacets] = n;
		v += n * 3;
	}
	
	nfacets = clip_polyhedral(nfacets, &nverts, &vdata, plane);

	binnerout_phcd(nfacets, nverts, vdata);

	/*printf("%d \n", nfacets);*/
	return nfacets;
}
