#include <math.h>
#include "nrutil.h"

#ifndef TOL
#define TOL 1.0e-5      /* Default value for single precision and variables scaled to order unity. */
#endif


#ifndef SWAP
#define SWAP(a,b) {float swap; swap=(a);(a)=(b);(b)=swap;}
#endif

void covsrt(float **covar, int ma, int ia[], int mfit)
/* Expand in storage the covariance matrix covar so as to take into account 
parameters that are being held .xed.(For the latter,return zero covariances.) */
{

	int i,j,k;

	for (i=mfit+1;i<=ma;i++)
		for (j=1;j<=i;j++) 
			covar[i][j]=covar[j][i]=0.0;

	k=mfit;

	for (j=ma;j>=1;j--) {
		if (ia[j]) {
			for (i=1;i<=ma;i++) SWAP(covar[i][k],covar[i][j])
				for (i=1;i<=ma;i++) SWAP(covar[k][i],covar[j][i])
					k--;
		}
	}
}

void gaussj(float **a, int n, float **b, int m)
/*
Linear equation solution by Gauss-Jordan elimination, equation (2.1.1) above. a[1..n][1..n]
is the input matrix. b[1..n][1..m] is input containing the m right-hand side vectors. On
output, a is replaced by its matrix inverse, and b is replaced by the corresponding set of solution
vectors.
*/
{

	int *indxc,*indxr,*ipiv;
	int i,icol,irow,j,k,l,ll;
	float big,dum,pivinv;
	indxc=ivector(1,n);  /*The integer arrays ipiv, indxr,andindxc are used for bookkeeping on the pivoting. */
	indxr=ivector(1,n);
	ipiv=ivector(1,n);

	for (j=1;j<=n;j++) ipiv[j]=0;

	for (i=1;i<=n;i++) { /*This is the main loop over the columns to be reduced. */
		big=0.0;
		for (j=1;j<=n;j++) /*This is the outer loop of the search for a pivot element. */ 
			if (ipiv[j] != 1)
				for (k=1;k<=n;k++) {
					if (ipiv[k] == 0) {
						if (FABS(a[j][k]) >= big) {
							big  = FABS(a[j][k]);
							irow = j;
							icol = k;
						}
					}
				}

				++(ipiv[icol]);

/*
We now have the pivot element, so we interchange rows, if needed, to put the pivot
element on the diagonal. The columns are not physically interchanged, only relabeled:
indxc[i],thecolumnoftheith pivot element, is the ith column that is reduced, while
indxr[i] is the row in which that pivot element was originally located. If indxr[i] !=
indxc[i] there is an implied column interchange. With this form of bookkeeping, the
solution b’s will end up in the correct order, and the inverse matrix will be scrambled
by columns.
*/

				if (irow != icol) {
					for (l=1;l<=n;l++) SWAP(a[irow][l],a[icol][l])
					for (l=1;l<=m;l++) SWAP(b[irow][l],b[icol][l])
				}

				indxr[i]=irow; /*We are now ready to divide the pivot row by the pivot element, located at irow and icol.*/
				indxc[i]=icol;

				if (a[icol][icol] == 0.0) nrerror("gaussj: Singular Matrix");
				pivinv=1.0f/a[icol][icol];
				a[icol][icol]=1.0;

				for (l=1;l<=n;l++) a[icol][l] *= pivinv;
				for (l=1;l<=m;l++) b[icol][l] *= pivinv;
				for (ll=1;ll<=n;ll++)  /* Next, we reduce the rows... */
					if (ll != icol) {  /*...except for the pivot one, of course. */
						dum=a[ll][icol];
						a[ll][icol]=0.0;
						
						for (l=1;l<=n;l++) a[ll][l] -= a[icol][l]*dum;
						for (l=1;l<=m;l++) b[ll][l] -= b[icol][l]*dum;
					}
	}

/*
This is the end of the main loop over columns of the reduction. It only remains to unscram-ble
the solution in view of the column interchanges. We do this by interchanging pairs of
columns in the reverse order that the permutation was built up.
*/

	for (l=n;l>=1;l--) {
		if (indxr[l] != indxc[l])
			for (k=1;k<=n;k++) SWAP(a[k][indxr[l]],a[k][indxc[l]]);

	} /* And we are done. */

	free_ivector(ipiv,1,n);
	free_ivector(indxr,1,n);
	free_ivector(indxc,1,n);
}


