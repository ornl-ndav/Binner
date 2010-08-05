/**

   \ingroup rebinner_execs
   
   \file src/gmeshorderv.c

   \brief CURRENT executable for fixing vertex ordering.

   $Id$

*/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cell.h"


int main(int argc, char ** argv)
{
	int i, n, buf[1];
	double vdata[4 + 8*3], *v, out[8*3];
	float corners[8][4];
	int id_array[8] = {0, 1, 2, 3, 4, 5, 6, 7};
	int ids[8];

	v = vdata + 4;
	
	for (i = 0; i < 8; corners[i][3] = 0., i ++);
	
	for (n = 0; ; n ++) 
    {

		if (fread(buf, sizeof(int), 1, stdin) <= 0)
			break;

		fwrite(buf, sizeof(int), 1, stdout);

		fread(vdata, sizeof(double), 4 + 8 *3, stdin);
		memcpy(out, v, sizeof(double)*8*3);
		memcpy(ids, id_array, sizeof(int)*8);

		for (i = 0; i < 8; i ++)
		{
			corners[i][0] = v[i*3];
			corners[i][1] = v[i*3+1];
			corners[i][2] = v[i*3+2];
		}

		correctCornersf3d(corners, ids);

		for (i = 0; i < 8; i ++)
		{
			v[i*3]   = out[ids[i]*3];
			v[i*3+1] = out[ids[i]*3 + 1];
			v[i*3+2] = out[ids[i]*3 + 2];
		}

		fwrite(vdata, sizeof(double), 4 + 8*3, stdout);
	}

	return n;
}
