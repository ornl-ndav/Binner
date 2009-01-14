#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "vcblinalg.h"
#include "vcbutils.h"
#include "binnerio.h"
#include "binner.h"
#include "cell.h"
#include "volume.h"

int main(int argc, char ** argv)
{
	clock_t time1, time2;

	int i, n, f, npara, sliceid, res, nvoxel, orig[3], xyzsize[3];
	double * vdata, * hcnt, * herr;
	int    * nverts;
	double totalvolume, cellsize, tmp, bounds[6]; 
	double * voxels;
	float  hitcnt, hiterr, corners[8][4], vbuf[1024];
	char   md5string[100], fname[100];

/*
	for (nfacets = 0, v = vdata; (n = get_polygond(v)) > 0; nfacets ++) {

		nverts[nfacets] = n;
		v += n * 3;
	}
*/

	i = getchar();
	ungetc(i, stdin);
	if (i == 'M')
	{  
	   /*
	    * if an md5string is provided
		* all results will be saved to a MR-JH directory 
		*/
	   scanf("%s", md5string);
	   /*
	   sprintf(fname, "%s.bin", md5string);
	   fgets(md5string, 100, stdin); /* get whatever that is left on this line 
	   */
	   sprintf(fname, "mkdir %s\n", md5string);
	   system(fname);
	   sprintf(fname, "%s/",md5string);
	}
	else
		sprintf(fname, "");

	res = atoi(argv[1]);
	
	/* f = atoi(argv[2]); */
	scanf("%d", &f);
	fgets(md5string, 100, stdin); /* get whatever that is left on this line */
 
	vdata = malloc(f * 6 * 4 * 3 * sizeof(double));
	nverts = malloc(f * 6 * sizeof(int));
	hcnt   = malloc(f * sizeof(double));
	herr   = malloc(f * sizeof(double));

	
    for (npara = 0; (n = get_pixelf(&sliceid,&hitcnt,&hiterr,corners)) > 0; npara ++) 
	{
		if (npara >= f) break; /* read at most f faces, i.e. f/6 parallelipeds */

		correctCornersf3d(corners);
		realCubef(corners, vbuf);
		hcnt[npara] = hitcnt;
		herr[npara] = hiterr;
		for (i = 0; i < 6*4; i ++)
		{
			vdata[(npara*6*4+i)*3+0] = vbuf[i*4+0];
			vdata[(npara*6*4+i)*3+1] = vbuf[i*4+1];
			vdata[(npara*6*4+i)*3+2] = vbuf[i*4+2];
		}
		
		for (i = 0; i < 6; i ++)
			nverts[npara*6+i] = 4;
		
	}

time1 = clock();

	bounding_box(npara*6*4, bounds, vdata);	

	cellsize = (bounds[1] - bounds[0])/res;
	tmp = (bounds[3] - bounds[2])/res;
	if (tmp > cellsize) cellsize = tmp;
	tmp = (bounds[5] - bounds[4])/res;
	if (tmp > cellsize) cellsize = tmp;
		
	orig[0] = (int)floor(bounds[0]/cellsize);
	orig[1] = (int)floor(bounds[2]/cellsize);
	orig[2] = (int)floor(bounds[4]/cellsize);
	xyzsize[0] = (int)ceil(bounds[1]/cellsize) - orig[0] + 1;
	xyzsize[1] = (int)ceil(bounds[3]/cellsize) - orig[1] + 1;
	xyzsize[2] = (int)ceil(bounds[5]/cellsize) - orig[2] + 1;
	printf("orig: %d %d %d, volume size: %d %d %d\n", orig[0], orig[1], orig[2], xyzsize[0], xyzsize[1], xyzsize[2]);

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	voxels = malloc(nvoxel * sizeof(double));
	for (i = 0; i < nvoxel; voxels[i] = 0.0, i ++);

	totalvolume = bin_smallpara3d_150(npara*6, 
							nverts,
							vdata, /* the vertices */
							hcnt,  /* no hit counter */
							herr,  /* no hit error */
							orig, 
							xyzsize,
							cellsize, 
							voxels);

time2 = clock();
	printf("%d parallelipeds in %.3f sec, total count %e\n", f, (float)(time2-time1)/CLOCKS_PER_SEC, totalvolume);
  
	vcbGenBinm("500.bin", VCB_DOUBLE, 3, orig, xyzsize, 1, voxels);
	
	output_with_compression(fname, xyzsize, voxels);
	export_VTK_volume(fname, orig, xyzsize, cellsize, voxels);

	free(herr);
	free(hcnt);
	free(voxels);
	free(nverts);
	free(vdata);
	
	return 0;
}