void lfit(float x[], float y[], float sig[], int ndat, float a[], int ia[],
		  int ma, float **covar, float *chisq, void (*funcs)(float, float [], int))
/*
Given a set of data points x[1..ndat] y[1..ndat] with individual standard deviations
sig[1..ndat],use chi^2 minimization to fit for some or all of the coefficients a[1..ma] 
of a function that depends linearly on a y = sum (a_i*afunc_i(x). The input array 
ia[1..ma] indicates by nonzero entries those components of a that should be fitted for 
and by zero entries those components that should be held fixed at their input values. 
The program returns values for a[1..ma], chi^2 = chisq and the covariance matrix 
covar[1..ma][1..ma].(Parameters held .xed will return zero covariances.) The user 
supplies a routine funcs(x,afunc,ma) that returns the ma basis functions evaluated 
at x = x in the array afunc[1..ma].
*/
{
	void covsrt(float **covar, int ma, int ia[], int mfit);
	void gaussj(float **a, int n, float **b, int m);

	int i,j,k,l,m,mfit=0;
	float ym,wt,sum,sig2i,**beta,*afunc;

	beta=matrix(1,ma,1,1);
	afunc=vector(1,ma);

	for (j=1;j<=ma;j++) if (ia[j]) mfit++;
		
	if (mfit == 0) nrerror("lfit: no parameters to be fitted");

	for (j=1;j<=mfit;j++) {  /*Initialize the (symmetric)matrix. */
		for (k=1;k<=mfit;k++) covar[j][k]=0.0;
		beta[j][1]=0.0;
	}

	for (i=1;i<=ndat;i++) { /*Loop over data to accumulate coeffcients 
		                    of the normal equations. */

		(*funcs)(x[i],afunc,ma);
		ym=y[i];

		if (mfit < ma) {  /* Subtract off dependences on known pieces of the fitting function. */
			for (j=1;j<=ma;j++)
				if (!ia[j]) ym -= a[j]*afunc[j];
		}

		sig2i=(float)(1.0f/SQR(sig[i]));

		for (j=0,l=1;l<=ma;l++) {
			if (ia[l]) {
				wt=afunc[l]*sig2i;
				for (j++,k=0,m=1;m<=l;m++)
					if (ia[m]) covar[j][++k] += wt*afunc[m];
					beta[j][1] += ym*wt;
			}
		}
	}

	for (j=2;j<=mfit;j++) /*Fill in above the diagonal from symmetry.*/
		for (k=1;k<j;k++)
			covar[k][j]=covar[j][k];

	gaussj(covar,mfit,beta,1); /*Matrix solution.*/
	for (j=0,l=1;l<=ma;l++)
		if (ia[l]) a[l]=beta[++j][1]; /*Partition solution to appropriate coefficients a*/

	*chisq=0.0;

	for (i=1;i<=ndat;i++) { /*Evaluate chisq of the fit*/
		(*funcs)(x[i],afunc,ma);
		for (sum=0.0,j=1;j<=ma;j++) sum += a[j]*afunc[j];
		*chisq += (float)SQR((y[i]-sum)/sig[i]);
	}

	covsrt(covar,ma,ia,mfit); /*Sort covariance matrix to true order of fitting coeff*/
	free_vector(afunc,1,ma);
	free_matrix(beta,1,ma,1,1);
}


