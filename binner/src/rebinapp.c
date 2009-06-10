#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "vcblinalg.h"
#include "binner.h"
#include "binnerio.h"

#define REBINDEBUG 0

static char * versionstring = "1.0.0";

char * rebinner_versionstring()
{
	return versionstring;
}

void scale_vertices(int n, double * vdata,double xscale, double yscale, double zscale)
{
	int i;
	float mat[16], v0[4], v1[4], s[3];
	double *v;

	s[0] = (float) xscale;
	s[1] = (float) yscale;
	s[2] = (float) zscale;
	vcbScale3fv(mat, s);
	v0[3] = 1.f;

	for (i = 0, v = vdata; i < n; v+= 3, i ++)
	{
		v0[0] = (float)v[0];
		v0[1] = (float)v[1];
		v0[2] = (float)v[2];
		vcbMatMult4fv(v1, mat, v0);
		v[0]  = v1[0];
		v[1]  = v1[1];
		v[2]  = v1[2];
	}

}

double fv_bounds(double * askbounds, double * spacing, int * orig, int * xyzsize)
{

	/* return value: double cellsize */
	/* inputs: askbounds[3], spacing[3] */
	/* outputs: orig[3], xyzsize[3] */
	
	double cellsize;

	orig[0] = (int)floor(askbounds[0]/spacing[0]);
	orig[1] = (int)floor(askbounds[2]/spacing[1]);
	orig[2] = (int)floor(askbounds[4]/spacing[2]);
	xyzsize[0] = (int)ceil(askbounds[1]/spacing[0]) - orig[0]; /* + 1 */
	xyzsize[1] = (int)ceil(askbounds[3]/spacing[1]) - orig[1]; /* + 1 */
	xyzsize[2] = (int)ceil(askbounds[5]/spacing[2]) - orig[2]; /* + 1 */

	/* let's figure out the actual binning cell size */
	cellsize = spacing[0];
	if (spacing[1] > cellsize) cellsize = spacing[1];
	if (spacing[2] > cellsize) cellsize = spacing[2];
	
	return cellsize;
}

double padded_bounds(double * bounds, int res, int * orig, int * xyzsize)
{
	double cellsize, tmp;
	
	cellsize = (bounds[1] - bounds[0])/res;
	tmp = (bounds[3] - bounds[2])/res;
	if (tmp > cellsize) cellsize = tmp;
	tmp = (bounds[5] - bounds[4])/res;
	if (tmp > cellsize) cellsize = tmp;
			
	orig[0] = (int)floor(bounds[0]/cellsize);
	orig[1] = (int)floor(bounds[2]/cellsize);
	orig[2] = (int)floor(bounds[4]/cellsize);
	xyzsize[0] = (int)ceil(bounds[1]/cellsize) - orig[0]+1;
	xyzsize[1] = (int)ceil(bounds[3]/cellsize) - orig[1]+1;
	xyzsize[2] = (int)ceil(bounds[5]/cellsize) - orig[2]+1;	
	
	return cellsize;
}

