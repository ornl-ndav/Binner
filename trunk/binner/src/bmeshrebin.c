#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include "vcblinalg.h"
#include "vcbutils.h"
#include "rebinapp.h"
#include "binnerio.h"
#include "binner.h"
#include "cell.h"
#include "volume.h"

#define REBINDEBUG 0

char   md5string[100], fname[200], rbuf[8192];
float  vbuf[1024];

int main(int argc, char ** argv)
{
	clock_t time1, time2;

	int i, j, n, f, npara, sliceid, res, nvoxel, orig[3], xyzsize[3];
	double * vdata, * hcnt, * herr, spacing[3];
	int    * nverts, * sid;
	double totalvolume, cellsize, bounds[6], askbounds[6]; 
	double * voxels;
	float  hitcnt, hiterr, corners[8][4];
	float  rebintime = 0;
	int   FV = 0; /* 0 - FV unspecified; 1 - FV specified */

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
	   fgets(md5string, 100, stdin); get whatever that is left on this line 
	   */
	   sprintf(fname, "mkdir %s\n", md5string);
	   system(fname);
	   sprintf(fname, "%s",md5string);
	}
	else
	   fname[0] = 0;

	/* f = atoi(argv[2]); */
	scanf("%d", &f);
	fgets(md5string, 100, stdin); /* get whatever that is left on this line */

	fprintf(stderr, "rebin data name     : %s\n", fname);

	vdata  = malloc(f * 6 * 4 * 3 * sizeof(double));
	nverts = malloc(f * 6 * sizeof(int));
	hcnt   = malloc(f * sizeof(double));
	herr   = malloc(f * sizeof(double));
	sid    = malloc(f * sizeof(int));

	i = getchar();
	ungetc(i, stdin);
	if (i == 'F')
	{  
	   /* ignore FV field so far */
	   j = getchar();
	   j = getchar();
	   if (j != 'V') 
		  fprintf(stderr, "wrong input format on line #3: FV ... FV field\n");

	   fgets(rbuf, 8192, stdin); /* get everything on that first line */
	   
	   if (argc < 2)
		   if (strlen(rbuf) > 20) /* not empty between the beginning and ending FV field */
		   {
			  sscanf(rbuf,
					 "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %d %d %d",
					 &vdata[0],  &vdata[1],  &vdata[2], 
					 &vdata[3],  &vdata[4],  &vdata[5],
					 &vdata[6],  &vdata[7],  &vdata[8], 
					 &vdata[9],  &vdata[10], &vdata[11],
					 &vdata[12], &vdata[13], &vdata[14], 
					 &vdata[15], &vdata[16], &vdata[17],
					 &vdata[18], &vdata[19], &vdata[20], 
					 &vdata[21], &vdata[22], &vdata[23],
					 &xyzsize[0],&xyzsize[1], &xyzsize[2]);	
			  /*
			  for (j = 0; j < 24; j ++)
				 fprintf(stderr,"%.2lf ", vdata[j]);
			  fprintf(stderr,"\n");
			  for (j = 0; j < 3; j ++)
				 fprintf(stderr,"%d ", xyzsize[j]);
			  fprintf(stderr,"\n");
			  */
			  bounding_box(8, askbounds, vdata);
			  for (j = 0; j < 3; j++)
				  spacing[j] = (askbounds[j*2+1] - askbounds[j*2])/xyzsize[j];

			  output_askinginfo(askbounds, xyzsize, spacing);
			  
			  FV = 1;
		   }
	}
	
    for (npara = 0; (n = get_pixelf(&sliceid,&hitcnt,&hiterr,corners)) > 0; npara ++) 
	{
		if (npara >= f) break; /* read at most f faces, i.e. f/6 parallelipeds */

		if (hitcnt >= 1e-16)
		{
			/* only ones with real hitcnt should be fixed */
			/* otherwise, just keep the space filled would be fine */
			correctCornersf3d(corners);
		}

		realCubef(corners, vbuf);

		for (i = 0; i < 6*4; i ++)
		{
			vdata[(npara*6*4+i)*3+0] = vbuf[i*4+0];
			vdata[(npara*6*4+i)*3+1] = vbuf[i*4+1];
			vdata[(npara*6*4+i)*3+2] = vbuf[i*4+2];
		}
		
		for (i = 0; i < 6; i ++)
			nverts[npara*6+i] = 4;

		hcnt[npara] = hitcnt;
		herr[npara] = hiterr;
		sid[npara] = sliceid;

#if REBINDEBUG
		fprintf(stderr,"sid, hcnt, herr: %d %lf %lf\n", sid[npara], hcnt[npara], herr[npara]);
#endif
	}

	bounding_box(npara*6*4, bounds, vdata);	
	output_actualinfo(bounds);

	if (FV == 1) 
	{
		cellsize = fv_bounds(askbounds, spacing, orig, xyzsize);
	
		scale_vertices( npara * 6 * 4, 
						vdata,
						cellsize/spacing[0], 
						cellsize/spacing[1],
						cellsize/spacing[2]);
	}
	else
	{
		res = 100; /* this is the default resolution */
		if (argc > 1) 
		   res = atoi(argv[1]); 

		cellsize = padded_bounds(bounds, res, orig, xyzsize); 
    }
	
	output_prerebininfo(orig, xyzsize, spacing, cellsize);

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	voxels = malloc(nvoxel * sizeof(double) * 2); /* rebin both counts and error */

	time1 = clock();

	totalvolume = rebin_byslice(npara,
								nverts,
								vdata, /* the vertices */
								sid,
								hcnt,        /* hit counter */
								herr,        /* hit counter error */
								orig, 
								xyzsize,
								cellsize, 
								spacing,
								voxels,
								fname);
	
	time2 = clock();
	rebintime += (float)(time2-time1)/CLOCKS_PER_SEC;
	
	output_postrebininfo(rebintime, npara, totalvolume, nvoxel);

	free(sid);
	free(herr);
	free(hcnt);
	free(voxels);
	free(nverts);
	free(vdata);
	
	return 0;
}
