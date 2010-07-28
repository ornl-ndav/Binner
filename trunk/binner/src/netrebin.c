#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "vcblinalg.h"
#include "vcbutils.h"
#include "binnerio.h"
#include "binner.h"
#include "volume.h"

/**
 * $Id$
 *
 */


int main(int argc, char ** argv)
{
	int nfacets, i, j, k, n, vid;
	double * vdata, * v;
	int    * nverts;        /* at most 200 facets */
	double totalvolume, cellsize;
	
	double bounds[6], * voxels;
	int    nvoxel;
	int    orig[3], xyzsize[3];
	int    f, toread;
	
	static char netbuf[1000000];
	char * cp;

	cellsize = atof(argv[1]);
	f = 150*6; /* the internal buffer holds at most 150 parallelipeds */
 
	/* input has at most 1k vertices */
	vdata = malloc(f * 4 * 3 * sizeof(double));
	/* input has at most 200 facets */
	nverts = malloc(f * sizeof(int));
	voxels = NULL;

	/* read and write the 8 bytes header without looking */
	//n = fread(netbuf, 2, sizeof(int), stdin);
	read(0, netbuf, 2*sizeof(int));
	//fwrite(netbuf, 2, sizeof(int), stdout);
	write(1,netbuf, 2*sizeof(int));
	
	while (1)
	{
		//n = fread(netbuf, 1, f * (sizeof(int)+4*3*sizeof(double)), stdin);
		toread = f * (sizeof(int)+4*3*sizeof(double));
		for (cp = netbuf, n = 0; (i = read(0, cp, toread)) > 0; toread -=i, cp+=i)
			n += i;
		
		/*fprintf(stderr,"after fread n = %d\n", n);*/

		if (n <= 0) break; /* got nothing. so done */
	
		/* now let's see if we can find up to 150 parallelipeds to work with */
		for (nfacets = 0, v = vdata, cp = netbuf; (nfacets < f/6) && n > 0; nfacets ++) {
			i = *((int *) cp); 
			nverts[nfacets] = i;
			cp += 4;
			n -= 4;
	
			memcpy(v, cp, i * 3 * sizeof(double));
			cp += i * 3 * sizeof(double);
			n -= i * 3 * sizeof(double);
			v += i * 3;
		}
		
		if (nfacets <= 0) break; /* got nothing workable. so done */
		fprintf(stderr, "netrebin got %d facets\n", nfacets);
	
		bounding_box((v-vdata)/3, bounds, vdata);	

		orig[0] = (int)(bounds[0]/cellsize);
		orig[1] = (int)(bounds[2]/cellsize);
		orig[2] = (int)(bounds[4]/cellsize);
		xyzsize[0] = (int)(bounds[1]/cellsize+0.5) - orig[0];
		xyzsize[1] = (int)(bounds[3]/cellsize+0.5) - orig[1];
		xyzsize[2] = (int)(bounds[5]/cellsize+0.5) - orig[2];
		fprintf(stderr,"orig: %d %d %d, xyzsize: %d %d %d\n", 
		        orig[0], orig[1], orig[2],
				xyzsize[0], xyzsize[1], xyzsize[2]);

		nvoxel = xyzsize[0]*xyzsize[1]*xyzsize[2];
		voxels = malloc(nvoxel * sizeof(double));
		for (i = 0; i < nvoxel; voxels[i] = 0.0, i ++);

		totalvolume = bin_para_3dvoxelize(nfacets, 
							nverts,
							vdata, /* the vertices */
							orig, 
							xyzsize,
							cellsize, 
							voxels);

		for (i = 0, n = 0; i < nvoxel; i ++)
			if (voxels[i] > 0) n ++;
		/*
		fwrite(&n, 1, sizeof(int), stdout);
		fwrite(orig, 3, sizeof(int), stdout);
		fwrite(xyzsize, 3, sizeof(int), stdout);
		*/
		cp = netbuf;
		memcpy(cp, &n, sizeof(int)); cp+= sizeof(int);
		memcpy(cp, orig, 3*sizeof(int)); cp+=3*sizeof(int);
		memcpy(cp, xyzsize, 3*sizeof(int)); cp+=3*sizeof(int);
		write(1, netbuf, cp-netbuf);

		n = 0;
		for (i = 0; i < xyzsize[0]; i ++)
			for (j = 0; j < xyzsize[1]; j ++)
				for (k = 0; k < xyzsize[2]; k ++) 
				{
					vid = (i*xyzsize[1] + j)*xyzsize[2] + k;
					if (voxels[vid] > 0) 
					{
					/*
						fwrite(&i, 1, sizeof(int), stdout);
						fwrite(&j, 1, sizeof(int), stdout);
						fwrite(&k, 1, sizeof(int), stdout);
						fwrite(&voxels[vid], 1, sizeof(double), stdout);
					*/
						cp = netbuf;
						memcpy(cp, &i, sizeof(int)); cp+= sizeof(int);
						memcpy(cp, &j, sizeof(int)); cp+= sizeof(int);
						memcpy(cp, &k, sizeof(int)); cp+= sizeof(int);
						memcpy(cp, &voxels[vid], sizeof(double)); cp+=sizeof(double);
						write(1, netbuf, cp-netbuf);
					}
					n ++;
				}
	}

	if (voxels != NULL) free(voxels);
	if (nverts != NULL) free(nverts);
	if (vdata != NULL) free(vdata);
	
	return 0;
}
