
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vcblinalg.h"

#ifndef VCB_M_PI
#ifndef VCB_M_PI
#define VCB_M_PI 3.14159265f
#endif
#endif

char * vcbPrintVec (float * a, int n)
{
	static char printvec_msg[300];
	int i, ilen = 0;

	sprintf(printvec_msg,"(");
	for (i = 0; i < n; i ++) {
		ilen = (int)strlen(printvec_msg);
		sprintf(&printvec_msg[ilen],"%f,", a[i]);
	}

	printvec_msg[strlen(printvec_msg)-1] = ')';
	printvec_msg[strlen(printvec_msg)] = '\0';

	return printvec_msg;
}

void  la_cross3(float * res, float * v1, float * v2)
{
	res[0] = v1[1]*v2[2] - v1[2]*v2[1];
	res[1] = v1[2]*v2[0] - v1[0]*v2[2];
	res[2] = v1[0]*v2[1] - v1[1]*v2[0];
}

void  la_dupvec(float * dup, float * v)
{
	dup[0] = v[0];
	dup[1] = v[1];
	dup[2] = v[2];
}

void  la_dupmat(float * dup, float * mat)
{
	int i;
	for (i = 0; i < 16; dup[i] = mat[i], i ++);
}

float la_dot3(float * v1, float * v2)
{
	return (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2]);
}

void  la_subtract(float * v1, float * v2)
{
	v1[0] -= v2[0];
	v1[1] -= v2[1];
	v1[2] -= v2[2];
}

void  la_add(float * v1, float * v2)
{
	v1[0] += v2[0];
	v1[1] += v2[1];
	v1[2] += v2[2];
}

float la_norm3(float * v)
{
	return (float)sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
}

void  la_normalize3(float * v)
{
	float norm = la_norm3(v);

	if (norm < 1e-8) {
		v[0] = v[1] = v[2] = 0.f;
	}
	else {
		v[0] /= norm;
		v[1] /= norm;
		v[2] /= norm;
	}
}
	
void  vcbIdentity3D(float * mat)
{
	int i;
	for (i = 0; i < 16; mat[i] = 0.f, i++);
	mat[0] = mat[5] = mat[10] = mat[15] = 1.f;
}

void  vcbTranslate3f(float * mat, float x, float y, float z)
{
	vcbIdentity3D(mat);
	mat[3]  = x;
	mat[7]  = y;
	mat[11] = z;
}

void  vcbTranslate3fv(float * mat, float * v)
{
	vcbTranslate3f(mat, v[0], v[1], v[2]);
}

void  vcbScale3f(float * mat, float sx, float sy, float sz)
{
	vcbIdentity3D(mat);
	mat[0]  = sx;
	mat[5]  = sy;
	mat[10] = sz;
}

void  vcbScale3fv(float * mat, float * sv)
{
	vcbScale3f(mat, sv[0], sv[1], sv[2]);
}

void  vcbRotate3f(float * mat, float ax, float ay, float az, float angle)
{
	float axis[3];
	axis[0] = ax;
	axis[1] = ay;
	axis[2] = az;
	vcbRotate3fv(mat,axis,angle);
}

void  vcbRotate3fv(float * mat, float * axis, float angle)
{
    float  angleRad, c, s, t; 
	
	angleRad = angle * VCB_M_PI / 180.0f,
	c = (float)cos((double)angleRad);
	s = (float)sin((double)angleRad);
	t = 1.0f - c;

	la_normalize3(axis);
	mat[0]  = t * axis[0] * axis[0] + c;
	mat[1]  = t * axis[0] * axis[1] - s * axis[2];
	mat[2]  = t * axis[0] * axis[2] + s * axis[1];
	mat[3]  = 0.f;
	mat[4]  = t * axis[0] * axis[1] + s * axis[2];
	mat[5]  = t * axis[1] * axis[1] + c;
	mat[6]  = t * axis[1] * axis[2] - s * axis[0];
	mat[7]  = 0.f;
	mat[8]  = t * axis[0] * axis[2] - s * axis[1];
	mat[9]  = t * axis[1] * axis[2] + s * axis[0];
	mat[10] = t * axis[2] * axis[2] + c;
	mat[11] = 0.f;
	mat[12] = mat[13] = mat[14] = 0.f;
	mat[15] = 1.f;
}

