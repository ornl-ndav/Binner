#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "vcblinalg.h"

/**
 * $Id$
 *
 */

int main(int argc, char ** argv)
{
	int nfacets, i, n, sliceid;
	float mat[16], v0[4], v1[4], axis[3], angle;
	float vdata[1024];
	float emin, emax, hitcnt, hiterr;
	char * buf;
	int lastchar[50], c;
	
	axis[0] = (float)atof(argv[1]);
	axis[1] = (float)atof(argv[2]);
	axis[2] = (float)atof(argv[3]);
	angle   = -(float)atof(argv[4]);

	buf = malloc(1024*8);

	for (nfacets = 0; fgets(buf, 1024*8, stdin) != NULL; nfacets ++) 
    {
		for (i = 0, c = 0; buf[i] != '\0'; i ++)
			if (isspace(buf[i]))
			{
				buf[i] = '\0';
				lastchar[c] = i;
				c ++;
			}

		c = 0;
		sliceid = atoi(buf+c); c = lastchar[0] + 1;
		emin = (float)atof(buf + c);  c = lastchar[1] + 1;
		emax = (float)atof(buf + c);  c = lastchar[2] + 1;
		hitcnt = (float)atof(buf +c); c = lastchar[3] + 1;
		hiterr = (float)atof(buf +c); c = lastchar[4] + 1;

		for (n = 0; n < 8 * 3; n ++)
		{
			vdata[n] = (float)atof(buf + c);
			c = lastchar[n + 5] + 1;
		}

		for (i = 0; i < 8; i ++) 
		{
			v0[0] = vdata[i*3];
			v0[1] = vdata[i*3+1];
			v0[2] = vdata[i*3+2];			
			v0[3] = 1.f;
			vcbMatMult4fv(v1, mat, v0);

			vdata[i*3]   = v1[0];
			vdata[i*3+1] = v1[1];
			vdata[i*3+2] = v1[2];			
			/*printvertexf(v1, 3);*/
			//printf(" %f %f %f", v1[0], v1[1], v1[2]);
		}

		fwrite(&sliceid, sizeof(int), 1, stdout);
		fwrite(&emin, sizeof(float), 1, stdout);
		fwrite(&emax, sizeof(float), 1, stdout);
		fwrite(&hitcnt, sizeof(float), 1, stdout);
		fwrite(&hiterr, sizeof(float), 1, stdout);
		fwrite(vdata, sizeof(float), 8*3, stdout);
	}

	free(buf);
	return nfacets;
}
