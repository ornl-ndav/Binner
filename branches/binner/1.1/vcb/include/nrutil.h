/****************************************************************************
 *   Copyright (C) 2004  by Jian Huang										*
 *	 seelab@cs.utk.edu														*
 *																			*
 *   This library is free software; you can redistribute it and/or			*
 *   modify it under the terms of the GNU Lesser General Public				*
 *   License as published by the Free Software Foundation; either			*
 *   version 2.1 of the License, or (at your option) any later version.		*
 *																			*
 *   This library is distributed in the hope that it will be useful,		*
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of			*
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU		*
 *   Lesser General Public License for more details.						*
 *																			*
 *   You should have received a copy of the GNU Lesser General Public		*
 *   License along with this library; if not, write to the					*
 *   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,			*
 *   Boston, MA  02110-1301  USA											*
 ****************************************************************************/

#ifndef _NR_UTILS_H_
#define _NR_UTILS_H_

static float sqrarg;
#ifndef SQR
#define SQR(a) ((sqrarg=(a)) == 0.0 ? 0.0 : sqrarg*sqrarg)
#endif

static double dsqrarg;
#ifndef DSQR
#define DSQR(a) ((dsqrarg=(a)) == 0.0 ? 0.0 : dsqrarg*dsqrarg)
#endif

static double dmaxarg1,dmaxarg2;
#ifndef DMAX
#define DMAX(a,b) (dmaxarg1=(a),dmaxarg2=(b),(dmaxarg1) > (dmaxarg2) ?\
(dmaxarg1) : (dmaxarg2))
#endif

static double dminarg1,dminarg2;
#ifndef DMIN
#define DMIN(a,b) (dminarg1=(a),dminarg2=(b),(dminarg1) < (dminarg2) ?\
(dminarg1) : (dminarg2))
#endif

static float maxarg1,maxarg2;
#ifndef FMAX
#define FMAX(a,b) (maxarg1=(a),maxarg2=(b),(maxarg1) > (maxarg2) ?\
(maxarg1) : (maxarg2))
#endif

static float minarg1,minarg2;
#ifndef FMIN
#define FMIN(a,b) (minarg1=(a),minarg2=(b),(minarg1) < (minarg2) ?\
(minarg1) : (minarg2))
#endif

static long lmaxarg1,lmaxarg2;
#ifndef LMAX
#define LMAX(a,b) (lmaxarg1=(a),lmaxarg2=(b),(lmaxarg1) > (lmaxarg2) ?\
(lmaxarg1) : (lmaxarg2))
#endif

static long lminarg1,lminarg2;
#ifndef LMIN
#define LMIN(a,b) (lminarg1=(a),lminarg2=(b),(lminarg1) < (lminarg2) ?\
(lminarg1) : (lminarg2))
#endif

static int imaxarg1,imaxarg2;
#ifndef IMAX
#define IMAX(a,b) (imaxarg1=(a),imaxarg2=(b),(imaxarg1) > (imaxarg2) ?\
(imaxarg1) : (imaxarg2))
#endif

static int iminarg1,iminarg2;
#ifndef IMIN
#define IMIN(a,b) (iminarg1=(a),iminarg2=(b),(iminarg1) < (iminarg2) ?\
(iminarg1) : (iminarg2))
#endif

static float ftmpval;
#ifndef FABS
#define FABS(a) (ftmpval = (float)(a), (ftmpval > 0) ? ftmpval :  -ftmpval)
#endif

/* #ifndef SIGN
#define SIGN(a,b) ((b) >= 0.0 ? (fabs(a) : -fabs(a)) 
#endif */
static float ftmpsign;
#ifndef SIGN
#define SIGN(a,b) (ftmpsign = FABS(a), (b) >= 0.0 ? ftmpsign : -ftmpsign)
#endif

#ifdef  __cplusplus
extern "C" {
#endif

void nrerror(char error_text[]);
float * vector(long nl, long nh);
int * ivector(long nl, long nh);
unsigned char * cvector(long nl, long nh);
unsigned long * lvector(long nl, long nh);
double * dvector(long nl, long nh);
float ** matrix(long nrl, long nrh, long ncl, long nch);
double ** dmatrix(long nrl, long nrh, long ncl, long nch);
int ** imatrix(long nrl, long nrh, long ncl, long nch);
float ** submatrix(float **a, long oldrl, long oldrh, long oldcl, long oldch,
long newrl, long newcl);
float ** convert_matrix(float *a, long nrl, long nrh, long ncl, long nch);
float *** f3tensor(long nrl, long nrh, long ncl, long nch, long ndl, long ndh);
void free_vector(float *v, long nl, long nh);
void free_ivector(int *v, long nl, long nh);
void free_cvector(unsigned char *v, long nl, long nh);
void free_lvector(unsigned long *v, long nl, long nh);
void free_dvector(double *v, long nl, long nh);
void free_matrix(float **m, long nrl, long nrh, long ncl, long nch);
void free_dmatrix(double **m, long nrl, long nrh, long ncl, long nch);
void free_imatrix(int **m, long nrl, long nrh, long ncl, long nch);
void free_submatrix(float **b, long nrl, long nrh, long ncl, long nch);
void free_convert_matrix(float **b, long nrl, long nrh, long ncl, long nch);
void free_f3tensor(float ***t, long nrl, long nrh, long ncl, long nch,
long ndl, long ndh);

#ifdef  __cplusplus
}
#endif

/* traditional - K&R */
/* we assume it's always ANSI */
/* void nrerror(); */
/* float *vector(); */
/*Rest of traditional declarations are here on the diskette.*/

#endif /* _NR_UTILS_H_ */
