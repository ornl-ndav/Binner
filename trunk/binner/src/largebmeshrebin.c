#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "vcblinalg.h"
#include "vcbutils.h"
#include "binnerio.h"
#include "binner.h"
#include "cell.h"
#include "volume.h"

/**
 * $Id$
 *
 */

char   md5string[100], fname[200], fullname[200], rbuf[8192];
float  vbuf[1024];

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

int main(int argc, char ** argv)
{
	clock_t time1, time2;

	int i, j, n, f, npara, sliceid, res, nvoxel, orig[3], xyzsize[3], nonempty;
	double * vdata, * hcnt, * herr, spacing[3];
	int    * nverts, * sid;
	double totalvolume, cellsize, tmp, askbounds[6]; 
	double * voxels;
	float  hitcnt, hiterr, corners[8][4];
	float  rebintime = 0;
	int   FV = 0; /* 0 - FV unspecified; 1 - FV specified */
	double volumescale;

	/* always assume FV is there */
	
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
	   
	   /* if (argc < 2) */
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

			  bounding_box(8, askbounds, vdata);
			  fprintf(stderr, "asking x domain     : %.3lf %.3lf\n", askbounds[0], askbounds[1]);
			  fprintf(stderr, "asking y domain     : %.3lf %.3lf\n", askbounds[2], askbounds[3]);
			  fprintf(stderr, "asking z domain     : %.3lf %.3lf\n", askbounds[4], askbounds[5]);
			  fprintf(stderr, "asking dimension    : %d %d %d\n", xyzsize[0], xyzsize[1], xyzsize[2]);
			  for (j = 0; j < 3; j++)
				  spacing[j] = (askbounds[j*2+1] - askbounds[j*2])/xyzsize[j];
			  fprintf(stderr, "asking bin size     : %e %e %e\n", spacing[0], spacing[1], spacing[2]);
			  
			  FV = 1;
		   }
	}
	
	if (FV != 1) 
	{
		fprintf(stderr,
		        "%s cannot find a valid FV field. aborting rebinning process ...\n", 
				argv[0]); 
		free(sid);
		free(herr);
		free(hcnt);
		free(voxels);
		free(nverts);
		free(vdata);
		
		exit(-1);
	}

	{
		res = 100; /* this is the default resolution */
		if (argc > 1) 
		   res = atoi(argv[1]); 

		cellsize = (askbounds[1] - askbounds[0])/res;
		tmp = (askbounds[3] - askbounds[2])/res;
		if (tmp > cellsize) cellsize = tmp;
		tmp = (askbounds[5] - askbounds[4])/res;
		if (tmp > cellsize) cellsize = tmp;
			
		orig[0] = (int)floor(askbounds[0]/cellsize);
		orig[1] = (int)floor(askbounds[2]/cellsize);
		orig[2] = (int)floor(askbounds[4]/cellsize);
		xyzsize[0] = (int)ceil(askbounds[1]/cellsize) - orig[0]+1;
		xyzsize[1] = (int)ceil(askbounds[3]/cellsize) - orig[1]+1;
		xyzsize[2] = (int)ceil(askbounds[5]/cellsize) - orig[2]+1;	

		spacing[0] = spacing[1] = spacing[2] = cellsize;
		volumescale = 1.0;
    }

 	fprintf(stderr, "rebin volume origin : %d %d %d\n", orig[0], orig[1], orig[2]);
	fprintf(stderr, "rebin volume size   : %d %d %d\n", xyzsize[0], xyzsize[1], xyzsize[2]);
	fprintf(stderr, "rebin bin size      : %e %e %e\n", cellsize, cellsize, cellsize);

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	voxels = malloc(nvoxel * sizeof(double));

	totalvolume = 0;

	while (1)
	{
		time1 = clock();

		for (npara = 0; (n = get_pixelf(&sliceid,&hitcnt,&hiterr,corners)) > 0; npara ++) 
		{
			if (npara >= f) break; /* read at most f faces, i.e. f/6 parallelipeds */

			correctCornersf3d(corners, NULL);
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
		
		if (npara <= 0) break; /* exit point: we are done with all slices */

		bounding_box(npara*6*4, askbounds, vdata);	
		fprintf(stderr, "slice %04d x domain : %.3lf %.3lf\n", sid[0], askbounds[0], askbounds[1]);
		fprintf(stderr, "slice %04d y domain : %.3lf %.3lf\n", sid[0], askbounds[2], askbounds[3]);
		fprintf(stderr, "slice %04d z domain : %.3lf %.3lf\n", sid[0], askbounds[4], askbounds[5]);
		fprintf(stderr, "slice %04d has      : %d paras\n", sid[0], npara);

		for (i = 0; i < nvoxel; voxels[i] = 0.0, i ++);

		totalvolume += bin_para_3dclip(sid[0], npara*6, 
								nverts,
								vdata, /* the vertices */
								hcnt,        /* hit counter */
								herr,        /* hit counter error */
								orig, 
								xyzsize,
								cellsize, 
								voxels, 0., 0.);

		time2 = clock();

		rebintime += (float)(time2-time1)/CLOCKS_PER_SEC;
	  
		for (i = 0; i < nvoxel; voxels[i] *= volumescale, i ++);

		sprintf(fullname, "%s/%04d", fname, sid[0]);		
		nonempty += output_with_compression(fullname, xyzsize, voxels);
	}
	
	totalvolume *= volumescale;

	export_VTKhdr(fname, orig, xyzsize, spacing, nonempty);
 
	fprintf(stderr, "rebin time          : %.3f sec\n", rebintime);
	fprintf(stderr, "rebin throughput    : %.2f per second\n", f/rebintime);
	fprintf(stderr, "recorded count sum  : %e\n", totalvolume);
	fprintf(stderr, "nonempty bins       : %d\n", nonempty);
	fprintf(stderr, "all bins            : %d\n", nvoxel);
	fprintf(stderr, "nonempty percentage : %.2f%%\n", (float)nonempty/nvoxel*100);

	free(sid);
	free(herr);
	free(hcnt);
	free(voxels);
	free(nverts);
	free(vdata);
	
	return 0;
}