float pythag(float a, float b)
/* Computes (a2 + b2)1/2 without destructive underflow or overflow. */
{
	float absa,absb;
	absa= FABS(a);
	absb= FABS(b);
	if (absa > absb) return (float) (absa*sqrt(1.0+SQR(absb/absa)));
	else return (absb == 0.0 ? 0.0f : (float) (absb*sqrt(1.0+SQR(absa/absb))));
}

void svdcmp(float **a, int m, int n, float w[], float **v)
/*
Given a matrix a[1..m][1..n], this routine computes its singular value decomposition, A =
U·W·V T. Thematrix U replaces a on output. The diagonal matrix of singular values W is output
as a vector w[1..n]. Thematrix V (not the transpose V T ) is output as v[1..n][1..n].
*/
{
	float pythag(float a, float b);
	int flag,i,its,j,jj,k,l,nm;
	float anorm,c,f,g,h,s,scale,x,y,z,*rv1;
	rv1=vector(1,n);
	g=scale=anorm=0.0;        /*Householder reduction to bidiagonal form.*/


	for (i=1;i<=n;i++) {
		l=i+1;
		rv1[i]=scale*g;
		g=s=scale=0.0;

		if (i <= m) {
			for (k=i;k<=m;k++) scale += FABS(a[k][i]);
			if (scale) {
				for (k=i;k<=m;k++) {
					a[k][i] /= scale;
					s += a[k][i]*a[k][i];
				}

				f=a[i][i];
				g = -SIGN(sqrt((double)s),f);
				h=f*g-s;
				a[i][i]=f-g;

				for (j=l;j<=n;j++) {
					for (s=0.0,k=i;k<=m;k++) s += a[k][i]*a[k][j];
					f=s/h;
					for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
				}

				for (k=i;k<=m;k++) a[k][i] *= scale;
			}
		}

		w[i]=scale *g;
		g=s=scale=0.0;

		if (i <= m && i != n) {
			for (k=l;k<=n;k++) scale += FABS(a[i][k]);
			if (scale) {
				for (k=l;k<=n;k++) {
					a[i][k] /= scale;
					s += a[i][k]*a[i][k];
				}

				f=a[i][l];
				g = -SIGN(sqrt((double)s),f);
				h=f*g-s;
				a[i][l]=f-g;

				for (k=l;k<=n;k++) rv1[k]=a[i][k]/h;
				for (j=l;j<=m;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[j][k]*a[i][k];
					for (k=l;k<=n;k++) a[j][k] += s*rv1[k];
				}

				for (k=l;k<=n;k++) a[i][k] *= scale;
			}
		}

		anorm = FMAX(anorm,FABS(w[i])+FABS(rv1[i]));
	}
	
	for (i=n;i>=1;i--) {      /* Accumulation of right-hand transformations. */
		if (i < n) {
			if (g) {
				for (j=l;j<=n;j++) /* Double division to avoid possible underflow. */
					v[j][i]=(a[i][j]/a[i][l])/g;

				for (j=l;j<=n;j++) {
					for (s=0.0,k=l;k<=n;k++) s += a[i][k]*v[k][j];
					for (k=l;k<=n;k++) v[k][j] += s*v[k][i];
				}
			}

			for (j=l;j<=n;j++) v[i][j]=v[j][i]=0.0;
		}

		v[i][i]=1.0;
		g=rv1[i];
		l=i;
	}

	for (i=IMIN(m,n);i>=1;i--) { /* Accumulation of left-hand transformations. */
		l=i+1;
		g=w[i];
		for (j=l;j<=n;j++) a[i][j]=0.0;

		if (g) {
			g=1.0f/g;
			for (j=l;j<=n;j++) {
				for (s=0.0,k=l;k<=m;k++) s += a[k][i]*a[k][j];
				f=(s/a[i][i])*g;
				for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
			}

			for (j=i;j<=m;j++) a[j][i] *= g;
		} 
		else for (j=i;j<=m;j++) a[j][i]=0.0;
		++a[i][i];
	}

	for (k=n;k>=1;k--) { /* Diagonalization of the bidiagonal form: Loop over 
							singular values, and over allowed iterations. */
		for (its=1;its<=30;its++) {
			flag=1;
			
			for (l=k;l>=1;l--) { /* Test for splitting. */
				nm=l-1;          /* Note that rv1[1] is always zero. */
				if ((FABS(rv1[l])+anorm) == anorm) {
					flag=0;
					break;
				}

				if ((FABS(w[nm])+anorm) == anorm) break;
			}

			if (flag) {
				c=0.0; /* Cancellation of rv1[l], if l > 1. */
				s=1.0;

				for (i=l;i<=k;i++) {
					f=s*rv1[i];
					rv1[i]=c*rv1[i];
					if ((FABS(f)+anorm) == anorm) break;
					g=w[i];
					h=pythag(f,g);
					w[i]=h;
					h=1.0f/h;
					c=g*h;
					s = -f*h;

					for (j=1;j<=m;j++) {
						y=a[j][nm];
						z=a[j][i];
						a[j][nm]=y*c+z*s;
						a[j][i]=z*c-y*s;
					}
				}
			}

			z=w[k];

			if (l == k) {      /* Convergence. */
				if (z < 0.0) { /* Singular value is made nonnegative. */
					w[k] = -z;
					for (j=1;j<=n;j++) v[j][k] = -v[j][k];
				}
				break;
			}
			
			if (its == 30) 
				nrerror("no convergence in 30 svdcmp iterations");

			x=w[l]; /* Shift from bottom 2-by-2 minor. */
			nm=k-1;
			y=w[nm];
			g=rv1[nm];
			h=rv1[k];
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0f*h*y);

			g=pythag(f,1.0);
			f=(float)((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;

			c=s=1.0;              /* Next QR transformation: */

			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1[i];
				y=w[i];
				h=s*g;
				g=c*g;
				z=pythag(f,h);
				rv1[j]=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g = g*c-x*s;
				h=y*s;
				y *= c;

				for (jj=1;jj<=n;jj++) {
					x=v[jj][j];
					z=v[jj][i];
					v[jj][j]=x*c+z*s;
					v[jj][i]=z*c-x*s;
				}

				z=pythag(f,h);
				w[j]=z;           /*Rotation can be arbitrary if z = 0. */

				if (z) {
					z=1.0f/z;
					c=f*z;
					s=h*z;
				}

				f=c*g+s*y;
				x=c*y-s*g;

				for (jj=1;jj<=m;jj++) {
					y=a[jj][j];
					z=a[jj][i];
					a[jj][j]=y*c+z*s;
					a[jj][i]=z*c-y*s;
				}
			}

			rv1[l]=0.0;
			rv1[k]=f;
			w[k]=x;
		}
	}
	free_vector(rv1,1,n);
}


