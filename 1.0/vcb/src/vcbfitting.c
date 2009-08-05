#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "vcbfitting.h"
#include "nr.h"
#include "nrutil.h"
#include "vcbdebug.h"

void fpoly(float x, float p[], int np)
{
	int j;
	p[1]=1.0;
	for (j=2;j<=np;j++) p[j]=p[j-1]*x;
}


void vcbFit1D(float * x, float * y, float * given_sig, 
		   int order, int num_elements, 
		   float * coeff, float * chisq)
{
#undef _USESVD
/* #define _USESVD */
	int i, len, ma, noidea = 0;
	float *sig, * w;
	float ** u, ** v, ** cvm;

#ifndef _USESVD
	int * ia;
#endif

	len = num_elements;
	sig = (given_sig == NULL) ? ((float *) malloc (sizeof(float) * len)) : given_sig;
	if (given_sig == NULL)  /* if no sigma's are provided, one's are used */
		for (i = 0; i < len; sig[i] = 1.f, i ++)
			;

	assert(sig != NULL);
	ma = order+1;

	u = matrix(1,len,1,ma);   /* u, v, w are work space for svdfit */
	v = matrix(1,ma,1,ma);
	w = vector(1,ma);
	cvm = matrix(1,ma,1,ma); /* convariance matrix */

	/*
	void lfit(float x[], float y[], float sig[], int ndat, float a[], int ia[],
		  int ma, float **covar, float *chisq, void (*funcs)(float, float [], int));
	*/

#ifdef _USESVD
	svdfit (x-1,y-1,sig-1,len, coeff-1, ma, u, v, w, chisq, fpoly);
#else
	ia = (int*) malloc (sizeof(int)*ma);
	for (i = 0; i < ma; i ++) ia[i] = 1;
	lfit(x-1,y-1,sig-1,len,coeff-1,ia-1, ma, cvm, chisq,fpoly);
	free(ia);
#endif

#undef _USESVD

	if (*chisq > 20) {

		VCB_WARNING("chisq = %f in vcbFit1D\nX ",*chisq);
		for (i = 0; i < len; i ++)
			VCB_WARNING("%f ",x[i]);
		VCB_WARNING("\nY ");
		for (i = 0; i < len; i ++)
			VCB_WARNING("%f ",y[i]);
		VCB_WARNING("\nnY ");
		for (i = 0; i < len; i ++)
			VCB_WARNING("%f ",vcbEvalPoly1D(coeff, ma, x[i]));
		VCB_WARNING("\nSTD ");
		for (i = 0; i < len; i ++)
			VCB_WARNING("%f ",sig[i]);
		VCB_WARNING("\n");

		for (i = 0; i < ma; i ++)
			VCB_WARNING("%f ",coeff[i]);
		VCB_WARNING("\n");
	}


#if 0 /* uncomment to see the covariance matrix of the coefficients */
	svdvar(v,ma,w,cvm);
	printf("the covariance matrix is:\n");
	for (i = 0; i < ma; i ++) {
		int j;
		for (j = 0; j < ma; j ++)
			printf("%f ",cvm[i+1][j+1]);
		printf("\n");
	}
#endif

	if (given_sig == NULL) free(sig);
	free_matrix(u,1,ma,1,ma);
	free_matrix(v,1,ma,1,ma);
	free_matrix(cvm,1,ma,1,ma);
	free_vector(w,1,ma);
}


float vcbEvalPoly1D(float * coeff, int order, float t)
{
	int i = 0;
	float xterm = 1.0f, result = 0.f;
	
	while (i < order) {
		result += coeff[i] * xterm;
		xterm *= t;
		i += 1;
	}

	return result;
}


float vcbEvalPolyd1D(float * coeff, float t, int polyorder, int dorder)
{
#ifndef VCB_MAX_ORDER
#define VCB_MAX_ORDER 15
#endif

	int   i,j;
	float x, result;
	float terms[VCB_MAX_ORDER];
	int   exponents[VCB_MAX_ORDER];

	if (polyorder > VCB_MAX_ORDER) {
		VCB_WARNING("vcbEvalPolyd1D failing, the requested \
			%d orders is higher than maximally allowed %d\n", polyorder, \
			VCB_MAX_ORDER);
		exit(0);
	}

	for (i = 0; i < polyorder; terms[i]=coeff[i], exponents[i]=i, i ++)
		;

	for (i = 0; i < dorder; i ++) {
		for (j = 0; j < polyorder; j ++) {
			terms[j] *= exponents[j];
			exponents[j] --;
		}
	}

	result = 0.f; x = 1.f;
	for (i = 0; i < polyorder; i ++) {
		if (exponents[i] > 0) x*= t;
		if (exponents[i]>= 0) result += (terms[i] * x);
	}

	return result;

#undef VCB_MAX_ORDER
}

