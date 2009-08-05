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

#ifndef _VCB_MATH_H
#define _VCB_MATH_H

#include "vcbutils.h"
/** Enum
 * Name: vcbKernelType
 * Version: vcbmntree.h
 * Description:
 *  typedefined&#58; vcbKernelType <br>
 *  vcbKernelType is an enumerator that the MR code uses to describe the type of
 *  data contained within the kernel.
 * Elements:
 *  VCB_GAUSSIAN:    gaussian kernel
 *  VCB_CRAWFISIAN:  crawfis kernel 
 *	VCB_GAUSSWSINC:  gaussian windowed sinc
 *	VCB_LINEAR:      linear interpolation
 **/
typedef enum _vcbmath_reconkernel {
	VCB_GAUSSIAN,    /* gaussian kernel */
	VCB_CRAWFISIAN,  /* crawfis kernel */
	VCB_GAUSSWSINC,  /* gaussian windowed sinc */
	VCB_LINEAR       /* linear interpolation */
} vcbKernelType;

#ifdef __cplusplus
extern "C" {
#endif

/** Function
 * Name: gaussian 
 * Version: vcbmntree.h
 * Description: A legacy guassian function for splatting first developed by Klaus Mueller. 
 * The most commonly used Gaussian kernel radius is 2.0 in object space, 
 * with each voxel's size being 1.0 (assuming a uniform grid).
 * Arguments:
 *	double r: The radius, must be positive.
 * Return: double; The kernel value for the radius, the range is [0,1]
 * Known Issues: None
 **/
double gaussian(double r);

/** Function
 * Name: crawfisian
 * Version: vcbmntree.h
 * Description: The Crawfisian kernel as defined in the Textured Splats paper in
 * Proc. of IEEE Visualization 1993, by Roger Crawfis
 * Arguments:
 *  double r: The radius, must be positive
 * Return: double; The kernel value for that radius, the range is [0,1]
 * Known Issues: None
 **/
double crawfisian( double r);

/** Function
 * Name: vcbGauss
 * Version: vcbmntree.h
 * Description: The general purpose gaussian function 2^(-(x^2)/(s^s))
 * Arguments:
 *  double x: The x value
 *  double s: The s value
 * Return: double; A value in the range of [0,1]
 * Known Issues: None
 **/
double vcbGauss(double x, double s);

/** Function
 * Name: vcbSinc
 * Version: vcbmntree.h
 * Description: The classic sinc function, vcbSinc(x)
 * Arguments: 
 *  double x: 
 * Return: double; sinc(x), range [0, 1]
 * Known Issues: None
 **/
double vcbSinc(double x);

/** Function
 * Name: vcbGaussWinSinc
 * Version: vcbmntree.h
 * Description: The classic gaussian weighted sinc function 
 * Arguments:
 *  double dist: Distance from the center of the kernel
 * Return: double; gaussian weighted sinc function, 1.0 when dist == 0.0,
 *   tapers to zero after fabs(dist) >= 2.0
 * Known Issues: None
 **/
double vcbGaussWinSinc(double dist);

/** Function
 * Name: vcbNormalize3
 * Version: vcbmntree.h
 * Description: normailize3 normalizes a vector of length three.
 * Arguments:
 *  float* a: A pointer to the first element in the vector array.
 * Return: None
 * Known Issues: None
 **/
void  vcbNormalize3 (float * a);

/** Function
 * Name: vcbDotProd3
 * Version: vcbmntree.h
 * Description: vcbDotProd3 returns the dot product of two vectors of length three.
 * Arguments:
 *  float* a: A pointer to the first element in the first vector array.
 *  float* b: A pointer to the first element in the second vector array.
 * Return: float; dot(a,b)
 * Known Issues: None
 **/
float vcbDotProd3  (float * a, float * b);

/** Function
 * Name: vcbCrossProd3
 * Version: vcbmntree.h
 * Description: vcbCrossProd3 computes the cross product of two vectors of length three.
 * Arguments:
 *  float* a: A pointer to the first element in the first vector array.
 *  float* b: A pointer to the first element in the second vector array.
 *  float* c: The vector obtained from cross product of a and b.
 * Return: None
 * Known Issues: None
 **/
void  vcbCrossProd3(float * a, float * b, float * c);

/** Function
 * Name: vcbAdd3
 * Version: vcbmntree.h
 * Description: vcbAdd3 computes the sum of two vectors.
 * Arguments:
 *  float* a: A pointer to the first element in the first vector array.
 *  float afac: A constant to be multiplied to all the elements of vector "a" 
 *   before the addition is preformed.
 *  float* b: A pointer to the first element in the second vector array.
 *  float bfac: A constant to be multiplied to all the elements of vector "b" 
 *   before the addition is preformed.
 *  float* c: The vector obtained from sum of a and b with their respective constants.
 * Return: None
 * Known Issues: None
 **/
void  vcbAdd3       (float * a, float afac, float *b, float bfac, float *c);

/** Function
 * Name: vcbAvgData1D
 * Version: vcbmntree.h
 * Description: vcbAvgData1D is a averaging function that smooths data.
 *  vcbAvgData1D assumes that both "datain" and "dataout" have already been allocated
 *  space and the two pointers are distinct.
 * Arguments:
 *  float* datain: The input array to be averaged.
 *  int itv: The interval of corresponding values.
 *  int len: The length of the interval.
 *  int kersize: The size of the averaging kernel.
 *  float* dataout: The output array to use.
 * Return: None
 * Known Issues: None
 **/
void vcbAvgData1D (float * datain, int itv, int len, int kersize, float * dataout);

/** Function
 * Name: vcbData1DGradient
 * Version: vcbmntree.h
 * Description: vcbData1DGradient differentiates data by differencing.
 *  vcbData1DGradient assumes that both "datain" and "dataout" have already been allocated
 *  space and the two pointers are distinct.
 * Arguments:
 *  float* datain: The input array to be differentiated.
 *  int itv: The interval of corresponding values.
 *  int len: The length of the interval.
 *  int kersize: The size of the averaging kernel.
 *  float* dataout: The output array to use.
 * Return: None
 * Known Issues: None
 **/
void vcbData1DGradient       (float * datain, int itv, int len, int kersize, float * dataout);

/** Function
 * Name: vcbSmoothData1D
 * Version: vcbmntree.h
 * Description: vcbSmoothData1D smooth one dimenstional data by polynomial fitting.
 *  the 1D data is linearized with "itv" attributes per vertex with the order of the storage
 *  being: attrib_1 attrib_2 ... attrib_itv attrib1 ...
 * Arguments:
 *  float* datain: The array of input coordinates.
 *  int itv: The number of attributes per vertex the data is linearized with.
 *  int len: The length of the input.
 *  int kersize: The size of the window of smoothing.
 *  int order: The order of the polynomial used for smoothing.
 *  float* dataout: The array of output coordinates.
 *  float* dxyz: Derivates in each component.
 * Return: None
 * Known Issues: None
 **/
void vcbSmoothData1D(float * datain, int itv, int len, int kersize, int order,
					    float * dataout, float * dxyz);

/** Function
 * Name: vcbCurvatureTorsion
 * Version: vcbmntree.h
 * Description: Computes the curvature and torsion along a mf1 (manifold one) curve, 
 *  with a polynomial as the underlying smoothing mechanism.
 * Arguments:
 *  float* datain: The array of input coordinates.
 *  int itv: The number of attributes per vertex the data is linearized with.
 *  int len: The length of the input.
 *  int winsize: The size of the window of smoothing.
 *  int order: The order of the polynomial used for smoothing.
 *  float* c_t: The array of output in the form c_t[curve, kappa, torsion, curve, kappa, torsion, ...]
 * Return: None
 * Known Issues: None
 **/
void vcbCurvatureTorsion(float * datain, int itv, int len, int winsize, int order, 
							   float * c_t);

/** Function
 * Name: vcbTurtleGeometry
 * Version: vcbmntree.h
 * Description: vcbTurtleGeometry computes the turtle geometry coefficients for an array of
 *  tangent vectors.
 *  Computes the coefficients as describes by Przemyslaw Prusinkiewicz.
 * Arguments:
 *  int n: The number of tangent vectors.
 *  float* tangents: The array of tangent vectors.
 *  float* Whlu: The output array of turtle geometry coefficients in the form Whlu[Wh, Wl, Wu, Wh, Wl, Wu,...]
 * Return: None
 * Known Issues: None
 **/
void vcbTurtleGeometry(int n, float * tangents, float * Whlu);

/** Function
 * Name: vcbAdvectFlow
 * Version: vcbmntree.h
 * Description: 
 *  vcbAdvectFlow advects a flow in a spatial domain of ncoord dimensions
 *  <p>
 *  Notes-
 *  <p>
 *  1. vcbAdvectFlow only computes the part that advects a flow using an adaptive step size 
 *  algorithm,  namely, adaptive step size 4th order Runga-Cutta. it relies on
 *  get_vector, a function provided by the user to access the actual flow field.
 *  get_vector should take care of indexing, and/or interpolation, if necessary.
 *  <p>
 *  2. a common problem (almost the only problem that occurs) is that the indexing scheme
 *  of the flow field (the volume) is not the same as the coordinate system used by
 *  the advection algorithm. In particular, the difference between right-handed
 *  coordinate system and the left-handed. All physics/math domains use right-handed
 *  coordinate system, but a lot of known volume dataset for flow fields use the
 *  left-handed coordinate system. The reason mostly has  to do with the 
 *  screen-coordinate system common used in computer graphics. The fix to this
 *  problem is quite simple. Just take the volume flow dataset, flip in y-direction
 *  (if you index the flow using [i,j,k], then flip the j-direction).
 *
 * Arguments:
 *  float* curloc: A pointer to a floating point array describing the starting
 *     location of the advection
 *  int ncoord: There should be ncoord floats in the array pointed to by curloc
 *  float* lbounds: pointer to an array describing the lower bound of the domain. 
 *     vcbAdvectFlow returns when the advection has stepped outside the domain
 *  float* ubounds: pointer to an array describing the upper bound of the domain. 
 *     vcbAdvectFlow returns when the advection has stepped outside the domain
 *  float t_sz: The size of step to take in time (advection)
 *  float eps: Error tolerance (epsilon). don't be over-zealous in setting this 
 *     value. 1e-30 would not be possible to achieve in practical scenarios. 
 *     normally, try 1e-3 ~ 1e-5 at most. (Hint&#58 1e-6 is roughly the precision floating
 *     point numbers can provide at most)
 *  float h1: An initial step size to take in the spatial domain (guessed by user).
 *     the adaptive step size alrogithm will use this initial guess as hint, but 
 *     still make the decision itself
 *  float hmin: The minimal step size the algorithm should take. don't be over-zealous
 *     in setting this threshold either. A normal example value would be 1e-4,ij
 *     for instance.
 *  void (*get_vector): A function that grabs the vector (flow) given a coordinate
 *     and time. for a static  flow, one can ignore time, but for time-varying flow,
 *     time should also be used. get_vector should be of this prototype&#58  
 *              void get_vector(float t_val, float * coord, float * vector)
 *     t_val (time) and coord (location) are inputs to get_vector, and vector is the output
 *
 * Return: int; 0 for normal return, 
 *             -1 if the advection has stopped (taking step sizes smaller than hmin),
 *             -2 if the advection has stepped outside the domain.
 *
 * Known Issues: vcbAdvectFlow depends on get_vector to obtain the low flow velocity.
 *     If one is not careful, the get_vector function may easily cause memory problems, possibly
 *     seg fault and/or bus error. If such a problem occurs, be sure to check the user
 *     provided get_vector function as the first priority. vcbAdvectFlow itself does not
 *     access any dataset besides calling get_vector.
 **/
int vcbAdvectFlow(float * curloc, int ncoord, float *lbounds, float * ubounds, float t_sz, float eps, float h1, float hmin, void (*get_vector)(float, float *, float *));

/** Function
 * Name: vcbMatPCA
 * Version: vcbmntree.h
 * Description:
 *  vcbMatPCA computes the Principal Component Analysis of a given covariance matrix
 *  Note- vcbMatPCA computes the eigen system using singular value decomposition (SVD).
 * Arguments:
 *  float* covmat: Input. Covariance matrix (a pointer to a linear array of floating point numbers)
 *  int n: Input. The covariance matrix is of size n by n
 *      among the n^2 elements in covmat, the storage sequence is row major order.
 *  float* eigenvectors: Output. Pointer to a linear array of floating point numbers describing the
 *      n eigenvectors (principal components) extracted from covmat. eigenvectors
 *      is of length n, with eigenvector_0, 1, 2, 3 ... n.<br>
 *      Space must already allocated by the calling function.
 *  float* eigenvals: Output. Pointer to a linear array of n floating point numbers
 *      the eigen value corresponding to each eigen vector.<br>
 *      Space must already allocated by the calling function.
 *
 *     eigenvectors and eigenvals must be space already allocated by the calling function
 * Return: None
 * Known Issues: None
 **/
void vcbMatPCA(float * covmat, int n, float * eigenvectors, float * eigenvals);

/** Function
 * Name: vcbDataPCA
 * Version: vcbmntree.h
 * Description:
 *  vcbDataPCA is a utility function that conveniently computes principal components of
 *     a list of data points (each data point has a number of attributes)
 *  Note- The eigenvalues and eigenvectors are sorted according to descending order
 *     of the eigen values.
 * Arguments:
 *  void* datain: Input. A pointer to the list of data points.
 *      the sequence of storage for datain is (p0_1 stands for attribute #1 of point 0)
 *      p0_1 p0_2 ... p0_nattribs p1_1 p1_2 ... p1_nattribs p2_1 ...
 *  vcbdatatype type: Input. The data type of each attribute on each data point. all 
 *      attributes must be of the same type.
 *  int npnts: Input. The number of data points.
 *  int nattribs: Input. The number of attributes on each data point.
 *
 *  float* means: Output. A pointer to an array of npnts floating point numbers. 
 *      This space must be allocated by the calling function. on output, it
 *      contains the mean value of each attribute averaged across all data points.
 *  float* eigenvectors: Output. Pointer to a linear array of floating point numbers
 *      describing the n eigenvectors (principal components) extracted from 
 *      covmat. eigenvectors is of length n, with eigenvector_0, 1, 2, 3 ... n<br>
 *      Space must already allocated by the calling function.
 *  float* eigenvals: Output. Pointer to a linear array of n floating point numbers
 *      the eigen value corresponding to each eigen vector.<br>
 *      Space must already allocated by the calling function.     
 * Return: int; 0 is successful, -1 if error
 * Known Issues: None
 **/
int vcbDataPCA(void * datain, vcbdatatype type, int npnts, int nattribs, float * means, float * eigenvectors, float * eigenvals);

typedef struct {
	char filename[300];
	int attrib;
} pca_dataset;

int vcbExternalDataPCA(pca_dataset *datain, int nattribs, vcbdatatype type, float *means, float* eigenvectors, float* eigenvals);

/** Function
 * Name: vcbInterpolate
 * Version: vcbmntree.h
 * Description:
 *  vcbInterpolate is supposed to be a common interface for high quality interpolation.
 *  Do not use for any code that requires interactivity as the performance offered is not sufficient.
 * Arguments:
 *    void* data: a point towards the first byte of the volume dataset
 *    vcbdatatype type: all quantities/attributes must be of the same type. valid types here are
 *            the regular VR_ types as specified in vcbutils.h, e.g. VCB_UNSIGNEDBYTE 
 *            or VCB_FLOAT, etc.
 *    int* dsizes: an integer array describing how many voxels there are in each dimension
 *    int ndims: how many dimensions does the dataset have
 *    unt nattr: on each voxel, how many quantities/attributes are stored
 *			<p>
 *          e.g. in the storage, the sequence of storage should be&#58;<br>
 *                V0_a0, V0_a1, V0_a2, ... V0_an, V1_a0 ... V1_an, V2_a0 ... V2_an ...
 *		<p>
 *    If you do not index your volume using the VCB convention, please provide an index function
 *		using the same interface as vcbDefaultIndex.
 *    Otherwise, simply give NULL for indexfunc to use vcbDefaultIndex.
 *  float* coords: an array of floating point describing which data point needs to be interpolated
 *  int whichattr: which one of among all the quantities need to be interpolated, valid values are&#58;
 *		[0, nattr-1]
 *  vcbKernelType ktype: which interpolation kernel to use. See vcbKernelType for valid choices.<br>
 *             For data smoothing, gaussian is recommended.<br>
 *             For most smooth interpolation results, gaussian windowed sinc (vcbSinc) is recommended.<br>
 *             For speed, linear is recommended.<br>
 *	int (* indexfunc)(int, int *, int *): The index function to be used. Pass NULL to use vcbDefaultIndex.
 *  Return: float; A floating point of the interpolated value.
 *	Known Issues: None
 **/
float vcbInterpolate(void * data, vcbdatatype type, int * dsizes, int ndims, int nattr, float * coords, int whichattrib, vcbKernelType ktype, int (* indexfunc)(int, int *, int *));

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