void svdfit(float x[], float y[], float sig[], int ndata, float a[], 
			int ma,float **u, float **v, float w[], float *chisq,
			void (*funcs)(float, float [], int))
/*
Given a set of data points x[1..ndata],y[1..ndata] with individual standard deviations
sig[1..ndata], use ?2 minimization to determine the coefficients a[1..ma] of the fitting
function y = SUMi (a_i × afunc_i(x)). Here we solve the fitting equations using singular
value decomposition of the ndata by ma matrix, as in §2.6. Arrays u[1..ndata][1..ma],
v[1..ma][1..ma], and w[1..ma] provide workspace on input; on output they define the
singular value decomposition, and can be used to obtain the covariance matrix. The program
returns values for the ma fit parameters a, and chi^2, chisq. The user supplies a routine
funcs(x,afunc,ma) that returns the ma basis functions evaluated at x = x in the array
afunc[1..ma].
*/
{
	void svbksb(float **u, float w[], float **v, int m, int n, float b[], float x[]);
	void svdcmp(float **a, int m, int n, float w[], float **v);

	int j,i;
	float wmax,tmp,thresh,sum,*b,*afunc;
	b=vector(1,ndata);	
	afunc=vector(1,ma);

	for (i=1;i<=ndata;i++) { /* Accumulate coefficients of the fitting matrix. */
		(*funcs)(x[i],afunc,ma);
		tmp=1.0f/sig[i];

		for (j=1;j<=ma;j++) u[i][j]=afunc[j]*tmp;

		b[i]=y[i]*tmp;
	}

	svdcmp(u,ndata,ma,w,v); /* Singular value decomposition. */
	wmax=0.0f;              /* Edit the singular values, given TOL from the
	                           #define statement, between here ... */

	for (j=1;j<=ma;j++)
		if (w[j] > wmax) wmax=w[j];

	thresh=(float)TOL*wmax;
	for (j=1;j<=ma;j++)
		if (w[j] < thresh) w[j]=0.0f;  /* ...and here. */

	svbksb(u,w,v,ndata,ma,b,a);
	*chisq=0.0f;                       /* Evaluate chi-square. */

	for (i=1;i<=ndata;i++) {
		(*funcs)(x[i],afunc,ma);
		for (sum=0.0,j=1;j<=ma;j++) sum += a[j]*afunc[j];
		*chisq += (tmp=(y[i]-sum)/sig[i],tmp*tmp);
	}

	free_vector(afunc,1,ma);
	free_vector(b,1,ndata);
}

