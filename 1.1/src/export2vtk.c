/**
   \ingroup rebinner_sdk

   \file src/export2vtk.c

   \brief CURRENT sdk executable to convert gmesh to vtk geometry format.

   $Id$
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "vcbutils.h"
#include "binnerio.h"
#include "binner.h"


int main(int argc, char ** argv)
{
	FILE * hdr;
	char fullname[256], dataname[256], idstring[10];
	double orig[3], spacing[3];
	int i, j, k, n, sz[3];
	
	double * dvol, * vol;
	unsigned short * coord;

	vcbdatatype rawdatatype; 
	int dims[3], ori[3], dsize[3], nattribs;

	printf("exporting from directory: %s\n", argv[1]);

	sprintf(fullname, "%s/run.hdr", argv[1]);	
	
	hdr = fopen(fullname, "r");
	
	fscanf(hdr, "REBINNED Qxyz HISTOGRAM\n%s\n", dataname);
	fscanf(hdr, "DIMENSIONS %d %d %d\n", &sz[0], &sz[1], &sz[2]);
	fscanf(hdr, "ORIGIN %lf %lf %lf\n", &orig[0], &orig[1], &orig[2]);
	fscanf(hdr, "SPACING %lf %lf %lf\n", &spacing[0], &spacing[1], &spacing[2]);	
	fclose(hdr);

	vol = malloc(sz[0]*sz[1]*sz[2]*sizeof(double));
	for (i = 0; i < sz[0]*sz[1]*sz[2]; vol[i] = 0.0, i ++);
	
	printf("sliceid to include: ");
	for ( ; fscanf(stdin, "%s", idstring) > 0; printf("sliceid to include (CNTL-D to end): "))	
	{
		sprintf(fullname, "%s/%s.bin.d", argv[1], idstring);	
		dvol = vcbReadBinm(fullname, &rawdatatype, dims, ori, dsize, &nattribs);
		if (dvol == NULL)
			continue;

		sprintf(fullname, "%s/%s.bin.us", argv[1], idstring);
		printf("opening %s\n", fullname);
		coord = vcbReadBinm(fullname, &rawdatatype, dims, ori, dsize, &nattribs);
		if (coord == NULL)
			continue;

		printf("loading slice %s under %s\n",idstring, argv[1]);

		for (n = 0; n < dsize[0]; n ++)
		{
			i = coord[n*3 + 0];
			j = coord[n*3 + 1];
			k = coord[n*3 + 2];
			vol[(i * sz[1] + j) * sz[2] +k] = dvol[n];
		}
		
		free(dvol);
		free(coord);
	}

	for (i = 0, j = 0; i < sz[0]*sz[1]*sz[2]; i ++)
		if (vol[i] > 1e-16) j ++;

	sprintf(fullname, "%s/combined", argv[1]);	
	export_VTKvol(fullname, orig, sz, spacing, vol);
	
	printf("\n\nVolume saved to      %s.vtk\n", fullname);
	printf("Domain size          %d %d %d\n", sz[0], sz[1], sz[2]);
	printf("Domain origin        %e %e %e\n", orig[0], orig[1], orig[2]);
	printf("Domain spacing       %e %e %e\n", spacing[0], spacing[1], spacing[2]);
	printf("Num non-empty voxels %d\n", j);
	printf("Num of all voxels    %d\n", i);
	printf("Non-empty percentage %.2f%%\n", (float)j/i*100);

	return 0;
}
