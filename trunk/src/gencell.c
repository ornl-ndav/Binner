#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vcbmath.h"
#include "binnerio.h"

float vertices[16][4] = {{0, 0, 0, 0},  /* vertex 0, 4 dims */
                         {1, 0, 0, 0},  /* vertex 1 */
						 {1, 1, 0, 0},  /* vertex 2 */
						 {0, 1, 0, 0},  /* vertex 3 */
						 {0, 0, 1, 0},  /* vertex 4 */
						 {1, 0, 1, 0},  /* vertex 5 */
						 {1, 1, 1, 0},  /* vertex 6 */
						 {0, 1, 1, 0},  /* vertex 7 */
						 {0, 0, 0, 1},  /* vertex 8 */
                         {1, 0, 0, 1},  /* vertex 9 */
						 {1, 1, 0, 1},  /* vertex 10 */
						 {0, 1, 0, 1},  /* vertex 11 */
						 {0, 0, 1, 1},  /* vertex 12 */
						 {1, 0, 1, 1},  /* vertex 13 */
						 {1, 1, 1, 1},  /* vertex 14 */
						 {0, 1, 1, 1}}; /* vertex 15 */

int quads3d[6][4] = {{0, 3, 2, 1}, /* out facing quads */
					 {4, 5, 6, 7},
					 {1, 2, 6, 5},
					 {2, 3, 7, 6},
					 {3, 0, 4, 7},
					 {0, 1, 5, 4}};

int quads4d[14][8]; 

/* this program generates a single paralleliped */

int main(int argc, char ** argv)
{
	int i, j;
	
	for (i = 0; i < 8; i ++)
		scanf("%f, %f, %f, %f\n", &vertices[i][0],&vertices[i][1],&vertices[i][2],&vertices[i][3]);
		
	for (i = 0; i < 6; i ++) {
		printf("4 ");
		for (j = 0; j < 4; j ++)
			printvertexf(vertices[quads3d[i][j]], 3); 
		printf("\n");
	}

	return 6;
}
