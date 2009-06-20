#include <stdlib.h>
#include <stdio.h>

int main(int argc, char ** argv)
{
	int nfacets, i, n, sliceid;
	float mat[16], v0[4], v1[4], axis[3], angle;
	double * vdata, * v;
	double emin, emax, hitcnt, hiterr;
	char * buf;
	int lastchar[50], c;

	buf = malloc(1024*8);
	vdata = malloc(1024 * sizeof(double));

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
		sliceid = atof(buf+c); c = lastchar[0] + 1;
		emin = atof(buf + c);  c = lastchar[1] + 1;
		emax = atof(buf + c);  c = lastchar[2] + 1;
		hitcnt = atof(buf +c); c = lastchar[3] + 1;
		hiterr = atof(buf +c); c = lastchar[4] + 1;

		for (n = 0; n < 8 * 3; n ++)
		{
			vdata[n] = atof(buf + c);
			c = lastchar[n + 5] + 1;
		}

		fwrite(&sliceid, sizeof(int), 1, stdout);
		fwrite(&emin, sizeof(double), 1, stdout);
		fwrite(&emax, sizeof(double), 1, stdout);
		fwrite(&hitcnt, sizeof(double), 1, stdout);
		fwrite(&hiterr, sizeof(double), 1, stdout);
		fwrite(vdata, sizeof(double), 8*3, stdout);
	}

	free(vdata);
	free(buf);
	return nfacets;
}
