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
	double totalvolume = 0., cellsize, bounds[6], askbounds[6]; 
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

	if (argc != 10) 
	{
		fprintf(stderr, 
				"usage: %s xmin xmax num_binx ymin ymax num_biny zmin zmax num_binz\n",
				argv[0]);
		exit(1);
	}

	fprintf(stderr, "rebinner version    : %s\n", rebinner_versionstring());
	
	askbounds[0] = atof(argv[1]);
	askbounds[1] = atof(argv[2]);
	xyzsize[0]   = atoi(argv[3]);
	askbounds[2] = atof(argv[4]);
	askbounds[3] = atof(argv[5]);
	xyzsize[1]   = atoi(argv[6]);
	askbounds[4] = atof(argv[7]);
	askbounds[5] = atof(argv[8]);
	xyzsize[2]   = atoi(argv[9]);
	
	for (j = 0; j < 3; j++)
		spacing[j] = (askbounds[j*2+1] - askbounds[j*2])/xyzsize[j];
		
	output_askinginfo(askbounds, xyzsize, spacing);
	cellsize = fv_bounds(askbounds, spacing, orig, xyzsize);

	output_prerebininfo(orig, xyzsize, cellsize);

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	voxels = malloc(nvoxel * sizeof(double) * 2); /* rebin both counts and error */
	for (i = 0; i < nvoxel*2; voxels[i] = 0.0, i ++);

	/* f: number of pixels to rebin together */
	f = 1000; 

	vdata  = malloc(f * 6 * 4 * 3 * sizeof(double));
	nverts = malloc(f * 6 * sizeof(int));
	hcnt   = malloc(f * sizeof(double));
	herr   = malloc(f * sizeof(double));
	sid    = malloc(f * sizeof(int));
	
	while (1)
	{
		for (npara = 0; npara < f; npara ++) 
		{
			/* read at most f pixels */
			n = get_pixel_energy(&sliceid,&emin,&emax,&hitcnt,&hiterr,corners);
			if (n <= 0) break; /* did not read in one pixel */
			
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

		if (npara <= 0) break; /* did not read in any pixels */

		bounding_box(npara*6*4, bounds, vdata);	
#if REBINDEBUG		
		output_actualinfo(bounds);
#endif
	
		scale_vertices( npara * 6 * 4, 
						vdata,
						cellsize/spacing[0], 
						cellsize/spacing[1],
						cellsize/spacing[2]);
	
		time1 = clock();

		totalvolume += rebin_gmesh(npara,
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
	
	}

	rebin_gmesh_output(sid[0], orig, xyzsize, cellsize, spacing, voxels, emin, emax);

	output_postrebininfo(rebintime, npara, totalvolume, nvoxel);

	free(sid);
	free(herr);
	free(hcnt);
	free(voxels);
	free(nverts);
	free(vdata);
	
	return 0;
}
