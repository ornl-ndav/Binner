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
#include <unistd.h>

#define REBINDEBUG 0

char * usage = "usage: %s [-f] [-b batchsize] [-t threshold] xmin xmax xspacing ymin ymax yspacing zmin zmax zspacing\n";

int main(int argc, char ** argv)
{
	clock_t time1, time2, time3, time4;

	int    i, j, f, npara, sliceid, nvoxel, orig[3], xyzsize[3];
	double * vdata, * hcnt, * herr, spacing[3];
	int    * nverts, * sid;
	double totalvolume = 0., cellsize, bounds[6], askbounds[6]; 
	double * voxels;
	double emin, emax, threshold;
	float  rebintime = 0, outputtime = 0, inputtime = 0;
	int    pixelcnt = 0, c = 0, filtermode = 0;
	double inputv[4 + 8*3];

	pid_t pid = getpid();
	
	if ((argc < 10) || (argc > 15))
	{
		/* 
		 * [-f]: toggle of filter mode. if set, gemeshrebin3 acts as a pure filter, not a 
		 * full-fledged rebinner. the missing piece is implemented by gmeshsink, which serves
		 * to collect and generate a globally consistent rebinned volume.
		 */
		fprintf(stderr, usage, argv[0]);
		exit(1);
	}

	fprintf(stderr, "rebinner version    : %s\n", rebinner_versionstring());

	/* f: number of pixels to rebin together, default to 10000 */
	f = 10000;
	threshold = 1e-16;

	while (argc > 10)
	{
		if (strcmp(argv[c+1],"-f") == 0)
		{
			filtermode = 1;
			argc -= 1;
			c += 1;
		}
		else if (strcmp(argv[c+1],"-b") == 0)
		{
			f = (int)(atof(argv[c+2]));
			argc -= 2;
			c += 2;
		}
		else if (strcmp(argv[c+1],"-t") == 0)
		{
			threshold = atof(argv[c+2]);
			argc -= 2;
			c += 2;
		}
		else
		{
			fprintf(stderr, usage, argv[0]);
			exit(1);
		}		
	}
	

	fprintf(stderr, "rebinner batch size : %d pixels\n", f);
	fprintf(stderr, "rebinner threshold  : %le \n", threshold);
	
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
	if (filtermode == 1)
		voxels = NULL; /* NULL tells rebin_gmesh to output voxel directly on stdout */
	else
	{
		voxels = malloc(nvoxel * sizeof(double) * 2); /* rebin both counts and error */
		for (i = 0; i < nvoxel*2; voxels[i] = 0.0, i ++);
	}

	vdata  = malloc(f * 6 * 4 * 3 * sizeof(double));
	nverts = malloc(f * 6 * sizeof(int));
	hcnt   = malloc(f * sizeof(double));
	herr   = malloc(f * sizeof(double));
	sid    = malloc(f * sizeof(int));
	
	while (1)
	{
		time3 = clock();

#if REBINDEBUG
		fprintf(stderr, "%d ready to take new inputs. lasttime did %d pixels\n", pid, npara);
#endif

		/* read at most f pixels in each pass */
		for (npara = 0; npara < f; npara ++) 
		{
			if (fread(&sliceid, sizeof(int), 1, stdin) <= 0)
			{
#if REBINDEBUG
				fprintf(stderr, "%d did not read correct sliceid\n", pid);
#endif
				break; /* did not read in a correct pixel */
			}

			if (fread(inputv, sizeof(double), 4 + 8 *3, stdin) < 28)
			{
#if REBINDEBUG
				fprintf(stderr, "%d did not read correct doubles\n", pid);
#endif
				break; /* did not read in a correct pixel */
			}

			realCube3d(inputv + 4, &vdata[(npara*6*4)*3]);
		
			for (i = 0; i < 6; i ++)
				nverts[npara*6+i] = 4;

			emin = inputv[0];
			emax = inputv[1];
			hcnt[npara] = inputv[2];//hitcnt;
			herr[npara] = inputv[3];//hiterr;
			sid[npara] = sliceid;

#if REBINDEBUG
			fprintf(stderr,
				"sid, emin, emax, hcnt, herr: %d %f %f %lf %lf\n", 
				sid[npara], inputv[0], inputv[1], hcnt[npara], herr[npara]);
#endif
		}

#if REBINDEBUG
		fprintf(stderr, "%d done reading inputs: %d pixels\n", pid, npara);
#endif

		if (npara <= 0) 
		{
			fprintf(stderr, "%d read no more pixels. quitting\n", pid);
			break; /* did not read in any pixels */
		}

/*
		bounding_box(npara*6*4, bounds, vdata);	
//#if REBINDEBUG		
		output_actualinfo(bounds);
//#endif
*/
		time4 = clock();
		inputtime += (float)(time4-time3)/CLOCKS_PER_SEC;

		time1 = clock();

#if REBINDEBUG
		fprintf(stderr, "%d begin scaling %d pixels\n", pid, npara);
#endif
	
		scale_vertices( npara * 6 * 4, 
						vdata,
						cellsize/spacing[0], 
						cellsize/spacing[1],
						cellsize/spacing[2]);

		pixelcnt += npara;

#if REBINDEBUG
		fprintf(stderr, "%d begin rebinning\n", pid);
#endif

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

#if REBINDEBUG
		fprintf(stderr, "%d completes rebinning %d pixels, %d total\n", pid, npara, pixelcnt);
#endif

		rebintime += (float)(time2-time1)/CLOCKS_PER_SEC;
		
		if (npara < f) 
			break; /* cannot read enough inputs to fullfill pipeline */
	
	}

#if REBINDEBUG
	fprintf(stderr, "%d finished rebinning %d pixels\n", pid, pixelcnt);
#endif

	if (filtermode == 0)
	{
		time3 = clock();
		totalvolume = rebin_gmesh_output(sid[0], orig, xyzsize, cellsize, spacing, voxels, emin, emax, threshold);
		time4 = clock();
		outputtime +=  (float)(time4-time3)/CLOCKS_PER_SEC;

		fprintf(stderr, "rebin input time    : %f sec\n", inputtime);
		fprintf(stderr, 
				"rebin input rate    : %.3f MB/sec\n", 
				pixelcnt*(sizeof(int)+sizeof(double)*(4 + 8 *3)/inputtime/1e6));

		fprintf(stderr, "rebin output time   : %f sec\n", outputtime);
		output_postrebininfo(rebintime, pixelcnt, totalvolume, nvoxel);
	}

	free(sid);
	free(herr);
	free(hcnt);
	if (filtermode == 0) free(voxels);

	free(nverts);
	free(vdata);
	
	return 0;
}
