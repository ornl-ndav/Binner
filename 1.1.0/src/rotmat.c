#include <stdlib.h>
#include <stdio.h>
#include "vcblinalg.h"

int main(int argc, char ** argv)
{
	int i;
	float mat[16], axis[3], angle;
	
	if (argc != 5)
	{
		fprintf(stderr,"usage: %s axis_x axis_y axis_z rotation_angle_in_degree\n", argv[0]);
		exit(1);
	}
	
	axis[0] = (float)atof(argv[1]);
	axis[1] = (float)atof(argv[2]);
	axis[2] = (float)atof(argv[3]);
	angle   = (float)atof(argv[4]);
	vcbRotate3fv(mat, axis, angle);

	printf("%e %e %e ", mat[0], mat[1], mat[2]);
	printf("%e %e %e ", mat[4], mat[5], mat[6]);
	printf("%e %e %e \n", mat[8], mat[9], mat[10]);

	return 0;
}