void svbksb(float **u, float w[], float **v, int m, int n, float b[], float x[])
/* 
Solves A·X = B for a vector X, where A is specified by the arrays u[1..m][1..n], w[1..n],
v[1..n][1..n] as returned by svdcmp. m and n are the dimensions of a, and will be equal for
square matrices. b[1..m] is the input right-hand side. x[1..n] is the output solution vector.
No input quantities are destroyed, so the routine may be called sequentially with different 
b’s.
*/
{
	int jj,j,i;
	float s,*tmp;

	tmp=vector(1,n);
	for (j=1;j<=n;j++) {         /* Calculate UTB. */
		s=0.f;
		if (w[j]) {              /* Nonzero result only if wj is nonzero. */
			for (i=1;i<=m;i++) s += u[i][j]*b[i];
			s /= w[j];           /* This is the divide by wj . */
		}
		tmp[j]=s;
	}

	for (j=1;j<=n;j++) {         /* Matrix multiply by V to get answer. */
		s=0.f;
		for (jj=1;jj<=n;jj++) s += v[j][jj]*tmp[jj];
		x[j]=s;
	}

	free_vector(tmp,1,n);
}

void svdvar(float **v, int ma, float w[], float **cvm)
/*
To evaluate the covariance matrix cvm[1..ma][1..ma] of the fit for ma parameters obtained
by svdfit, call this routine with matrices v[1..ma][1..ma], w[1..ma] as returned from
svdfit.
*/
{
	int k,j,i;
	float sum,*wti;
	wti=vector(1,ma);
	for (i=1;i<=ma;i++) {
		wti[i]=0.f;
		if (w[i]) wti[i]=1.0f/(w[i]*w[i]);
	}

	for (i=1;i<=ma;i++) {         /* Sum contributions to covariance matrix (15.4.20). */
		for (j=1;j<=i;j++) {
			for (sum=0.f,k=1;k<=ma;k++) sum += v[i][k]*v[j][k]*wti[k];
			cvm[j][i]=cvm[i][j]=sum;
		}
	}

	free_vector(wti,1,ma);
}

