#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "vcblinalg.h"
#include "vcbutils.h"
#include "binnerio.h"
#include "binner.h"
#include "volume.h"

int main(int argc, char ** argv)
{
	clock_t time1, time2;

	int nfacets, i, n;
	double * vdata, * v;
	int    * nverts;        /* at most 200 facets */
	double totalvolume, cellsize;
	
	double bounds[6], * voxels;
	int    nvoxel;
	int    orig[3], xyzsize[3];
	int    res;
	int    f;

	res = atoi(argv[1]);
	f = atoi(argv[2]);
 
	/* input has at most 1k vertices */
	vdata = malloc(f * 4 * 3 * sizeof(double));
	/* input has at most 200 facets */
	nverts = malloc(f * sizeof(int));

	for (nfacets = 0, v = vdata; (n = get_polygond(v)) > 0; nfacets ++) {

		nverts[nfacets] = n;
		/*for (i = 0; i < n; i ++)
			printf("%lf %lf %lf ", v[i*3], v[i*3+1], v[i*3+2]);
		printf("\n");
		*/
		v += n * 3;
	}

time1 = clock();

	bounding_box((v-vdata)/3, bounds, vdata);	
	cellsize = (bounds[1] - bounds[0])/res;

	orig[0] = (int)(bounds[0]/cellsize);
	orig[1] = (int)(bounds[2]/cellsize);
	orig[2] = (int)(bounds[4]/cellsize);
	xyzsize[0] = (int)(bounds[1]/cellsize) - orig[0] + 1;
	xyzsize[1] = (int)(bounds[3]/cellsize) - orig[1] + 1;
	xyzsize[2] = (int)(bounds[5]/cellsize) - orig[2] + 1;
	//printf("orig: %d %d %d, volume size: %d %d %d\n", orig[0], orig[1], orig[2], xyzsize[0], xyzsize[1], xyzsize[2]);

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	voxels = malloc(nvoxel * sizeof(double));
	for (i = 0; i < nvoxel; voxels[i] = 0.0, i ++);

	totalvolume = bin_para3d_150(nfacets, 
							nverts,
							vdata, /* the vertices */
							orig, 
							xyzsize,
							cellsize, 
							voxels);

time2 = clock();
	printf("total time: %.3f sec\n",(float)(time2-time1)/CLOCKS_PER_SEC);
  
	//printf("rebin completed. totalvolume = %lf \n", totalvolume);
	vcbGenBinm("300.bin", VCB_DOUBLE, 3, orig, xyzsize, 1, voxels);

	free(voxels);
	free(nverts);
	free(vdata);
	
	return 0;
}
