
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "vcblinalg.h"
#include "cell.h"
#include "geometry.h"

static float verticesf[16][4] = {{0, 0, 0, 0},  /* vertex 0, 4 dims */
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

static double verticesd[16][4] = {{0, 0, 0, 0},  /* vertex 0, 4 dims */
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

static int quads3d[6][4] = {{0, 3, 2, 1}, /* out facing quads */
					 {4, 5, 6, 7},
					 {1, 2, 6, 5},
					 {2, 3, 7, 6},
					 {3, 0, 4, 7},
					 {0, 1, 5, 4}};

int basicCubef(float * v)
{
	int i, j, cnt;
	/* return value: number of floats written in v */
	for (i = 0, cnt = 0; i < 6; i ++) {
		for (j = 0; j < 4; cnt += 3, j ++)
			memcpy(&v[cnt], &verticesf[quads3d[i][j]], 3*sizeof(float)); 
	}
	
	return cnt;
}

int basicCubed(double * v)
{
	int i, j, cnt;
	/* return value: number of floats written in v */
	for (i = 0, cnt = 0; i < 6; i ++) {
		for (j = 0; j < 4; cnt += 3, j ++)
			memcpy(&v[cnt], &verticesd[quads3d[i][j]], 3*sizeof(double)); 
	}
	
	return cnt;
}

int realCubef(float (* corners)[4], float * v) /* corners: input, v: output quad list */
{
	/* assume corners has at least 8 * 4 floats */
	/* assume v has at least 6 * 4 *4 floats */
	/* assume all coordinates are in x,y,z */
	
	int i, j;
	for (i = 0; i < 6; i ++)
		for (j = 0; j < 4; j ++)
			memcpy(&v[(i*4 + j)*4], 
				   corners[quads3d[i][j]], 
				   4 * sizeof(float));

	return 6;
}

static void swapVec4(float * v1, float * v2)
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

static void swapVec4_id(float * v1, float * v2, int * id1, int * id2)
{
	int j;
	float tmp [4];
	
	for (j = 0; j < 4; j ++)
	{
		tmp[j] = v1[j];
		v1[j] = v2[j];
		v2[j] = tmp[j];
	}
	
	j = id1[0];
	id1[0] = id2[0];
	id2[0] = j;
}

static float ordermetric(float * ctr, float * n, float * v1, float * v2)
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

int correctCornersf3d(float (* vertices)[4], int * ids) /* vertices: input/output */
{
	/* assume vertices has at least 8 * 4 floats */
	/* assume all coordinates are in x,y,z,w */
	int i, j, k;
	float ctr[4], normals[8][4], fnormal[4], fctr[4];
	int d;
	float distvec[4], dist;
	float tab[4];
	int lh[2], step = 0, pivot, opposite;

	ctr[0] = ctr[1] = ctr[2] = ctr[3] = 0.f;
	
	for (i = 0; i < 8; i ++)
	{
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
#ifdef DEBUG
			printf("exchanging vertex %d with vertex %d\n", opposite, d);
#endif
			if (ids == NULL)
				swapVec4(vertices[opposite], vertices[d]);
			else
				swapVec4_id(vertices[opposite], vertices[d], ids + opposite, ids+ d);
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
#ifdef DEBUG
			printf("exchanging vertex %d with vertex %d\n", k+1, d);
#endif
			if (ids == NULL)
				swapVec4(vertices[k+1], vertices[d]);
			else
				swapVec4_id(vertices[k+1], vertices[d], ids + k+1, ids+ d);

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
#ifdef DEBUG
			printf("exchanging vertex %d with vertex %d\n", d, k+4);
#endif
			if (ids == NULL)
				swapVec4(vertices[d], vertices[k+4]);
			else
				swapVec4_id(vertices[d], vertices[k+4], ids + d, ids+ k+4);
		}
	}
	
	return 0;
}

void parallelipedByInputd(double * v)
{
	int i, j, cnt;

	for (i = 0; i < 8; i ++)
		scanf("%lf, %lf, %lf, %lf\n",
		       &verticesd[i][0],
			   &verticesd[i][1],
			   &verticesd[i][2],
			   &verticesd[i][3]);

	for (i = 0, cnt = 0; i < 6; i ++) {
		for (j = 0; j < 4; cnt += 3, j ++)
			memcpy(&v[cnt], &verticesd[quads3d[i][j]], 3*sizeof(double)); 
	}

}

void parallelipedByInputf(float * v)
{
	int i, j, cnt;

	for (i = 0; i < 8; i ++)
		scanf("%f, %f, %f, %f\n", 
			  &verticesf[i][0],
			  &verticesf[i][1],
			  &verticesf[i][2],
			  &verticesf[i][3]);

	for (i = 0, cnt = 0; i < 6; i ++) {
		for (j = 0; j < 4; cnt += 3, j ++)
			memcpy(&v[cnt], &verticesf[quads3d[i][j]], 3*sizeof(float)); 
	}

}

void vRot3f(int n, float * v, float * axis, float angle)
{
	int i;
	float mat[16], v0[4], v1[4];

	vcbRotate3fv(mat, axis, angle);
	v0[3] = 1.f;

	for (i = 0; i < n; i ++) {
		v0[0] = v[i*3+0];
		v0[1] = v[i*3+1];
		v0[2] = v[i*3+2];
		vcbMatMult4fv(v1, mat, v0);
		v[i*3+0] = v1[0];
		v[i*3+1] = v1[1];
		v[i*3+2] = v1[2];
	}
}

void vScale3f(int n, float * v, float * s) /* scale factors s[3] */
{
	int i;
	float mat[16], v0[4], v1[4];

	vcbScale3fv(mat, s);
	v0[3] = 1.f;
	
	for (i = 0; i < n; i ++) {
		v0[0] = v[i*3+0];
		v0[1] = v[i*3+1];
		v0[2] = v[i*3+2];
		vcbMatMult4fv(v1, mat, v0);
		v[i*3+0] = v1[0];
		v[i*3+1] = v1[1];
		v[i*3+2] = v1[2];
	}
}

void vTrans3f(int n, float *v, float * t) /* translat factors t[3] */
{
	int i;
	float mat[16], v0[4], v1[4];

	vcbTranslate3fv(mat, t);
	v0[3] = 1.f;

	for (i = 0; i < n; i ++) {
		v0[0] = v[i*3+0];
		v0[1] = v[i*3+1];
		v0[2] = v[i*3+2];
		vcbMatMult4fv(v1, mat, v0);
		v[i*3+0] = v1[0];
		v[i*3+1] = v1[1];
		v[i*3+2] = v1[2];
	}
}

/*
	for (i = 0; i < 6; i ++) {
		for (j = 0; j < 4; j ++)
			printvertex(vertices[quads3d[i][j]], 3); 
		printf("\n");
	}

	return 0;
*/