void  vcbInvMat(float * invMat, float * origmat)
{
	int i, j, i1, si;
	float tmp, mat[16];

	la_dupmat(mat, origmat);

	/* 
	 * As mat evolves from original mat into identity, vcbInvMat 
	 * evolve from identity to inverse(mat)
	 */
	vcbIdentity3D(invMat);

    /* Loop over cols of a from left to right, eliminating above and below diag */
    for (j=0; j<4; j++) {   /* Find largest pivot in column j among rows j..3 */
		i1 = j;		        /* Row with largest pivot candidate */

		for (i=j+1; i<4; i++)
			if (fabs((double)mat[i*4+j]) > fabs((double)mat[i1*4+j]))
				i1 = i;

		/* Swap rows i1 and j in mat and vcbInvMat to put pivot on diagonal */
		for (si = 0; si < 4; tmp=mat[i1*4+si], mat[i1*4+si]=mat[j*4+si], mat[j*4+si]=tmp, si++);
		for (si = 0; si < 4; tmp=invMat[i1*4+si], invMat[i1*4+si]=invMat[j*4+si], invMat[j*4+si]=tmp, si++);

		/* Scale row j to have a unit diagonal */
		if (mat[j*4+j] == 0.f) {
			fprintf(stderr,"Error: linalg vcbInvMat encountered a singular matrix, can't invert.\n");
			return;
		}

		tmp = mat[j*4+j];
		for (si = 0; si < 4; invMat[j*4+si]/=tmp, si++);
		for (si = 0; si < 4; mat[j*4+si] /= tmp, si++);

		/* Eliminate off-diagonal elems in col j of a, doing identical ops to b */
		for (i=0; i<4; i++)
			if (i!=j) {
				tmp = mat[i*4+j];
				for (si = 0; si < 4; invMat[i*4+si] -= (tmp*invMat[j*4+si]), si++);
				for (si = 0; si < 4; mat[i*4+si] -= (tmp*mat[j*4+si]), si++);
			}
	}
}

void  vcbInvMatr(float *vcbinvmat)
{
	float mat[16];
	int i;
	for (i = 0; i < 16; mat[i] = vcbinvmat[i], i ++);
	vcbInvMat(vcbinvmat,mat);
}

void  vcbMatMult4fv(float * v0, float * mat, float * v1)
{
	float v[4];
	v[0] = mat[0] *v1[0] + mat[1] *v1[1] + mat[2] *v1[2] + mat[3] *v1[3];
	v[1] = mat[4] *v1[0] + mat[5] *v1[1] + mat[6] *v1[2] + mat[7] *v1[3];
	v[2] = mat[8] *v1[0] + mat[9] *v1[1] + mat[10]*v1[2] + mat[11]*v1[3];
	v[3] = mat[12]*v1[0] + mat[13]*v1[1] + mat[14]*v1[2] + mat[15]*v1[3];

	v0[0] = v[0]/v[3]; v0[1] = v[1]/v[3]; v0[2] = v[2]/v[3]; v0[3] = 1.f;
}

void  vcbMatMult4f(float * v, float * mat, float x, float y, float z, float w)
{
	float v1[4];
	v1[0] = x; v1[1] = y; v1[2] = z; v1[3] = w;
	vcbMatMult4fv(v, mat, v1);
}

void  vcbTransposeMat(float *tmat, float * mat)
{
	tmat[0]  = mat[0]; tmat[1]  = mat[4]; tmat[2]  = mat[8];  tmat[3]  = mat[12];
	tmat[4]  = mat[1]; tmat[5]  = mat[5]; tmat[6]  = mat[9];  tmat[7]  = mat[13];
	tmat[8]  = mat[2]; tmat[9]  = mat[6]; tmat[10] = mat[10]; tmat[11] = mat[14];
	tmat[12] = mat[3]; tmat[13] = mat[7]; tmat[14] = mat[11]; tmat[15] = mat[15];
}

void  vcbTransposeMatr(float *tmat)
{
	float mat[16];
	int i;
	for (i = 0; i < 16; mat[i] = tmat[i], i ++);
	vcbTransposeMat(tmat,mat);
}

