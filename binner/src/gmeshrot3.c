/**

   \ingroup rebinner_execs
   
   \file src/gmeshrot3.c

   \brief CURRENT executable for rotating gmesh geometry.

   $Id$

*/
 
 
#include <stdlib.h>
#include <stdio.h>
#include "vcblinalg.h"


void  matMult3x3(double * v0, double * mat, double * v1)
{
	double v[4];
	v[0] = mat[0] *v1[0] + mat[1] *v1[1] + mat[2] *v1[2]; /* + mat[3] *v1[3]; */
	v[1] = mat[4] *v1[0] + mat[5] *v1[1] + mat[6] *v1[2]; /* + mat[7] *v1[3]; */
	v[2] = mat[8] *v1[0] + mat[9] *v1[1] + mat[10]*v1[2]; /* + mat[11]*v1[3]; */
	/*v[3] = mat[12]*v1[0] + mat[13]*v1[1] + mat[14]*v1[2] + mat[15]*v1[3];*/

	v0[0] = v[0]; v0[1] = v[1]; v0[2] = v[2]; /* v0[3] = 1.f; */
}

int main(int argc, char ** argv)
{
	int i, n, buf[1];
	double vdata[4 + 8*3], *v;
	double mat[16], axis[3], angle;
	
	if (argc != 5)
	{
		fprintf(stderr, "usage: %s x_axis y_axis z_axis angle\n", argv[0]);
		exit(1);
	}
	
	axis[0] = atof(argv[1]);
	axis[1] = atof(argv[2]);
	axis[2] = atof(argv[3]);
	angle   = atof(argv[4]);

	vcbRotate3dv(mat, axis, angle);

	v = vdata + 4;
	
	for (n = 0; ; n ++) 
    {

		if (fread(buf, sizeof(int), 1, stdin) <= 0)
			break;

		fwrite(buf, sizeof(int), 1, stdout);
		
		fread(vdata, sizeof(double), 4 + 8 *3, stdin);

		for (i = 0; i < 8; i ++)
			matMult3x3(v + i * 3, mat, v + i*3);

		fwrite(vdata, sizeof(double), 4 + 8*3, stdout);
	}

	return 0;
}
