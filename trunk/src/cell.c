
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