void  vcbMatMultM(float *fmat, float * mat)
{
	float tmpmat[16];
	int i, j, k;

	la_dupmat(tmpmat, fmat);
	for (i = 0; i < 4; i ++)
		for (j = 0; j < 4; j ++) {
			fmat[i*4+j] = 0.f;
			for (k = 0; k < 4; k ++)
				fmat[i*4+j] += tmpmat[i*4+k] * mat[k*4 + j];
		}
}

void  vcbOrtho3D(float * mat, float l, float r, float b, float t, float n, float f)
{
	mat[0] = 2.f/(r-l); mat[1] = 0.f;       mat[2] = 0.f;        mat[3] = -(r+l)/(r-l);
	mat[4] = 0.f;       mat[5] = 2.f/(t-b); mat[6] = 0.f;        mat[7] = -(t+b)/(t-b);
	mat[8] = 0.f;       mat[9] = 0.f;       mat[10]= -2.f/(f-n); mat[11]= -(f+n)/(f-n);
	mat[12]= 0.f;       mat[13]= 0.f;       mat[14]= 0.f;        mat[15]= 1.f;
}

void  vcbFrustum3D(float * mat, float l, float r, float b, float t, float n, float f)
{
	mat[0] = 2.f*n/(r-l); mat[1] = 0.f;         mat[2] = (r+l)/(r-l);  mat[3] = 0.f;
	mat[4] = 0.f;         mat[5] = 2.f*n/(t-b); mat[6] = (t+b)/(t-b);  mat[7] = 0.f;
	mat[8] = 0.f;         mat[9] = 0.f;         mat[10]= -(f+n)/(f-n); mat[11]= -2.0f*f*n/(f-n);
	mat[12]= 0.f;         mat[13]= 0.f;         mat[14]= -1.f;         mat[15]= 0.f;
}

void  vcbPerspective3D(float * mat, float fovy, float aspect, float zNear, float zFar)
{
	float xmin, xmax, ymin, ymax;

	ymax = zNear * (float)tan(fovy * VCB_M_PI / 360.0f);
	ymin = -ymax;
	xmin = ymin * aspect;
	xmax = ymax * aspect;
  
	vcbFrustum3D(mat, xmin, xmax, ymin, ymax, zNear, zFar);
}

void  vcbLookAtRotMat(float * mat, float * eye, float * center, float * up)
{
   float x[3], y[3], z[3];

   /* Make rotation matrix */

   /* Z vector: z = eye-center */
   la_dupvec(z, eye);
   la_subtract(z, center);
   la_normalize3(z);

   /* Y vector: y = up */
   la_dupvec(y, up);

   /* X vector = Y cross Z */
   la_cross3(x, y, z);

   /* Recompute Y = Z cross X */
   la_cross3(y, z, x);

   la_normalize3(x);
   la_normalize3(y);

   /* rotational matrix */
   mat[0]  = x[0]; mat[1]  = x[1]; mat[2]  = x[2]; mat[3]  = 0.f;
   mat[4]  = y[0]; mat[5]  = y[1]; mat[6]  = y[2]; mat[7]  = 0.f;
   mat[8]  = z[0]; mat[9]  = z[1]; mat[10] = z[2]; mat[11] = 0.f;
   mat[12] = 0.f;  mat[13] = 0.f;  mat[14] = 0.f;  mat[15] = 1.f;
}

void  vcbLookAt(float * mat, float * eye, float * center, float * up)
{
   float transmat[16];

   /* Make rotation matrix */
   vcbLookAtRotMat(mat, eye, center, up);
 
   /* translate eye to origin */
   vcbTranslate3f(transmat, -eye[0], -eye[1], -eye[2]);
   vcbMatMultM(mat,transmat);
}

void  vcbViewPortMat(float * mat, int scrWidth, int scrHeight)
{
	float scalemat[16];
	vcbScale3f(scalemat,scrWidth/2.f, scrHeight/2.f, 1.f);
	vcbTranslate3f(mat,scrWidth/2.f, scrHeight/2.f, 0.f);
	vcbMatMultM(mat, scalemat);
}
