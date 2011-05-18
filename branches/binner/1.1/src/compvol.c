/**
   \ingroup rebinner_tests
   \file src/compvol.c

   \brief CURRENT test of core API -- binnerio.h, volume.h.
   
   $Id$

*/

#include <stdlib.h>
#include <stdio.h>
#include "vcblinalg.h"
#include "binnerio.h"
#include "volume.h"


int main(int argc, char ** argv)
{
	int nfacets, n;
	double * vdata, * v;
	int    * nverts;        /* at most 200 facets */
	double volume;

	/* double vdata[128];*/

	/* input has at most 1k vertices */
	vdata = malloc(1024 * 3 * sizeof(double));
	/* input has at most 200 facets */
	nverts = malloc(200 * sizeof(int));

	for (nfacets = 0, v = vdata; (n = get_polygond(v)) > 0; nfacets ++) {

		nverts[nfacets] = n;
		v += n * 3;
	}
	
	volume = polyhedral_volume(nfacets, nverts, vdata);

	printf("volume = %16.14lf \n", volume);

	return 0;
}