double rebin_byslice(int npara, 			
					int   * nverts,
			double *vdata, /* the vertices */
			int    *sliceid,
			double *hitcnt,
			double *hiterr,
			int *	orig, 
			int *	xyzsize,
			double  cellsize, /* assume uniform cell size, hence cubic cells */
			double *spacing,
			double *voxels,
			char * outputpath)
{
	int i, j, n, nonempty, nvoxel;
	double totalvolume = 0.0, volumescale;
	char fullname[200];

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	volumescale = 	(cellsize/spacing[0]) * (cellsize/spacing[1]) * (cellsize/spacing[2]);
	volumescale =  1.0/volumescale;
	
	for (n = 0, j = 0, nonempty = 0; n < npara; n += j)
	{
	
		for (i = n; i < npara; i++) /* sliceid monotonically increase */
			if (sliceid[i] != sliceid[n])
				break;

		j = i - n; /* now we know this many para are on the same slice */

#if REBINDEBUG
		fprintf(stderr, "before rebin: n = %d, npara = %d, slice %d has %d paras\n", n, npara, sliceid[n], j);
#endif

		for (i = 0; i < nvoxel*2; voxels[i] = 0.0, i ++);

		totalvolume += bin_smallpara3d_150(j * 6, /* npara*6 */ 
								nverts + n*6,
								vdata + n*6*4*3, /* the vertices */
								hitcnt + n,        /* hit counter */
								hiterr + n,        /* hit counter error */
								orig, 
								xyzsize,
								cellsize, 
								voxels);

		for (i = 0; i < nvoxel; voxels[i*2] *= volumescale, i ++);   /* counts */
		for (i = 0; i < nvoxel; voxels[i*2+1] *= volumescale, i ++); /* error */
		totalvolume *= volumescale;

		sprintf(fullname, "%s/%04d", outputpath, sliceid[n]);
		nonempty += output_with_compression(fullname, xyzsize, voxels);

		fprintf(stderr, 
		        "rebinned slice %04d : %.2f%% nonempty, %d parallelipeds\n", 
				sliceid[n], (100.f*nonempty)/nvoxel, j);

		/* vcbGenBinm("500.bin", VCB_DOUBLE, 3, orig, xyzsize, 2, voxels); */
#if REBINDEBUG
		printf("rebin_slice recorded totalvolume = %lf, before scaling\n", totalvolume);
		for (i = 0; i < nvoxel; i ++)
			printf("voxels[%d] = %lf error[%d] = %lf\n", i, voxels[i*2], i, voxels[i*2+1]);
#endif
	}
	
	export_VTKhdr(outputpath, orig, xyzsize, spacing, nonempty); 

	return totalvolume;
}

void printcorners(int x, int y, int z, int * orig, double * spacing)
{
	x = x - orig[0];
	y = y - orig[1];
	z = z - orig[2];

	printf("%f %f %f ", x*spacing[0], y*spacing[1], z*spacing[2]);
}

double rebin_gmesh(int npara, 			
					int   * nverts,
			double *vdata, /* the vertices */
			int    *sliceid,
			double *hitcnt,
			double *hiterr,
			int *	orig, 
			int *	xyzsize,
			double  cellsize, /* assume uniform cell size, hence cubic cells */
			double *spacing,
			double *voxels,
			double emin, double emax)
{
	int i, j, n, nonempty, nvoxel;
	double totalvolume = 0.0, volumescale;

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	volumescale = 	(cellsize/spacing[0]) * (cellsize/spacing[1]) * (cellsize/spacing[2]);
	volumescale =  1.0/volumescale;
	
	for (n = 0, j = 0, nonempty = 0; n < npara; n += j)
	{
		for (i = n; i < npara; i++) /* sliceid monotonically increase */
			if (sliceid[i] != sliceid[n])
				break;

		j = i - n; /* now we know this many para are on the same slice */

#if REBINDEBUG
		fprintf(stderr, "before rebin: n = %d, npara = %d, slice %d has %d paras\n", n, npara, sliceid[n], j);
#endif

		totalvolume += bin_smallpara3d_150(j * 6, /* npara*6 */ 
								nverts + n*6,
								vdata + n*6*4*3, /* the vertices */
								hitcnt + n,        /* hit counter */
								hiterr + n,        /* hit counter error */
								orig, 
								xyzsize,
								cellsize, 
								voxels);
	}
	
	return totalvolume;
}

