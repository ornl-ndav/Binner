#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include "vcbmath.h"
#include "vcbfitting.h"
#include "nr.h"
#include "vcbutils.h"
#include "nrutil.h"

#ifndef VCB_EPSILON
#ifndef VCB_EPSILON
#define VCB_EPSILON 1e-8
#endif
#endif
#ifndef _VCB__M_PI
#define _VCB__M_PI   3.1415926535897932384626433832795
#endif

double gaussian(double r)
{
/*
 * The legacy parameters for 6 different splatting kernels
 *      type      radius     scale factor    stddev     vcbRange 
 *   Gaussian2.0    2.0       3.37313867   0.0017575   0.01030
 *   Gaussian3.0    2.0       1.85553277   0.0212080   0.10415
 *   Gaussian4.0    2.0       1.20615399   0.0697269   0.34171
 *   Gaussian3.0    1.6       1.83131325   0.0240710   0.13957
 *   Gaussian4.0    1.4       1.19117141   0.0707025   0.32957
 *   Bessel         2.0       2.50445032   0.0012753   0.00895
 *
 * the most important parameters are radius and scale factor.
 * stddev and vcbRange are not used.
 */	
	static double scalefac = 1/3.37313867;
	double res;

	if (r < 0.) r = -r;
	if (r >= 2.0) return 0.;

	res = scalefac * pow(2.0,-2.0*r*r);

	return res;
}

double crawfisian( double r)
{
      double s = 0.8893919243498159;
      double t = 1.556227804798694;
      double a = 0.5575267703530060;
      double b = -1.157744128784905;
      double c = 0.6710333014812285;
      double d = 0.067598983313541278;
      double e = 0.2824741750452964;
      double f = t*t*(d+e*t);
      double g = -t*(2*d+3*e*t);
      double h = d+3*e*t;
      double v, u, rs, y, z, p, q;

	  if (r < 0) r = -r;

      if(r == 0)
      {
         v = s*((a-f) + s*((-g)/2.0 + s*((b-h)/3.0 + s*(c+e)/4.0)))
           + t*(f + t*(g/2.0 + t*(h/3.0 + t*(-e)/4.0)));
      }
      else if(r < s)
      {
         rs = r*r;
         y = sqrt(s*s-rs);
         z = sqrt(t*t-rs);
         p = log(y+s);
         q = log(z+t);
         u = log(r);
         v = a*y + b*y*y*y/3.0 + b*y*rs + 0.5*c*rs*(y*s+rs*(p-u))
            + c*(y*s*s*s/4.0 - rs*0.125*(y*s+rs*(p-u)))
            + f*(z-y) + 0.5*(g-e*rs)*(z*t-y*s+rs*(q-p))
            + h*rs*(z-y) + (h/3.0)*(z*z*z-y*y*y)
            - e*(0.25*(z*t*t*t-y*s*s*s) - rs*0.125*(z*t-y*s+rs*(q-p)));
      }
      else if(r < t)
      {
         rs = r*r;
         z = sqrt(t*t-rs);
         q = log(z+t);
         u = log(r);
         v = f*z + 0.5*(g-e*rs)*(z*t+rs*(q-u)) + h*rs*z + (h/3.0)*z*z*z
            -e*(0.25*z*t*t*t-rs*0.125*(z*t+rs*(q-u)));
      }
      else
      {
         v = 0.0;
      }

      return 2.0*v;
}

double vcbGauss(double x, double s)
{ 
	return pow(2.,-((x/s)*(x/s))); 
}

double vcbSinc(double x)
{ 
	return (x > VCB_EPSILON ? (sin(_VCB__M_PI*x))/(_VCB__M_PI*x) : 1.0);
}

double vcbGaussWinSinc(double dist)
{
	/* 1.11 is an empirical number */
	if (dist < 0) dist = -dist;
	return (vcbGauss(dist, 1.11) * vcbSinc(dist));
}

double linear(double dist)
{
	if (dist < 0.) dist = -dist;
	if (dist > 1.) dist = dist - floor(dist);
	return (1.- dist);
}

double clamp(double x, double lo, double hi)
{ 
	return (x<lo)? lo : ((x>hi)? hi : x); 
}

