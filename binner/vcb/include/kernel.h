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

#ifndef _VCB_KERNEL_H_
#define _VCB_KERNEL_H_

#ifndef GAUSSIANFOOTPRINTRADIUS
#define GAUSSIANFOOTPRINTRADIUS   2.0
#endif
#ifndef CRAWFISIANFOOTPRINTRADIUS
#define CRAWFISIANFOOTPRINTRADIUS 1.56
#endif
#ifndef NGAUSSIANFOOTPRINTRADIUS
#define NGAUSSIANFOOTPRINTRADIUS   -2.0
#endif
#ifndef NCRAWFISIANFOOTPRINTRADIUS
#define NCRAWFISIANFOOTPRINTRADIUS -1.56
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* When kernel.h is included, these macros are available:
 *
 *     #ifndef GAUSSIANFOOTPRINTRADIUS
#define GAUSSIANFOOTPRINTRADIUS   2.0
#endif
 *     #ifndef CRAWFISIANFOOTPRINTRADIUS
#define CRAWFISIANFOOTPRINTRADIUS 1.56
#endif
 *     #ifndef NGAUSSIANFOOTPRINTRADIUS
#define NGAUSSIANFOOTPRINTRADIUS   -2.0
#endif
 *     #ifndef NCRAWFISIANFOOTPRINTRADIUS
#define NCRAWFISIANFOOTPRINTRADIUS -1.56
#endif
 */

/** Function
 * Name: fptTableC1d
 * Version: 0.9
 * Description:
 *  Generates a 1D footprint table using Crawfisian kernel. 
 *  (fz,lz) defines the range on which numerical integration 
 *  is performed. Legal values of fz and lz should be [-2.f, 2.f].
 *  Crawfisian kernel is preferred for splatting, but for the 
 *  Gaussian version of this function, see fptTableG1d.
 * Arguments:
 *  int res: Resolution of the table.
 *  float* tab: A pointer to an external allocated memory space
 *         for at least res number of floats.
 *  float fz: The first z.
 *  float lz: The last z.
 * Return: None
 * Known Issues: None 
 **/
void   fptTableC1d(int res, float * tab, float fz, float lz);

/** Function
 * Name: fptTableG1d
 * Version: 0.9
 * Description:
 *  Generates a 1D footprint table using Gaussian. 
 *  (fz,lz) defines the range on which numerical integration 
 *  is performed. Legal values of fz and lz should be [-2.f, 2.f].
 *  See fptTableC1d for the Crawfisian version of this function.
 * Arguments:
 *  int res: Resolution of the table.
 *  float* tab: A pointer to an external allocated memory space
 *         for at least res number of floats.
 *  float fz: The first z.
 *  float lz: The last z.
 * Return: None
 * Known Issues: None 
 **/
void   fptTableG1d(int res, float * tab, float fz, float lz);

/** Function
 * Name: fptTableC2d
 * Version: 0.9
 * Description:
 *  Generates a 2D footprint table using Crawfisian kernel. 
 *  (fz,lz) defines the range on which numerical integration 
 *  is performed. Legal values of fz and lz should be [-2.f, 2.f].
 *  Crawfisian kernel is preferred for splatting, but for the 
 *  Gaussian version of this function, see fptTableG2d.
 * Arguments:
 *  int res: Resolution of the table.
 *  float* tab: A pointer to an external allocated memory space
 *         for at least res number of floats.
 *  float fz: The first z.
 *  float lz: The last z.
 * Return: None
 * Known Issues: None 
 **/
void   fptTableC2d(int res, float * tab, float fz, float lz);

/** Function
 * Name: fptTableG2d
 * Version: 0.9
 * Description:
 *  Generates a 2D footprint table using Gaussian kernel. 
 *  (fz,lz) defines the range on which numerical integration 
 *  is performed. Legal values of fz and lz should be [-2.f, 2.f].
 *  For the Crawfisian version of this function, see fptTableC2d.
 * Arguments:
 *  int res: Resolution of the table.
 *  float* tab: A pointer to an external allocated memory space
 *         for at least res number of floats.
 *  float fz: The first z.
 *  float lz: The last z.
 * Return: None
 * Known Issues: None 
 **/
void   fptTableG2d(int res, float * tab, float fz, float lz);

/** Function
 * Name: fptenc
 * Version: 0.9
 * Description:
 *  Computes the footprint value at r distance away from center using numerical
 *  integration performed between [fz, lz]. If r is smaller than the radius of 
 *  the 3D kernel (1.56), fptenc should return non-zero values. 
 *  (fz,lz) defines the range on which numerical integration 
 *  is performed. Legal values of fz and lz should be [-2.f, 2.f].
 *  This function uses Crawfisian kernel (preferred); see fpteng for the Gaussian version of this function.
 * Arguments:
 *  double r: Resolution of the table.
 *  float fz: The first z.
 *  float lz: The last z.
 * Return: None
 * Known Issues: None 
 **/
double fptenc(double r, double fz, double lz);

/** Function
 * Name: fpteng
 * Version: 0.9
 * Description:
 *  Computes the footprint value at r distance away from center using numerical
 *  integration performed between [fz, lz]. If r is smaller than the radius of 
 *  the 3D kernel (2.0), fpteng should return non-zero values. 
 *  (fz,lz) defines the range on which numerical integration 
 *  is performed. Legal values of fz and lz should be [-2.f, 2.f].
 *   This function uses Gaussian; see fptenc for the Crawfisian version of this function.
 * Arguments:
 *  double r: Resolution of the table.
 *  float fz: The first z.
 *  float lz: The last z.
 * Return: None
 * Known Issues: None 
 **/
double fpteng(double r, double fz, double lz);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