void rk4(float y[], float dydx[], int n, float x, float h, float yout[], void (*derivs)(float, float [], float []))
/*
 * Given values for the variables y[1..n] and their derivatives dydx[1..n] known at x, use the
 * fourth-order Runge-Kutta method to advance the solution over an interval h and return the
 * incremented variables as yout[1..n], which need not be a distinct array from y. The user
 * supplies the routine derivs(x,y,dydx), which returns derivatives dydx at x.
 */
{
	int i;
	float xh,hh,h6,*dym,*dyt,*yt;

	dym=vector(1,n);
	dyt=vector(1,n);
	yt=vector(1,n);

	hh=h*0.5f;
	h6=h/6.f;
	xh=x+hh;

	for (i=1;i<=n;i++) yt[i]=y[i]+hh*dydx[i];  /* First step  */
	
	(*derivs)(xh,yt,dyt);                      /* Second step */

	for (i=1;i<=n;i++) yt[i]=y[i]+hh*dyt[i];

	(*derivs)(xh,yt,dym);                      /* Third step */

	for (i=1;i<=n;i++) {
		yt[i]=y[i]+h*dym[i];
		dym[i] += dyt[i];
	}

	(*derivs)(x+h,yt,dyt);                     /* Fourth step */

	for (i=1;i<=n;i++)                         /* Accumulate increments with proper weights */
		yout[i]=y[i]+h6*(dydx[i]+dyt[i]+2.f*dym[i]);

	free_vector(yt,1,n);
	free_vector(dyt,1,n);
	free_vector(dym,1,n);
}

void rkck(float y[], float dydx[], int n, float x, float h, 
		  float yout[], float yerr[], void (*derivs)(float, float [], float []))
/*
 * Given values for n variables y[1..n] and their derivatives dydx[1..n] known at x, use
 * the fifth-order Cash-Karp Runge-Kutta method to advance the solution over an interval h
 * and return the incremented variables as yout[1..n]. Also return an estimate of the local
 * truncation error in yout using the embedded fourth-order method. The user supplies the routine
 * derivs(x,y,dydx), which returns derivatives dydx at x.
 */
{

	int i;
	static float a2=0.2f,a3=0.3f,a4=0.6f,a5=1.0f,a6=0.875f,b21=0.2f,
		b31=3.f/40.f,b32=9.f/40.f,b41=0.3f,b42=-0.9f,b43=1.2f,
		b51=-11.f/54.f,b52=2.5f,b53=-70.f/27.f,b54=35.f/27.f,
		b61=1631.f/55296.f,b62=175.f/512.f,b63=575.f/13824.f,
		b64=44275.f/110592.f,b65=253.f/4096.f,c1=37.f/378.f,
		c3=250.f/621.f,c4=125.f/594.f,c6=512.f/1771.f,
		dc5 = -277.f/14336.f;

	float dc1=c1-2825.f/27648.f,dc3=c3-18575.f/48384.f, dc4=c4-13525.f/55296.f,dc6=c6-0.25f;
	float *ak2,*ak3,*ak4,*ak5,*ak6,*ytemp;

	ak2=vector(1,n);
	ak3=vector(1,n);
	ak4=vector(1,n);
	ak5=vector(1,n);
	ak6=vector(1,n);
	ytemp=vector(1,n);

	for (i=1;i<=n;i++)               /* First step */
		ytemp[i]=y[i]+b21*h*dydx[i];

	(*derivs)(x+a2*h,ytemp,ak2);     /* Second step */

	for (i=1;i<=n;i++)
		ytemp[i]=y[i]+h*(b31*dydx[i]+b32*ak2[i]);

	(*derivs)(x+a3*h,ytemp,ak3);     /* Third step */

	for (i=1;i<=n;i++)
		ytemp[i]=y[i]+h*(b41*dydx[i]+b42*ak2[i]+b43*ak3[i]);

	(*derivs)(x+a4*h,ytemp,ak4);     /* Fourth step */

	for (i=1;i<=n;i++)
		ytemp[i]=y[i]+h*(b51*dydx[i]+b52*ak2[i]+b53*ak3[i]+b54*ak4[i]);

	(*derivs)(x+a5*h,ytemp,ak5);     /* Fifth step */

	for (i=1;i<=n;i++)
		ytemp[i]=y[i]+h*(b61*dydx[i]+b62*ak2[i]+b63*ak3[i]+b64*ak4[i]+b65*ak5[i]);

	(*derivs)(x+a6*h,ytemp,ak6);     /* Sixth step */

	for (i=1;i<=n;i++)               /* Accumulate increments with proper weights */
		yout[i]=y[i]+h*(c1*dydx[i]+c3*ak3[i]+c4*ak4[i]+c6*ak6[i]);

    /* Estimate error as difference between fourth and fifth order methods */
	for (i=1;i<=n;i++)
		yerr[i]=h*(dc1*dydx[i]+dc3*ak3[i]+dc4*ak4[i]+dc5*ak5[i]+dc6*ak6[i]);

	free_vector(ytemp,1,n);
	free_vector(ak6,1,n);
	free_vector(ak5,1,n);
	free_vector(ak4,1,n);
	free_vector(ak3,1,n);
	free_vector(ak2,1,n);
}

