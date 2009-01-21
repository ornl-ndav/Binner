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
	double totalvolume, cellsize, tmp, bounds[6], askbounds[6]; 
	double * voxels;
	float  hitcnt, hiterr, corners[8][4];
	float  rebintime = 0;
	int   FV = 0; /* 0 - FV unspecified; 1 - FV specified */
	double volumescale;

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
		
	}

	bounding_box(npara*6*4, bounds, vdata);	
	fprintf(stderr, "actual x domain     : %.3lf %.3lf\n", bounds[0], bounds[1]);
	fprintf(stderr, "actual y domain     : %.3lf %.3lf\n", bounds[2], bounds[3]);
	fprintf(stderr, "actual z domain     : %.3lf %.3lf\n", bounds[4], bounds[5]);

	if (FV == 1) 
	{
		orig[0] = (int)floor(askbounds[0]/spacing[0]);
		orig[1] = (int)floor(askbounds[2]/spacing[1]);
		orig[2] = (int)floor(askbounds[4]/spacing[2]);
		xyzsize[0] = (int)ceil(askbounds[1]/spacing[0]) - orig[0]+1;
		xyzsize[1] = (int)ceil(askbounds[3]/spacing[1]) - orig[1]+1;
		xyzsize[2] = (int)ceil(askbounds[5]/spacing[2]) - orig[2]+1;

		/* let's figure out the actual binning cell size */
		cellsize = spacing[0];
		if (spacing[1] > cellsize) cellsize = spacing[1];
		if (spacing[2] > cellsize) cellsize = spacing[2];

		scale_vertices( npara * 6 * 4, 
						vdata,
						cellsize/spacing[0], 
						cellsize/spacing[1],
						cellsize/spacing[2]);
						
		volumescale = 	(cellsize/spacing[0]) * (cellsize/spacing[1]) * (cellsize/spacing[2]);
		volumescale =  1.0/volumescale;
	}
	else
	{
		res = 100; /* this is the default resolution */
		if (argc > 1) 
		   res = atoi(argv[1]); 

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

		spacing[0] = spacing[1] = spacing[2] = cellsize;
		volumescale = 1.0;
    }

 	fprintf(stderr, "rebin volume origin : %d %d %d\n", orig[0], orig[1], orig[2]);
	fprintf(stderr, "rebin volume size   : %d %d %d\n", xyzsize[0], xyzsize[1], xyzsize[2]);
	fprintf(stderr, "rebin bin size      : %e %e %e\n", cellsize, cellsize, cellsize);

	nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
	voxels = malloc(nvoxel * sizeof(double));

	totalvolume = 0;

	for (n = 0, j = 0, nonempty = 0; n < f; n += j)
	{
		time1 = clock();
		
		for (i = n; i < f; i++) /* sliceid monotonically increase */
			if (sid[i] != sid[n])
				break;

		j = i - n; /* now we know this many para are on the same slice */

		/* printf("n = %d, slice %d has %d paras\n", n, sid[n], j);*/

		for (i = 0; i < nvoxel; voxels[i] = 0.0, i ++);

		totalvolume += bin_smallpara3d_150(j * 6, /* npara*6 */ 
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
	  
		sprintf(fullname, "%s/%04d", fname, sid[n]);
		fprintf(stderr, "slice %04d has      : %d parallelipeds\n", sid[n], j);

		/* vcbGenBinm("500.bin", VCB_DOUBLE, 3, orig, xyzsize, 1, voxels); */

		for (i = 0; i < nvoxel; voxels[i] *= volumescale, i ++);
		
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
