/**

   \ingroup rebinner_execs
   
   \file src/gmeshindex.c

   \brief CURRENT executable of rebinner for indexing bounds of many gmehs files.
  
   usage: gmeshindex all_files_to_index, for example:
   \code
   UNIX> gmeshindex 81[3-9]-mesh/*
   \endcode
   This command will index the bounds of all files under 813-mesh to 819-mesh.

   $Id$

 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "binner.h"


void update_bounds(int n, double * bound, double * v)
{
	double  fimin,fjmin,fkmin,fimax,fjmax,fkmax;
	int k;
	double * vp;

	fimin = bound[0];
	fimax = bound[1];
	fjmin = bound[2];
	fjmax = bound[3];
	fkmin = bound[4];
	fkmax = bound[5];

	for (k = 0, vp = v; k < n; vp += 3, k++)
	{
		if (vp[0]<fimin)  fimin=vp[0];
		if (vp[0]>fimax)  fimax=vp[0];
		if (vp[1]<fjmin)  fjmin=vp[1];
		if (vp[1]>fjmax)  fjmax=vp[1];
		if (vp[2]<fkmin)  fkmin=vp[2];
		if (vp[2]>fkmax)  fkmax=vp[2];
	}
	
	bound[0] = fimin;
	bound[1] = fimax;
	bound[2] = fjmin;
	bound[3] = fjmax;
	bound[4] = fkmin;
	bound[5] = fkmax;

}

int main(int argc, char ** argv)
{
	int i, n, buf[1];
	char PATH[1024];
	double vdata[4 + 8*3], *v;
	double bounds[6];
	FILE * fp;
	
	if (argc <= 1)
	{
		fprintf(stderr, "usage: %s all_files_to_index \n", argv[0]);
		exit(1);
	}

	v = vdata + 4;
	
	getcwd(PATH, 1024);
	/* fprintf(stderr, "path = %s\n", PATH); */

	for (i= 1; i < argc; i ++)
	{
		fp = fopen(argv[i], "rb");
		if (fp == NULL)
		{
			/* something is wrong with this file */
			/* move on to the next file */
			continue;
		}

		bounds[0] = bounds[2] = bounds[4] = 1e16;
		bounds[1] = bounds[3] = bounds[5] = -1e16;	

		for (n = 0; ; n ++) 
		{

			if (fread(buf, sizeof(int), 1, fp) <= 0)
				break;
			
			fread(vdata, sizeof(double), 4 + 8 *3, fp);

			update_bounds(8, bounds, v);

		}

		if (argv[i][0] != '/')
			fprintf(stdout, "%s/%s ", PATH, argv[i]);
		else
			fprintf(stdout, "%s ", argv[i]);
			
		fprintf(stdout, "%d %le %le %le %le %le %le\n", n, bounds[0], bounds[1], bounds[2], bounds[3], bounds[4], bounds[5]);

		fclose(fp);
	}

	return 0;
}
