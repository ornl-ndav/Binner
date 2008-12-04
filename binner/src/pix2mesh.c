#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vcbmath.h"
#include "cell.h"
#include "binnerio.h"

/* this program generates a single paralleliped */

int main(int argc, char ** argv)
{
	int i, j;
	float corners[8][4];
	float v[6*4*4];

	for (i = 0; i < 8; i ++)
		scanf("%f, %f, %f, %f\n", &corners[i][0],
		                          &corners[i][1],
								  &corners[i][2],
								  &corners[i][3]);
	correctCornersf3d(corners);
	realCubef(corners, v);
	for (i = 0; i < 6; i ++) {
		printf("4");
		for (j = 0; j < 4; j ++)
			printf(" %f %f %f", v[(i*4+j)*4+0], v[(i*4+j)*4+1], v[(i*4+j)*4+2]); 
		printf("\n");
	}
	return 6;
}