#ifndef SAFETY
#define SAFETY 0.9f
#endif
#ifndef PGROW
#define PGROW -0.2f
#endif
#ifndef PSHRNK
#define PSHRNK -0.25f
#endif
#ifndef ERRCON
#ifndef ERRCON
#define ERRCON 1.89e-4f /* the value ERRCON equals (5/SAFETY) raised to the power (1/PGROW), see use below. */
#endif
#endif

void rkqs(float y[], float dydx[], int n, float *x, float htry, float eps, float yscal[], 
		  float *hdid, float *hnext, void (*derivs)(float, float [], float []))
/*
 * Fifth-order Runge-Kutta step with monitoring of local truncation error to ensure accuracya nd
 * adjust stepsize. Input are the dependent variable vector y[1..n] and its derivative dydx[1..n]
 * at the starting value of the independent variable x. Also input are the stepsize to be attempted
 * htry, the required accuracy eps, and the vector yscal[1..n] against which the error is
 * scaled. On output, y and x are replaced bythei r new values, hdid is the stepsize that was
 * actuallyac complished, and hnext is the estimated next stepsize. derivs is the user-supplied
 * routine that computes the right-hand side derivatives.
 */
{
	int i;
	float errmax,h,htemp,xnew,*yerr,*ytemp;

	yerr=vector(1,n);
	ytemp=vector(1,n);
	h=htry;               /* Set stepsize to the initial trial value */

	for (;;) {
		rkck(y,dydx,n,*x,h,ytemp,yerr,derivs);  /* take a step */
		errmax=0.0;                             /* Evaluate accuracy. */

		for (i=1;i<=n;i++) errmax=FMAX(errmax,FABS(yerr[i]/yscal[i]));
		errmax /= eps;                          /* Scale relative to required tolerance. */

		if (errmax <= 1.f) break;               /* Step succeeded. Compute size of next step. */

		htemp=SAFETY*h*((float)pow((double)errmax,(double)PSHRNK));
		/* Truncation error too large, reduce stepsize. */
		h=(h >= 0.f ? FMAX(htemp,0.1f*h) : FMIN(htemp,0.1f*h));  /* No more than a factor of 10 */

		xnew=(*x)+h;

		if (xnew == *x) nrerror("stepsize underflow in rkqs");

	}

	if (errmax > ERRCON) 
		*hnext=SAFETY*h*((float)pow((double)errmax,(double)PGROW));
	else 
		*hnext=5.f*h;                          /* No more than a factor of 5 increase */

	*x += (*hdid=h);
	for (i=1;i<=n;i++) y[i]=ytemp[i];

	free_vector(ytemp,1,n);
	free_vector(yerr,1,n);
}


