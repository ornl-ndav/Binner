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

char   rbuf[8192];
float  vbuf[1024];

int main(int argc, char ** argv)
{
	clock_t time1, time2;

	int i, j, n, f, npara, sliceid, res, nvoxel, orig[3], xyzsize[3];
	double * vdata, * hcnt, * herr, spacing[3];
	int    * nverts, * sid;
	double totalvolume, cellsize, bounds[6], askbounds[6]; 
	double * voxels;
	float emin, emax;
	float  hitcnt, hiterr, corners[8][4];
	float  rebintime = 0;
	int   nfields, inputformat;

	/* read first line. 
	 * must be in the following formats:
	 *   FV: xmin xmax num_binx; ymin ymax num_biny; zmin zmax num_binz 
	 * or
	 *   IV: num_binx; num_biny; num_binz
	 */

	if (argc != 1)
		fprintf(stderr, "usage: %s < gmesh_input > rebin_output");

	fprintf(stderr, "%s rebinner %s\n", argv[0], rebinner_versionstring());
	
	i = getchar();
	if (i == 'F')
	{  
		j = getchar();
		if (j != 'V') 
		{
			output_gmesh_formaterr();
			exit(1);
		}
		
		fgets(rbuf, 8192, stdin); /* get everything on that first line */

		nfields = sscanf(rbuf,
						"%lf %lf %d; %lf %lf %d; %lf %lf %d",
						&askbounds[0],  &askbounds[1],  &xyzsize[0],
						&askbounds[2],  &askbounds[3],  &xyzsize[1],
						&askbounds[4],  &askbounds[5],  &xyzsize[2]);

		if (nfields != 9) 
		{
			output_gmesh_formaterr();
			exit(1);
		}

		for (j = 0; j < 3; j++)
			spacing[j] = (askbounds[j*2+1] - askbounds[j*2])/xyzsize[j];
		
		output_askinginfo(askbounds, xyzsize, spacing);
		inputformat = 1;

	}
	else if (i == 'I')
	{
		j = getchar();
		if (j != 'V') 
		{
			output_gmesh_formaterr();
			exit(2);
		}
		
		fgets(rbuf, 8192, stdin); /* get everything on that first line */

		sscanf(rbuf, "%d", &res);
		
		if (nfields != 1) 
		{
			output_gmesh_formaterr();
			exit(3);
		}

		xyzsize[0] = xyzsize[1] = xyzsize[2] = res;

		askbounds[0] = askbounds[2] = askbounds[4] = -1e16;
		askbounds[1] = askbounds[3] = askbounds[5] = 1e16;

		output_askinginfo_short(xyzsize);
		inputformat = 2;
	}
	else 
	{
		output_gmesh_formaterr();
		exit(4);
	}

	/* f: number of pixels. each pixel corresponds to one paralleliped */
	scanf("%d", &f);

	vdata  = malloc(f * 6 * 4 * 3 * sizeof(double));
	nverts = malloc(f * 6 * sizeof(int));
	hcnt   = malloc(f * sizeof(double));
	herr   = malloc(f * sizeof(double));
	sid    = malloc(f * sizeof(int));
	
    for (npara = 0; (n = get_pixel_energy(&sliceid,&emin,&emax,&hitcnt,&hiterr,corners)) > 0; npara ++) 
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
		fprintf(stderr,
				"sid, emin, emax, hcnt, herr: %d %f %f %lf %lf\n", 
				sid[npara], emin, emax, hcnt[npara], herr[npara]);
#endif
	}

	bounding_box(npara*6*4, bounds, vdata);	
	output_actualinfo(bounds);

	if (inputformat == 1) 
	{
		cellsize = fv_bounds(askbounds, spacing, orig, xyzsize);
	
		scale_vertices( npara * 6 * 4, 
						vdata,
						cellsize/spacing[0], 
						cellsize/spacing[1],
						cellsize/spacing[2]);
	}
	else
		cellsize = padded_bounds(bounds, res, orig, xyzsize); 
	
	output_prerebininfo(orig, xyzsize, cellsize);

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	voxels = malloc(nvoxel * sizeof(double) * 2); /* rebin both counts and error */

	time1 = clock();

	totalvolume = rebin_gmesh(npara,
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
								emin, emax);
	
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