double rebin_gmesh_output(
			int sliceid,
			int *	orig, 
			int *	xyzsize,
			double  cellsize, /* assume uniform cell size, hence cubic cells */
			double *spacing,
			double *voxels,
			double emin, double emax)
{
	int i, nonempty, nvoxel, x, y, z;
	double totalvolume = 0.0, volumescale;

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	volumescale = 	(cellsize/spacing[0]) * (cellsize/spacing[1]) * (cellsize/spacing[2]);
	volumescale =  1.0/volumescale;

	for (i = 0; i < nvoxel; voxels[i*2] *= volumescale, i ++);   /* counts */
	for (i = 0; i < nvoxel; voxels[i*2+1] *= volumescale, i ++); /* error */

	for (i = 0, x = 0; x < xyzsize[0]; x ++)
		for (y = 0; y < xyzsize[1]; y ++)
			for (z = 0; z < xyzsize[2]; z ++)
			{
				printf("%d %f %f ", sliceid, emin, emax);
				printf("%le %le ", voxels[i*2], voxels[i*2+1]);
				i ++;
				printcorners(x, y, z, orig, spacing);
				printcorners(x+1, y, z, orig, spacing);
				printcorners(x, y+1, z, orig, spacing);
				printcorners(x+1, y+1, z, orig, spacing);
				printcorners(x, y, z+1, orig, spacing);
				printcorners(x+1, y, z+1, orig, spacing);
				printcorners(x, y+1, z+1, orig, spacing);
				printcorners(x+1, y+1, z+1, orig, spacing);
				printf("\n");
			}

	for (i = 0, nonempty = 0; i < nvoxel; i ++)
		if (voxels[i*2] > 1e-16)
		{
			nonempty ++;
			totalvolume += voxels[i*2];
		}
	
	fprintf(stderr, 
			"rebinned volume     : %.2f%% occupied\n", 
			(100.f*nonempty)/nvoxel);

#if REBINDEBUG
		fprintf(stderr, "rebin_slice recorded totalvolume = %lf, after scaling\n", totalvolume);
		for (i = 0; i < nvoxel; i ++)
			fprintf(stderr, "voxels[%d] = %lf error[%d] = %lf\n", i, voxels[i*2], i, voxels[i*2+1]);
#endif

	return totalvolume;

}


void output_askinginfo(double * askbounds, int * xyzsize, double * spacing)
{
	fprintf(stderr, "asking x domain     : %.3lf %.3lf\n", askbounds[0], askbounds[1]);
	fprintf(stderr, "asking y domain     : %.3lf %.3lf\n", askbounds[2], askbounds[3]);
	fprintf(stderr, "asking z domain     : %.3lf %.3lf\n", askbounds[4], askbounds[5]);
	fprintf(stderr, "asking dimension    : %d %d %d\n", xyzsize[0], xyzsize[1], xyzsize[2]);
	fprintf(stderr, "asking bin size     : %e %e %e\n", spacing[0], spacing[1], spacing[2]);
}

void output_askinginfo_short(int * xyzsize)
{
	fprintf(stderr, "asking x domain     : unspecified\n");
	fprintf(stderr, "asking y domain     : unspecified\n");
	fprintf(stderr, "asking z domain     : unspecified\n");
	fprintf(stderr, "asking dimension    : %d %d %d\n", xyzsize[0], xyzsize[1], xyzsize[2]);
	fprintf(stderr, "asking bin size     : unspecified\n");
}

void output_actualinfo(double * bounds)
{
	fprintf(stderr, "actual x domain     : %.3lf %.3lf\n", bounds[0], bounds[1]);
	fprintf(stderr, "actual y domain     : %.3lf %.3lf\n", bounds[2], bounds[3]);
	fprintf(stderr, "actual z domain     : %.3lf %.3lf\n", bounds[4], bounds[5]);
}

void output_prerebininfo(int * orig, int * xyzsize, double cellsize)
{
 	fprintf(stderr, "rebin volume origin : %d %d %d\n", orig[0], orig[1], orig[2]);
	fprintf(stderr, "rebin volume size   : %d %d %d\n", xyzsize[0], xyzsize[1], xyzsize[2]);
	fprintf(stderr, "rebin cell size     : %e\n", cellsize);
}

void output_postrebininfo(float rebintime, int npara, double totalvolume, int nvoxel)
{
	fprintf(stderr, "rebin time          : %.3f sec\n", rebintime);
	fprintf(stderr, "rebin throughput    : %.2f per second, %d total\n", npara/rebintime, npara);
	fprintf(stderr, "recorded total cnt  : %le\n", totalvolume);
	fprintf(stderr, "all bins            : %d\n", nvoxel);
}

void output_gmesh_formaterr()
{
	fprintf(stderr,"gmeshrebin error:\n"); 
	fprintf(stderr,"wrong input format on stdin. need either of on line 1:\n");
	fprintf(stderr,"FV: xmin xmax num_binx; ymin ymax num_biny; zmin zmax num_binz \n");
	fprintf(stderr,"IV: num_bins \n");
	fprintf(stderr,"line 2: number_of_pixels by itself \n");
	fprintf(stderr,"line 3 and on: sid emin emax hits err [x y z] for all 8 corners \n");
}