/**
 * printvec is a utility function that mainly serves debugging purposes.
 *
 * printvec returns a string containing the requested data in the given format. The string
 * is always contained in the internal static char array. This function is intended to be used
 * in printf calls.  Note: in each printf only one printvec can be called.
 *
 * <pre>List of parameters:
 *	"a" is a pointer to the first element in the array to be printed.
 *	"n" is the length of the vector "a" (number of elements of "a").</pre>
 */
char * printvec (float * a, int n)
{
	static char printvec_msg[120];
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


void vcbNormalize3(float * a)
{
	float norm = (float)sqrt(vcbDotProd3(a,a));
	if (norm > 1e-6) {
		a[0] /= norm;
		a[1] /= norm;
		a[2] /= norm;
	}
	else {
		a[0] = 0.f;
		a[1] = 0.f;
		a[2] = 0.f;
	}
}


float vcbDotProd3(float * a, float *b)
{
	return (a[0]*b[0]+a[1]*b[1]+a[2]*b[2]);
}


void vcbCrossProd3(float * a, float * b, float * out)
{
   float c[3];
   c[0] = a[1]*b[2] - a[2]*b[1];
   c[1] = a[2]*b[0] - a[0]*b[2];
   c[2] = a[0]*b[1] - a[1]*b[0];

   out[0] = c[0]; out[1] = c[1]; out[2] = c[2];
}


void vcbAdd3 (float * a, float afac, float *b, float bfac, float *c)
{
	c[0] = a[0] * afac + b[0]*bfac;
	c[1] = a[1] * afac + b[1]*bfac;
	c[2] = a[2] * afac + b[2]*bfac;
}


void vcbAvgData1D(float * datain, int itv, int len, int kersize, 
					   float * dataout)
{
	int i, j, icnt, ilen;
	ilen = len * itv;

	for (i = 0; i < ilen; dataout[i] = 0.f, i ++)
		;

	for (i = 0; i < ilen; i ++) {
		icnt = 0;

		for (j = i - kersize*itv; j <= i + kersize*itv; j += itv)
			if (j >= 0 && j < ilen) {
				icnt += 1;
				dataout[i] +=  datain[j];
			}

		dataout[i] /= icnt;
	}
}



void vcbData1DGradient(float * datain, int itv, int len, int kersize, 
				 float * dataout)
{
	int i, j, tlen, lj, hj;
	tlen = len * itv;
	for (i = 0; i < tlen; dataout[i] = 0.f, i ++)
		;

	for (i = 0; i < tlen; i ++) {

		j = i - kersize*itv;
		if (j >=0) {
			dataout[i] -= datain[j];
			lj = j;
		}
		else {
			dataout[i] -= datain[i];
			lj = i;
		}

		j = i + kersize*itv;
		if (j < tlen) {
			dataout[i] += datain[j];
			hj = j;
		}
		else {
			dataout[i] += datain[i];
			hj = i;
		}
		dataout[i] /= ((hj - lj)/itv);
	}

}

void vcbSmoothData1D(float * coordsin, int itv, int len, int winsize, int order,
					    float * coordsout, float * dxyz)
{
	int i, j, k, cnt, ndata, ma;
	float * x, * y, * coeff, chisq, t;

	ndata = 2*winsize + 1; /* number of data points used in fitting */
	ma = order;                /* order of fit */

	x = (float *) malloc(sizeof(float)*ndata);
	y = (float *) malloc(sizeof(float)*ndata);
	coeff = (float *) malloc(sizeof(float)*(ma+1));

	/* construct the axis for fitting */
	for (j = -winsize, cnt = 0; j <= winsize; cnt++, j ++)
		x[cnt] = (float)j;

	t = 0.f;

	for (i = 0; i < len; i ++) {

		/* now treat components one by one */
		for (k = 0; k < itv; k++) {

			for (j = i - winsize, cnt = 0; j <= i + winsize; cnt++, j ++)
				y[cnt] = (j >= 0 && j < len) ? coordsin[j*itv+k]:coordsin[i*itv+k];
		
			vcbFit1D(x,y,NULL,ma,ndata,coeff,&chisq);
			dxyz[i*itv+k] = vcbEvalPolyd1D(coeff,t,ma,1);
			coordsout[i*itv+k] = vcbEvalPoly1D(coeff, ma, t);

		}
	}
}


void vcbCurvatureTorsion(float * coordsin, int itv, int len, int winsize, int order, 
							   float * c_t)
{
	int i, j, k, cnt, ndata, ma;
	float * x, * y, * coeff, chisq, t;
	float d1[3], d2[3], d3[3];  /* 1st, 2nd and 3rd derivatives */
	float curv, kappa, torsion; /* curvature, kappa and torsion */

	ndata = 2*winsize + 1;      /* number of data points used in fitting */
	ma = order;                 /* order of fit */

	x = (float *) malloc(sizeof(float)*ndata);
	y = (float *) malloc(sizeof(float)*ndata);
	coeff = (float *) malloc(sizeof(float)*(ma+1));

	/* construct the axis for fitting */
	for (j = -winsize, cnt = 0; j <= winsize; cnt++, j ++)
		x[cnt] = (float)j;

	t = 0.f;

	for (i = 0; i < len; i ++) {

		/* now treat components one by one */
		for (k = 0; k < itv; k++) {

			for (j = i - winsize, cnt = 0; j <= i + winsize; cnt++, j ++)
				y[cnt] = (j >= 0 && j < len) ? coordsin[j*itv+k]:coordsin[i*itv+k];
		
			vcbFit1D(x,y,NULL,ma,ndata,coeff,&chisq);
			d1[k] = vcbEvalPolyd1D(coeff,t,ma,1);
			d2[k] = vcbEvalPolyd1D(coeff,t,ma,2);
			d3[k] = vcbEvalPolyd1D(coeff,t,ma,3);
		}

		curv = (d1[1]*d2[2] - d2[1]*d1[2])*(d1[1]*d2[2] - d2[1]*d1[2]) +
			(d1[2]*d2[0] - d2[2]*d1[0])*(d1[2]*d2[0] - d2[2]*d1[0]) +
            (d1[0]*d2[1] - d2[0]*d1[1])*(d1[0]*d2[1] - d2[0]*d1[1]);

		kappa = (float) sqrt(curv/pow((double)vcbDotProd3(d1,d1), 3.0));

		if (fabs(curv)>1e-10) 
			torsion = (d1[0]*d2[1]*d3[2] + d2[0]*d3[1]*d1[2] 
			        + d3[0]*d1[1]*d2[2] - d3[0]*d2[1]*d1[2] 
			        - d2[0]*d1[1]*d3[2] - d1[0]*d3[1]*d2[2])/curv;
		else  
			torsion = 0.f;

		c_t[i*3 + 0] = curv;
		c_t[i*3 + 1] = kappa;
		c_t[i*3 + 2] = torsion;
	}

}


void vcbTurtleGeometry(int n, float * tangents, float * Whlu)
{
#ifndef vcb_id_H
#define vcb_id_H 0
#endif
#ifndef vcb_id_L
#define vcb_id_L 1
#endif
#ifndef vcb_id_U
#define vcb_id_U 2
#endif

	int i;
	float a[3], b[3], * H, U0[3], L[3], U[3], OMEGA[3]; /* temp varialbes */

	U0[0] = 0.f; U0[1] = 0.f; U0[2] = 1.f;
	H = &tangents[0];                    /* H vector is always the tangent */
	vcbCrossProd3(H,U0,L);
	vcbNormalize3(L);                       /* compute L vector */
	vcbCrossProd3(L,H,U);
	vcbNormalize3(U);                       /* compute U vector */

	/* the first rotation coefficents are zero */
	Whlu[vcb_id_H] = Whlu[vcb_id_L] = Whlu[vcb_id_U] = 0.f;

	/* now rotation starts from here on */
	for (i = 0; i < n-1; i ++) {
		H = &tangents[(i+1)*3];
		Whlu[i*3+vcb_id_H] = 0.f;            /* the rotation about H axis is always zero */
		Whlu[i*3+vcb_id_L] = vcbDotProd3(H,L);
		Whlu[i*3+vcb_id_U] = (-1.f)*vcbDotProd3(H,U);

		vcbAdd3(L,Whlu[i*3+vcb_id_L],U,Whlu[i*3+vcb_id_U],OMEGA);
		/*printf("OMEGA %s\n",printvec(OMEGA,3))*/;
		
		vcbCrossProd3(OMEGA,L,a);
		vcbCrossProd3(OMEGA,U,b);
		vcbAdd3(L,1.f,a,1.f,L); vcbNormalize3(L);
		vcbAdd3(U,1.f,b,1.f,U); vcbNormalize3(U);
	}

#undef vcb_id_H
#undef vcb_id_L
#undef vcb_id_U
}

static void (* eget_vector)(float, float *, float *);
static void __vcbmath_get_vector(float x_val, float * coord, float * dydx)
{
	coord ++;
	dydx ++;
	eget_vector(x_val, coord, dydx);
}

int vcbAdvectFlow(float * curloc, int ncoord, float *lbounds, float * ubounds, float t_sz, float eps, float h1, float hmin, void (*get_vector)(float, float *, float *))
{
	/* return 0 - normal, -1 - stopped, -2 - stepping outside the volume */
	int nok, nbad, i;
	eget_vector = get_vector;
	if (odeint(curloc-1, ncoord, 0.f, t_sz, eps, h1, hmin, &nok, &nbad, __vcbmath_get_vector) < 0) {
		for (i = 0; i < ncoord; i ++)
			if ((curloc[i] < lbounds[i]) | (curloc[i] > ubounds[i]))
				return (-2);
		return (-1);
	}
	else
		return 0;
}

void vcbMatPCA(float * convmat, int n, float * eigenvectors, float * eigenvals)
{
	float ** v, **a;
	int      i, j;

	a = matrix(1, n, 1, n);
	v = matrix(1, n, 1, n);
	for (i = 0; i < n; i ++)
		for (j = 0; j < n; j ++)
			a[i+1][j+1] = convmat[i*n+j];

	svdcmp(a, n, n, eigenvals-1, v);

	for (i = 0; i < n; i ++)
		for (j = 0; j < n; j ++)
			eigenvectors[i*n+j] = a[j+1][i+1];

	free_matrix(a, 1,n, 1, n);
	free_matrix(v, 1,n, 1, n);
}

/* internal variables, functions used by vcbDataPCA start here */
/* all are static */
static unsigned char  * ubnp;
static unsigned short * usnp;
static unsigned int   * uinnp;
static char           *  bnp;
static short          *  snp;
static int            *  innp;
static float          *  fnp;
static double         *  dnp;

static float val_ub(int i) { return (float)(ubnp[i]); }
static float val_us(int i) { return (float)(usnp[i]); }
static float val_ui(int i) { return (float)(uinnp[i]); }
static float val_b (int i) { return (float)(bnp[i]); }
static float val_s (int i) { return (float)(snp[i]); }
static float val_i (int i) { return (float)(innp[i]); }
static float val_f (int i) { return (fnp[i]); }
static float val_d (int i) { return (float)(dnp[i]); }
static float (* getval) (int i);

static void datais(void * datain, vcbdatatype type)
{
	switch (type) {
		case VCB_UNSIGNEDBYTE:     /* 8  bits unsigned integer */
			ubnp = (unsigned char *) datain;
			getval = val_ub;
			break;
		case VCB_BYTE:             /* 8  bits integer */
			bnp  = (char *) datain;
			getval = val_b;
			break;
		case VCB_UNSIGNEDSHORT:    /* 16 bits unsigned short integer */
			usnp = (unsigned short *) datain;
			getval = val_us;
			break;
		case VCB_SHORT:            /* 16 bits short integer */
			snp = (short *) datain;
			getval = val_s;
			break;
		case VCB_UNSIGNEDINT:      /* 32 bits unsigned binary integer */
			uinnp = (unsigned int *) datain;
			getval = val_ui;
			break;
		case VCB_INT:              /* 32 bits binary integer */
			innp = (int *) datain;
			getval = val_i;
			break;
		case VCB_FLOAT:            /* 32 bits floating point values */
			fnp = (float *) datain;
			getval = val_f;
			break;
		case VCB_DOUBLE:           /* 64 bits floating point values */
			dnp = (double *) datain;
			getval = val_d;
			break;
		default:
			ubnp = (unsigned char *) datain;
			getval = val_ub;
			break;
	}
}

typedef struct eigensort{
	float eigenval;
	int   which;
} _eign;

static int cmpfunc(const void *x, const void *y)
{
   _eign * pp, * qq;

   pp = (_eign *) x;
   qq = (_eign *) y;

   if (pp->eigenval <  qq->eigenval) return 1;
   if (pp->eigenval == qq->eigenval) return 0;

   return (-1);
}

/* end of internal variables, functions used by vcbDataPCA */

/* vcbDataPCA starts here */
int vcbDataPCA(void * datain, vcbdatatype type, int npnts, int nattribs, float * means, 
				  float * eigenvectors, float * eigenvals)
{
	int i, j, k, cvmsz, idx;
	float * cvm; /* the covariance matrix */
	float * tmpvec;
	_eign * eigns;

	cvmsz = nattribs * nattribs;
	if ((cvm = (float *) calloc (cvmsz, sizeof(float))) == NULL) {
		perror("vcbDataPCA can't allocate covariance matrix with this many attributes");
		return (-1);
	}

	datais(datain, type);

	/* compute the mean for each attribute */
	for (j = 0; j < nattribs; means[j] = 0.f, j ++);

	for (i = 0, idx = 0; i < npnts; i ++, idx+= nattribs)
		for (j = 0; j < nattribs; j ++)
			means[j] += getval(idx+j);

	for (j = 0; j < nattribs; means[j] /= npnts, j ++);
	/* done computing the mean */

	/* start to build the covariance matrix */
	for (i = 0; i < npnts; i ++) {
		idx = i * nattribs;
		for (j = 0; j < nattribs; j ++)
			for (k = 0; k < nattribs; k ++)
				cvm[j*nattribs + k] += (getval(idx+j) - means[j]) * (getval(idx+k) - means[k]);
	}
		
	for (i = 0; i < cvmsz; cvm[i] /= (npnts - 1.f), i ++);
	/* done building the covariance matrix */

	/* compute PCA using singular value decomposition */
	tmpvec = (float *)  malloc (cvmsz * sizeof(float));
	vcbMatPCA(cvm, nattribs, tmpvec, eigenvals);
	/* done computing PCA */

	/* let's sort eigenvalues to descending order using quicksort */
	eigns = (_eign *) malloc(nattribs * sizeof(_eign));
	for (j = 0; j < nattribs; j ++) {
		eigns[j].eigenval = eigenvals[j];
		eigns[j].which = j;
	}
	qsort(eigns, nattribs, sizeof(_eign), cmpfunc);
	for (j = 0; j < nattribs; j ++) {
		memcpy(&eigenvectors[j*nattribs], &tmpvec[eigns[j].which*nattribs], nattribs*sizeof(float));
		eigenvals[j] = eigns[j].eigenval;
	}
	/* done with sorting */

	free(tmpvec);
	free(cvm);
	return 0;
}

int vcbExternalDataPCA(pca_dataset *datain, int nattribs, vcbdatatype type, float *means, float* eigenvectors, float* eigenvals) {
	int i, j, k, cvmsz, dims, dattribs, dattribs2, npnts, npnts2;
	float * cvm; /* the covariance matrix */
	float * tmpvec;
	float a;
	void *d1, *d2;
	_eign * eigns;
	int dsize[100];

	cvmsz = nattribs * nattribs;
	if ((cvm = (float *) calloc (cvmsz, sizeof(float))) == NULL) {
		perror("vcbDataPCA can't allocate covariance matrix with this many attributes");
		return (-1);
	}
	
	for (j = 0; j < nattribs; means[j] = 0.f, j ++);

	/* compute the mean for each attribute */
	for (j = 0; j < nattribs; j ++) {
		d1 = vcbReadBinf(datain[j].filename, type, vcbBigEndian(), &dims, dsize, &dattribs);

		if(!j) { 
			for(npnts=1,k=0;k<dims;k++) npnts *= dsize[k]; 
		} else {
			for(npnts2=1,k=0;k<dims;k++) npnts2 *= dsize[k]; 
			if (npnts2-npnts) {
				perror("dimensions of datasets do not match");
				return (-1);
			}
		}
		if (datain[j].attrib >= dattribs) {
			perror("attribute number out of vcbRange");
			return (-1);
		}

		datais(d1, type);
		for (i=0;i<npnts;i++) means[j] += getval(dattribs*i);

		free(d1);
	}
	for (j = 0; j < nattribs; means[j] /= npnts, j ++);

	/* done computing the mean */

	/* start to build the covariance matrix */
	for (j = 0; j < nattribs; j ++) {
		d1 = vcbReadBinf(datain[j].filename, type, vcbBigEndian(), &dims, dsize, &dattribs);
		for(k = j; k < nattribs; k++) {
			if(k-j) {
				d2 = vcbReadBinf(datain[k].filename, type, vcbBigEndian(), &dims, dsize, &dattribs2);
			} else {
				d2 = d1; 
				dattribs2 = dattribs;
			}

			for (i = 0; i < npnts; i ++) {
				datais(d1, type);
				a = getval(i*dattribs);
				datais(d2, type);
				cvm[j*nattribs + k] += (a - means[j]) * (getval(i*dattribs2) - means[k]);
			}

			if(k-j) free(d2);
		}
		free(d1);
	}

	for(j=1;j<nattribs;j++) {
		for(k=0;k<j;k++) {
			cvm[j*nattribs + k] = cvm[k*nattribs +j];
		}
	}

	for (i = 0; i < cvmsz; cvm[i] /= (npnts - 1.f), i ++);
	/* done building the covariance matrix */
/*
	for (j = 0; j < nattribs; j ++) {
		for (k = 0; k < nattribs; k ++) {
			printf("% 4.5f  ", cvm[j*nattribs+k]);
		}
		printf("\n");
	}
*/
	/* compute PCA using singular value decomposition */
	tmpvec = (float *)  malloc (cvmsz * sizeof(float));
	vcbMatPCA(cvm, nattribs, tmpvec, eigenvals);
	/* done computing PCA */

	/* let's sort eigenvalues to descending order using quicksort */
	eigns = (_eign *) malloc(nattribs * sizeof(_eign));
	for (j = 0; j < nattribs; j ++) {
		eigns[j].eigenval = eigenvals[j];
		eigns[j].which = j;
	}
	qsort(eigns, nattribs, sizeof(_eign), cmpfunc);
	for (j = 0; j < nattribs; j ++) {
		memcpy(&eigenvectors[j*nattribs], &tmpvec[eigns[j].which*nattribs], nattribs*sizeof(float));
		eigenvals[j] = eigns[j].eigenval;
	}
	/* done with sorting */

	free(tmpvec);
	free(cvm);
	return 0;
}


/* 
 * start of new addition in vcbmath for the vcbInterpolate module 
 */

static struct _kernelsetup {
	float radius;
	double (* kfunc) (double);
} kernel_lut[4] = {{2.0f, gaussian}, {1.556f, crawfisian}, {2.f, vcbGaussWinSinc}, {0.f, linear}};


float vcbInterpolate(void * data, vcbdatatype type, int * dsizes, int ndims, int nattr, 
					 float * coords, int whichattrib, vcbKernelType ktype,
					 int (* indexfunc)(int, int *, int *))
/*
 * vcbInterpolate is supposed to be a common interface for high quality interpolation
 * donot use for any code that requires interactivity. the performance offered by it
 * is not sufficient
 *
 * inputs:
 *    data  : a point towards the first byte of the volume dataset
 *    ndims : how many dimensions does the dataset have
 *    dsizes: an integer array describing how many voxels there are in each dimension
 *    nattr : on each voxel, how many quantities/attributes are stored
 *            e.g. in the storage, the sequence of storage should be
 *                V0_a0, V0_a1, V0_a2, ... V0_an, V1_a0 ... V1_an, V2_a0 ... V2_an ...
 *
 *    type  : all quantities/attributes must be of the same type. valid types here are
 *            the regular VR_ types as specified in vcbutils.h, e.g. VCB_UNSIGNEDBYTE 
 *            or VCB_FLOAT, etc.
 *  
 *    if you do not index your volume using the VR convention, please provide a indexfunction.
 *    otherwise, simply give NULL for indexfunc. the default index function used is as the 
 *    following:
 *
 *    int vcbDefaultIndex(int ndims, int * dsizes, int * coords)
 *    {
 *      	int i, id = coords[0];
 *	        for (i = 1; i < ndims; i ++)
 *		        id = id*dsizes[i] + coords[i];
 *
 *	        return id;
 *    }
 *
 *    if using a different indexing scheme, an indexing function using this exact interface 
 *    must be provided.
 *
 *  coords   : an array of floating point describing which data point needs to be interpolated
 *  whichattr: which one of among all the quantities need to be interpolated, valid values
 *             are: [0, nattr-1]
 *  ktype    : which interpolation kernel to use, valid choices are:
 *                     	VCB_GAUSSIAN,    &#47;* gaussian kernel*\/
 *	                    VCB_CRAWFISIAN,  &#47;* crawfis kernel*\/
 *  	                VCB_GAUSSWSINC,  &#47;* gaussian windowed vcbSinc*\/
 *                      VCB_LINEAR       &#47;* linear interpolation*\/
 *             for data smoothing, gaussian is recommended
 *             for most smooth interpolation results, gaussin windowed vcbSinc is recommended
 *             for speed, linear is recommended
 *
 *  return value:
 *        a floating point of the interpolated value
 *
 *  Sample program:
 *        unsigned char * voxels;
 *        int             sz[3], nvoxels;
 *        float testval;
 *        float coord[3];
 * 
 *        sz[0] = sz[1] = sz[2] = 15;
 *        nvoxels = sz[0]*sz[1]*sz[2];
 *        coord[0] = coord[1] = coord[2] = 3.7f;
 *
 *        voxels = (unsigned char *) malloc (nvoxels*sizeof(unsigned char));
 *        memset(voxels, 15, nvoxels);  &#47;* setting all voxels to 15*\/
 *
 *        &#47;* using gaussian*\/
 *        testval = vcbInterpolate(voxels,VCB_UNSIGNEDBYTE,sz, 3, 1, coord, 0, VCB_GAUSSIAN, NULL);
 *        printf("%f ",testval);
 *       
 *        in this case, testval = 15.049
 *
 */
{
#ifndef _VCB__MNA
#define _VCB__MNA 4 /* Maximal Number of Axes */
#endif
	int    i, j, k, idx, cnt, tabsz, total;
	float  voxel;
	int    lbounds[_VCB__MNA], ubounds[_VCB__MNA], sz[_VCB__MNA], voxidx[_VCB__MNA];
	float *pptr[_VCB__MNA];
	float  val, dist, kradius, * parts, weight;
	double (*kerfunc) (double);
	VCB_indexfunc ifunc;

	if (ndims > _VCB__MNA || ndims < 1) {
		fprintf(stderr,"vcbInterpolate handles 1~%d dimensions, %d is not right\n",_VCB__MNA,ndims);
		exit(-1);
	}

	if (data == NULL) {
		fprintf(stderr,"vcbInterpolate is given NULL pointer to data\n");
		exit(-1);
	}

	for (i = 0; i < ndims; i ++) {
		if ((coords[i] < 0) || (coords[i] > (dsizes[i]-1)))
			return 0.f;
	}

	ifunc = vcbDefaultIndex;
	if (indexfunc != NULL) ifunc = indexfunc;
	datais(data, type);
	kerfunc = kernel_lut[ktype].kfunc;
	kradius = kernel_lut[ktype].radius;

	for (i = 0, total = 1, tabsz = 0; i < ndims; i ++) {
		lbounds[i] = (int)floor(coords[i] - kradius);
		if (lbounds[i] < 0) lbounds[i] = 0;

		ubounds[i] = (int)ceil (coords[i] + kradius);
		if (ubounds[i] > (dsizes[i] - 1)) ubounds[i] = dsizes[i] - 1;

		sz[i] = ubounds[i] - lbounds[i] + 1;
		total *= sz[i];
		tabsz += sz[i];
	}

	if (ktype >= VCB_GAUSSWSINC) {
		parts = (float *) malloc(tabsz*sizeof(float));

		for (i = 0, cnt = 0; i < ndims; i ++) {
			pptr[i] = &parts[cnt];
			cnt    += sz[i];
			for (j = lbounds[i], k = 0; j <= ubounds[i]; j ++, k ++) {
				dist = j - coords[i];
				pptr[i][k] = (float)kerfunc(dist);
			}
		}
	}
		
	for (i = 0, val = 0.f; i < total; i ++) {	
		for (j = ndims-1, idx = i; j > 0; j --) {	
			voxidx[j] = idx % sz[j] + lbounds[j];
			idx /= sz[j];	
		}
		voxidx[0] = idx + lbounds[0];
		idx = ifunc(ndims, dsizes, voxidx);
		voxel = getval(idx * nattr + whichattrib);
		
		if (ktype >= VCB_GAUSSWSINC)
			for (j = 0, weight = 1.f; j < ndims; weight *= pptr[j][voxidx[j]-lbounds[j]], j ++);
		else {
			for (j = 0, weight = 0.f; j < ndims; dist = voxidx[j]-coords[j], weight += dist*dist, j++);
			weight = (float)kerfunc(sqrt(weight));
		}
		val += voxel * weight;
	}

	if (ktype >= VCB_GAUSSWSINC)
		free(parts);

	return val;
#undef _VCB__MNA
}

/*
 * end of new addition for the vcbInterpolate module
 */
