#ifndef _REBINAPP_
#define _REBINAPP_

/**
 * $Id$
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

char * rebinner_versionstring();

void	scale_vertices(int n, double * vdata,double xscale, double yscale, double zscale);
double	fv_bounds(double * askbounds, double * spacing, int * orig, int * xyzsize);
double	padded_bounds(double * bounds, int res, int * orig, int * xyzsize);

double	rebin_byslice(int npara,
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
					 char * outputpath);

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
			double emin, double emax);

double rebin_gmesh_output(
			int sliceid,
			int *	orig, 
			int *	xyzsize,
			double  cellsize, /* assume uniform cell size, hence cubic cells */
			double *spacing,
			double *voxels,
			double emin, 
			double emax,
			double threshold);

void	output_askinginfo(double * askbounds, int * xyzsize, double * spacing);
void	output_askinginfo_short(int * xyzsize);
void	output_actualinfo(double * bounds);
void	output_prerebininfo(int * orig, int * xyzsize, double * spacing, double cellsize);
void	output_postrebininfo(float rebintime, int npara, double totalvolume, int nvoxel);
void	output_gmesh_formaterr();

void	gmesh_singlebin_output(double * dp, 
							int sliceid, int x, int y, int z, 
							int * orig, double * spacing);
void	gmesh_singlebin_output_nd(double * dp, 
							int sliceid, int x, int y, int z, 
							int * orig, double * spacing);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
