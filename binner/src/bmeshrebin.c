#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "vcblinalg.h"
#include "vcbutils.h"
#include "binnerio.h"
#include "binner.h"
#include "cell.h"
#include "volume.h"

char   md5string[100], fname[200], fullname[200], rbuf[8192];
float  vbuf[1024];

int main(int argc, char ** argv)
{
	clock_t time1, time2;

	int i, j, n, f, npara, sliceid, res, nvoxel, orig[3], xyzsize[3];
	double * vdata, * hcnt, * herr, spacing[3];
	int    * nverts, * sid;
	double totalvolume, cellsize, tmp, bounds[6]; 
	double * voxels;
	float  hitcnt, hiterr, corners[8][4];
	float  rebintime = 0;

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
	   fgets(rbuf, 8192, stdin); /* get everything on that first line */
	   sscanf(rbuf, "%s", md5string);
	   /*
	   sprintf(fname, "%s.bin", md5string);
	   fgets(md5string, 100, stdin); /* get whatever that is left on this line 
	   */
	   sprintf(fname, "mkdir %s\n", md5string);
	   system(fname);
	   sprintf(fname, "%s",md5string);
	}
	else
		sprintf(fname, "");

	res = atoi(argv[1]);
	
	/* f = atoi(argv[2]); */
	scanf("%d", &f);
	fgets(md5string, 100, stdin); /* get whatever that is left on this line */

	i = getchar();
	ungetc(i, stdin);
	if (i == 'F')
	{  
	   /* ignore FV field so far */
	   fgets(rbuf, 8192, stdin); /* get everything on that first line */
	}
 
	vdata = malloc(f * 6 * 4 * 3 * sizeof(double));
	nverts = malloc(f * 6 * sizeof(int));
	hcnt   = malloc(f * sizeof(double));
	herr   = malloc(f * sizeof(double));
	sid    = malloc(f * sizeof(int));

	
    for (npara = 0; (n = get_pixelf(&sliceid,&hitcnt,&hiterr,corners)) > 0; npara ++) 
	{
		if (npara >= f) break; /* read at most f faces, i.e. f/6 parallelipeds */

		correctCornersf3d(corners);
		realCubef(corners, vbuf);
		hcnt[npara] = hitcnt;
		herr[npara] = hiterr;
		sid[npara] = sliceid;

		for (i = 0; i < 6*4; i ++)
		{
			vdata[(npara*6*4+i)*3+0] = vbuf[i*4+0];
			vdata[(npara*6*4+i)*3+1] = vbuf[i*4+1];
			vdata[(npara*6*4+i)*3+2] = vbuf[i*4+2];
		}
		
		for (i = 0; i < 6; i ++)
			nverts[npara*6+i] = 4;
		
	}

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
	printf("orig: %d %d %d, volume size: %d %d %d, cellsize %e\n", 
		   orig[0], orig[1], orig[2], 
		   xyzsize[0], xyzsize[1], xyzsize[2],
		   cellsize);

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	voxels = malloc(nvoxel * sizeof(double));

	spacing[0] = spacing[1] = spacing[2] = cellsize;
	export_VTKhdr(fname, orig, xyzsize, spacing);

	for (n = 0, j = 0; n < f; n += j)
	{
		time1 = clock();
		
		for (i = n; i < f; i++) /* sliceid monotonically increase */
			if (sid[i] != sid[n])
				break;

		j = i - n; /* now we know this many para are on the same slice */

		/* printf("n = %d, slice %d has %d paras\n", n, sid[n], j);*/

		for (i = 0; i < nvoxel; voxels[i] = 0.0, i ++);

		totalvolume = bin_smallpara3d_150(j * 6, /* npara*6 */ 
								nverts + n*6,
								vdata + n*6*4*3, /* the vertices */
								hcnt + n,        /* hit counter */
								herr + n,        /* hit counter error */
								orig, 
								xyzsize,
								cellsize, 
								voxels);

		time2 = clock();

		rebintime += (float)(time2-time1)/CLOCKS_PER_SEC;
	  
		sprintf(fullname, "%s/%0.4d", fname, sid[n]);
		printf("slice %d has %d parallelipeds\n", sid[n], j);

		/* vcbGenBinm("500.bin", VCB_DOUBLE, 3, orig, xyzsize, 1, voxels); */
		
		output_with_compression(fullname, xyzsize, voxels);
	}
	
	printf("%d parallelipeds in %.3f sec (%.2f per sec), total count %e\n", f, rebintime, f/rebintime, totalvolume);

	free(sid);
	free(herr);
	free(hcnt);
	free(voxels);
	free(nverts);
	free(vdata);
	
	return 0;
}
