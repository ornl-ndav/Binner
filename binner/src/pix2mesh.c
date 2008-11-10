#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vcbmath.h"
#include "vcblinalg.h"
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

void swapVec4(float * v1, float * v2)
{
	int j;
	float tmp [4];
	
	for (j = 0; j < 4; j ++)
	{
		tmp[j] = v1[j];
		v1[j] = v2[j];
		v2[j] = tmp[j];
	}
}


float ordermetric(float * ctr, float * n, float * v1, float * v2)
{
	float vec1[4], vec2[4], cross[4];
	int i;
	float val;

	for (i = 0; i < 4; i ++)
	{
		vec1[i] = v1[i] - ctr[i];
		vec2[i] = v2[i] - ctr[i];
	}
	
	la_cross3(cross, vec1, vec2);
	val = la_norm3(cross);
	
	if (la_dot3(n, cross) > 0)
		val *= -1;
	
	return val;
}


/* this program generates a single paralleliped */

int main(int argc, char ** argv)
{
	int i, j, k;
	float ctr[4], normals[8][4], fnormal[4], fctr[4];
	int d;
	float distvec[4], dist;
	float tmp[4], tmp2[4], cross1[4], cross2[4], tab[4];
	int lh[2], step = 0, pivot, opposite;

	ctr[0] = ctr[1] = ctr[2] = ctr[3] = 0.f;
	
	for (i = 0; i < 8; i ++)
	{
		scanf("%f, %f, %f, %f\n", &vertices[i][0],&vertices[i][1],&vertices[i][2],&vertices[i][3]);
		for (j = 0; j < 4; j ++)
			ctr[j] += vertices[i][j]/8.f;
	}

	for (lh[0] = 0, lh[1] = 7, step = 0; lh[0] < lh[1]; step ++)
	{
		pivot = lh[step%2];
		opposite = lh[1 - (step%2)];

		for (i = lh[0], d = pivot, dist = 0.f; i <= lh[1]; i ++)
		{
			for (j = 0; j < 4; j ++)
				distvec[j] = vertices[i][j] - vertices[pivot][j];

			if (la_norm3(distvec) > dist)
			{
				dist = la_norm3(distvec);
				d = i;
			}
		}

		if (opposite != d)
		{
			printf("exchanging vertex %d with vertex %d\n", opposite, d);
			swapVec4(vertices[opposite], vertices[d]);
		}

		if (pivot < opposite) 
			lh[0]++;
		else
			lh[1]--;
	}

	for (i = 0; i < 8; i ++)
	{
		for (j = 0; j < 4; j ++)
			normals[i][j] = vertices[i][j] - ctr[j];
		la_normalize3(normals[i]);
	}

	/* now we have 2 oppositing faces, let's make vertices appear in the right order */
	
	for (i = 0; i < 4; i ++)
	{
		fctr[i] = 0;
		fnormal[i] = 0;
		for (j = 0; j < 4; j ++)
		{
			fnormal[i] += normals[j][i];
			fctr[i] += vertices[j][i]/4.f;
		}
	}
	la_normalize3(fnormal);

	for (k = 0; k < 2; k ++)
	{
		for (j = k; j < 4; j ++)
			tab[j] = ordermetric(fctr, fnormal, vertices[k], vertices[j]);

		for (i = k+1, d = 0, dist = 0; i < 4; i ++)
		{
			if (dist < tab[i])
			{
				dist = tab[i];
				d = i;
			}
		}
		
		if ( k+1 != d)
		{
			printf("exchanging vertex %d with vertex %d\n", k+1, d);
			swapVec4(vertices[k+1], vertices[d]);
		}
	}

	/* now need to align the vertices on two faces, basically fix the anchors at 0 and 4 */
	for (k = 0; k < 4; k ++)
	{
		for (i = 0, dist = 1e6; i < 4; i ++)
		{
			for (j = 0; j < 4; j ++)
				distvec[j] = vertices[i+4][j] - vertices[k][j];

			if (la_norm3(distvec) < dist)
			{
				dist = la_norm3(distvec);
				d = i+4;
			}
		}
		
		if ( k + 4 != d)
		{
			printf("exchanging vertex %d with vertex %d\n", d, k+4);
			swapVec4(vertices[d], vertices[k+4]);
		}
	}

	for (i = 0; i < 6; i ++) {
		printf("4 ");
		for (j = 0; j < 4; j ++)
			printvertexf(vertices[quads3d[i][j]], 3); 
		printf("\n");
	}

	return 6;
}
