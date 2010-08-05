/**
   \ingroup rebinner_tests
   \file src/gencell.c

   \brief CURRENT test of core API -- cell.h

   this program generates a single paralleliped
   
   $Id$

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "cell.h"
#include "binnerio.h"


int main(int argc, char ** argv)
{
#if 0
	int i, j;
	
	for (i = 0; i < 8; i ++)
		scanf("%f, %f, %f, %f\n", &vertices[i][0],&vertices[i][1],&vertices[i][2],&vertices[i][3]);
		
		
	for (i = 0; i < 6; i ++) {
		printf("4 ");
		for (j = 0; j < 4; j ++)
			printvertexf(vertices[quads3d[i][j]], 3); 
		printf("\n");
	}
#else
	int i, j;
	float corners[8][4];
	float v[6*4*4];
	
	for (i = 0; i < 8; i ++)
		scanf("%f, %f, %f, %f\n", &corners[i][0],
		                          &corners[i][1],
								  &corners[i][2],
								  &corners[i][3]);

	realCubef(corners, v);

	for (i = 0; i < 6; i ++) {
		printf("4");
		for (j = 0; j < 4; j ++)
			printf(" %f %f %f", v[(i*4+j)*4+0], v[(i*4+j)*4+1], v[(i*4+j)*4+2]); 
		printf("\n");
	}
#endif

	return 6;
}
