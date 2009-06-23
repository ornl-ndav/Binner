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
	double emin, emax, hitcnt, hiterr, corners[8][4];
	float  rebintime = 0;
	int   nfields, inputformat, pixelcnt = 0, c = 0;
	double inputv[4 + 8*3];

	if ((argc != 10) && (argc != 12))
	{
		fprintf(stderr, 
				"usage: %s [-b batchsize] xmin xmax xspacing ymin ymax yspacing zmin zmax zspacing\n",
				argv[0]);
		exit(1);
	}

	fprintf(stderr, "rebinner version    : %s\n", rebinner_versionstring());

	/* f: number of pixels to rebin together, default to 10000 */
	f = 10000;
	if (argc == 12)
	{
		if (strcmp(argv[c+1],"-b") == 0)
			f = atoi(argv[c+2]);
		else
		{
			fprintf(stderr, 
				"usage: %s [-b batchsize] xmin xmax xspacing ymin ymax yspacing zmin zmax zspacing\n",
				argv[0]);
			exit(1);
		}
		
		c += 2;
	}

	fprintf(stderr, "rebinner batch size : %d pixels\n", f);
	
	askbounds[0] = atof(argv[c+1]);
	askbounds[1] = atof(argv[c+2]);
	spacing[0]   = atof(argv[c+3]);
	askbounds[2] = atof(argv[c+4]);
	askbounds[3] = atof(argv[c+5]);
	spacing[1]   = atof(argv[c+6]);
	askbounds[4] = atof(argv[c+7]);
	askbounds[5] = atof(argv[c+8]);
	spacing[2]   = atof(argv[c+9]);
	
	for (j = 0; j < 3; j++)
		xyzsize[j] = (int)ceil((askbounds[j*2+1] - askbounds[j*2])/spacing[j]);

	output_askinginfo(askbounds, xyzsize, spacing);

	for (j = 0; j < 3; j++)
		spacing[j] = (askbounds[j*2+1] - askbounds[j*2])/xyzsize[j];

	cellsize = fv_bounds(askbounds, spacing, orig, xyzsize);

	output_prerebininfo(orig, xyzsize, spacing, cellsize);

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	voxels = malloc(nvoxel * sizeof(double) * 2); /* rebin both counts and error */
	for (i = 0; i < nvoxel*2; voxels[i] = 0.0, i ++);

	vdata  = malloc(f * 6 * 4 * 3 * sizeof(double));
	nverts = malloc(f * 6 * sizeof(int));
	hcnt   = malloc(f * sizeof(double));
	herr   = malloc(f * sizeof(double));
	sid    = malloc(f * sizeof(int));
	
	while (1)
	{
		/* read at most f pixels in each pass */
		for (npara = 0; npara < f; npara ++) 
		{
			if (fread(&sliceid, sizeof(int), 1, stdin) <= 0)
				break; /* did not read in a correct pixel */

			fread(inputv, sizeof(double), 4 + 8 *3, stdin);

			realCube3d(inputv + 4, &vdata[(npara*6*4)*3]);
		
			for (i = 0; i < 6; i ++)
				nverts[npara*6+i] = 4;

			hcnt[npara] = inputv[2];//hitcnt;
			herr[npara] = inputv[3];//hiterr;
			sid[npara] = sliceid;

#if REBINDEBUG
			fprintf(stderr,
				"sid, emin, emax, hcnt, herr: %d %f %f %lf %lf\n", 
				sid[npara], inputv[0], inputv[1], hcnt[npara], herr[npara]);
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

		pixelcnt += npara;

		time1 = clock();

		rebin_gmesh(npara,
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

	totalvolume = rebin_gmesh_output(sid[0], orig, xyzsize, cellsize, spacing, voxels, emin, emax);

	output_postrebininfo(rebintime, pixelcnt, totalvolume, nvoxel);

	free(sid);
	free(herr);
	free(hcnt);
	free(voxels);
	free(nverts);
	free(vdata);
	
	return 0;
}