#ifndef MAXSTP
#define MAXSTP 10000
#endif
#ifndef TINY
#define TINY 1.0e-30f
#endif
static int kmax=0, kount;
static float *xp,**yp,dxsav;
/*
 * User storage for intermediate results. Preset kmax and dxsav in the calling program. If kmax !=
 * 0 results are stored at approximate intervals dxsav in the arrays xp[1..kount], 
 * yp[1..nvar][1..kount], where kount is output by odeint. Defining declarations for these 
 * variables, with memory allocations xp[1..kmax] and yp[1..nvar][1..kmax] for the arrays, 
 * should be in the calling program. kmax, kount, xp, yp and dxsav all should be external
 * variables to save intermediate results for the caller to inspect the correctness of
 * the process.
 *
 * if kmax == 0, then there is no request to store intermediate results. since we need
 * runtime performance and trust the quality of NR code to a quite high level, we always
 * use kmax = 0. in this case, xp and yp don't have to point to valid memory and dxsav
 * can be a random value. to avoid linking problems, we have set kmax, kount, xp, yp and
 * dxsav to static linkage.
 */

int  odeint(float * ystart, int nvar, float x1, float x2, float eps, 
			float h1, float hmin, int *nok, int *nbad, 
			void (*derivs)(float, float *, float *))
/*
 * Runge-Kutta driver with adaptive stepsize control. Integrate starting values ystart[1..nvar]
 * from x1 to x2 with accuracy eps, storing intermediate results in global variables. h1 should
 * be set as a guessed first stepsize, hmin as the minimum allowed stepsize (can be zero). On
 * output nok and nbad are the number of good and bad (but retried and fixed) steps taken, and
 * ystart is replaced byv alues at the end of the integration interval. derivs is the user-supplied
 * routine for calculating the right-hand side derivative, while rkqs is the name of the stepper
 * routine to be used.
 *
 * return value: 0 - normal return, -1 means either too many steps are taken, or too small the
 *               step sizes are (in both cases, the ode integration should stop, since very
 *               likely the field is not doing much or the integration has already stepped outside
 *               the domain)
 */
{
	int nstp,i;
	float xsav,x,hnext,hdid,h;
	float *yscal,*y,*dydx;

	yscal=vector(1,nvar);
	y=vector(1,nvar);
	dydx=vector(1,nvar);

	x=x1;
	h=SIGN(h1,x2-x1);
	*nok = (*nbad) = kount = 0;

	for (i=1;i<=nvar;i++) y[i]=ystart[i];

	if (kmax > 0) xsav=x-dxsav*2.f;           /* Assures storage of first step */

	for (nstp=1;nstp<=MAXSTP;nstp++) {        /* Take at most MAXSTP steps */
		(*derivs)(x,y,dydx);

		for (i=1;i<=nvar;i++) {
			/* scaling used to monitor accuracy. This general-purpose choice can be modified if need be */
			yscal[i]=FABS(y[i]) + FABS(dydx[i]*h)+TINY;
		}

		if (kmax > 0 && kount < kmax-1 && FABS(x-xsav) > FABS(dxsav)) {
			xp[++kount]=x;                     /* store intermediate results */
			for (i=1;i<=nvar;i++) yp[i][kount]=y[i];
			xsav=x;
		}

		if ((x+h-x2)*(x+h-x1) > 0.0) h=x2-x;   /* if stepsize can overshoot, decrease */
		
		rkqs(y,dydx,nvar,&x,h,eps,yscal,&hdid,&hnext,derivs);

		if (hdid == h) ++(*nok); else ++(*nbad);
		if ((x-x2)*(x2-x1) >= 0.0) {           /* are we done? */

			for (i=1;i<=nvar;i++) ystart[i]=y[i];
			if (kmax) {
				xp[++kount]=x;                 /* save final step */
				for (i=1;i<=nvar;i++) yp[i][kount]=y[i];
			}

			free_vector(dydx,1,nvar);
			free_vector(y,1,nvar);
			free_vector(yscal,1,nvar);
			return 0;                            /* normal exit */
		}

		if (FABS(hnext) <= hmin) return (-1); /*nrerror("Step size too small in odeint");*/
		h=hnext;
	}
	
	return (-1); /* nrerror("Too many steps in routine odeint"); */
}
