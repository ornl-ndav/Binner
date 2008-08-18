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

#ifndef _VCB_FITTING_H
#define _VCB_FITTING_H

#ifdef __cplusplus
extern "C" {
#endif

/** Function
 * Name: fpoly
 * Version: 0.9
 * Description: A fitting routine for polynomials. This is 
 *  an internal function used by other routines of the VCB.
 * Arguments:
 *  float x: The coefficient used for fitting.
 *  float p[]: The array of coefficients in the polynomial.
 *  int np: An integer greater than the degree of the polynomial being fit
 * Return: None
 * Known Issues: None
 **/
void fpoly(float x, float p[], int np);

/** Function
 * Name: vcbFit1D
 * Version: 0.9
 * Description: This function uses svd methods to fit a 1d array to polynomials.
 * Arguments:
 *  float* x: The x coordinates for the data points.
 *  float* y: The y coordinates for the data poitns.
 *  float* given_sig: The sigma of each data point.  To get statistically safe 
 *   results this argument should be used.  If it is unavailable, however, 
 *   simply pass NULL to given_sig.
 *  int order: One integer greater than the degree of the polynomial to be used.
 *  int num_elements: Number of data points to use.
 *  float* coeff: Pointer to the cofficients.  vcbFit1D assumes that coeff points to an allocated
 *	 array in memory of at least "order" long.
 *  float* chisq: A statistical term to determine how well the fit is; the closer to 
 *   zero the more accurate the result.  Often, if chisq is less than 0.1 then the 
 *   fitting result can be used for real.
 * Return: None
 * Known Issues: None
 **/
void vcbFit1D(float * x, float * y, float * given_sig, 
		   int order, int num_elements, float * coeff, float * chisq);

/** Function
 * Name: vcbEvalPoly1D
 * Version: 0.9
 * Description: vcbEvalPoly1D is a function that evaluates a polynomial.
 * Arguments:
 *  float* coeff: A pointer to the polynomial's coefficients.
 *  int order: Should be one integer greater than the degree of the polynomial to be evaluated.
 *  float t: Constant at which the function should be evaluated.
 * Return: None
 * Known Issues: None
 **/
float vcbEvalPoly1D(float * coeff, int order, float t);

/** Function
 * Name: vcbEvalPolyd1D
 * Version: 0.9
 * Description: vcbEvalPolyd1D evaluates a polynomial's derivative.
 * Arguments:
 *  float* coeff: A pointer to the polynomial's coefficients.
 *  float t: Constant at which the function should be evaluated.
 *  int polyorder: The order of the polynomial.
 *  int dorder: The order of the polynomial's derivative.
 * Return: None
 * Known Issues: None
 **/
float vcbEvalPolyd1D(float * coeff, float t, int polyorder, int dorder);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
