/****************************************************************************
 *   Copyright (C) 2004  by Markus Glatter									*
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

#ifndef _VCB_OPS_H
#define _VCB_OPS_H

#include "vcbutils.h"

/* start function declarations */

#ifdef __cplusplus
extern "C" {
#endif

/** Function
 * Name: vcbDerivative
 * Version: 0.9
 * Description: Calculates the derivative of a given data set with regard to a given dimension.
 * Arguments:
 *  int ndims: Contains the number of dimensions of the rawdata.
 *  int* dsizes: An array of integers that specifies the number of voxels in each dimension.  
 *  int nattribs: The number of attibutes on each voxel. (all attributes are of the rawdatatype type).
 *  int dim: The dimension with regard to which the derivative will be computed.
 *  vcbdatatype datatype: The type of binary data in the file as defined in the _vcbDATATYPE enumerator. 
 *  int attrib: The attribute the derivative is computed on.
 *  void* rawdata: A pointer to the data set in memory.
 *  VCBfloat* result: aApointer to the resulting data set in memory. The array has to be 
 *   appropriatly allocated before using this function.
 * Return: None
 * Known Issues: None
 **/
void     vcbDerivative(int ndims, int * dsizes, int nattribs, int dim, vcbdatatype datatype, int attrib, void * rawdata, VCBfloat * result);

/** Function
 * Name: vcbAverage
 * Version: 0.9
 * Description: Calculates an average value for every voxel averaging all it's neighboring voxels.
 * Arguments:
 *  int ndims: Contains the number of dimensions of the rawdata.
 *  int* dsizes: An array of integers that specifies the number of voxels in each dimension.  
 *  int nattribs: The number of attibutes on each voxel. (all attributes are of the rawdatatype type).
 *  vcbdatatype datatype: The type of binary data in the file as defined in the _vcbDATATYPE enumerator. 
 *  int attrib: The attribute the derivative is computed on.
 *  void* rawdata: A pointer to the data set in memory.
 *  VCBfloat* result: aApointer to the resulting data set in memory. The array has to be 
 *   appropriatly allocated before using this function.
 * Return: None
 * Known Issues: None
 **/
void     vcbAverage (int ndims, int * dsizes, int nattribs, vcbdatatype datatype, int attrib, void * rawdata, VCBfloat * result);

/** Function
 * Name: vcbLinearityError
 * Version: 0.9
 * Description: Calculates the linearity error for every voxel.
 * Arguments:
 *  int ndims: Contains the number of dimensions of the rawdata.
 *  int* dsizes: An array of integers that specifies the number of voxels in each dimension.  
 *  int nattribs: The number of attibutes on each voxel. (all attributes are of the rawdatatype type).
 *  vcbdatatype datatype: The type of binary data in the file as defined in the _vcbDATATYPE enumerator. 
 *  int attrib: The attribute the derivative is computed on.
 *  void* rawdata: A pointer to the data set in memory.
 *  VCBfloat* result: aApointer to the resulting data set in memory. The array has to be 
 *   appropriatly allocated before using this function.
 * Return: None
 * Known Issues: None
 **/
void vcbLinearityError(int ndims, int * dsizes, int nattribs, vcbdatatype datatype, int attrib, void * rawdata, VCBfloat * result);

#ifdef __cplusplus
}  /* extern C */
#endif


#endif
