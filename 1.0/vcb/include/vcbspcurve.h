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

#ifndef _VCB_SPCURVE_H
#define _VCB_SPCURVE_H

#include "zcurve.h"
#include "hilbert.h"

#ifdef __cplusplus
extern "C" {
#endif


/** Function
 * Name: vcbGenHscurve
 * Version: 0.9
 * Description:
 *  "vcbGenHscurve" takes two inputs and generates a hilbert space filling curve.
 *  <p>
 *  Note&#58; The maximum length of space filling curves that can be generated with 
 *  this code is 1024x1024x1024 vertices in length.
 * Arguments:
 *	int n_dims: The number of dimensions which must be an integer > 1. Usually, a hilbert space
 *		filling curve is generated for high dimensional spaces of n by n by n ... by n size, with
 *		n being a power of 2 and the total number of dimensions being "n_dims".
 *	int n_bits: The number of bits in each dimension used to specify each voxel. For instance,
 *		if there are 32 elements along each dimension, then "n_bits" = 5.
 *	int* coords: A pointer to the coordinates generated for each vertex on the space filling curve.
 *		All coordinate value are generated as integers, and the memory for the coordinates must
 *		be preallocated before entering "vcbGenHscurve".
 * Return: None
 * Known Issues: None
 **/
void vcbGenHscurve(int n_dims, int n_bits, int * coords);

/** Function
 * Name: vcbGenZscurve
 * Version: 0.9
 * Description:
 *  "vcbGenZscurve" takes two inputs and generates a z space filling curve.
 *  <p>
 *  Note&#58; The maximum length of space filling curves that can be generated with 
 *  this code is 1024x1024x1024 vertices in length.
 * Arguments:
 *	int n_dims: The number of dimensions (must be > 1). Usually, a z space
 *		filling curve is generated for high dimensional spaces of n by n by n ... by n size, with
 *		n being a power of 2 and the total number of dimensions being "n_dims".
 *	int n_bits: The number of bits in each dimension used to specify each voxel. For instance,
 *		if there are 32 elements along each dimension, then "n_bits" = 5.
 *	int* coords: A pointer to the coordinates generated for each vertex on the space filling curve.
 *		All coordinate value are generated as integers, and the memory for the coordinates must
 *		be preallocated before entering "vcbGenZscurve".
 * Return: None
 * Known Issues: None
 **/
void vcbGenZscurve(int n_dims, int n_bits, int * coords);


/** Function
 * Name: vcbScurveSize
 * Version: 0.9
 * Description:
 *  "vcbScurveSize" computes the size of a space filling curve given 
 *  the number of dimensions (n_dims) and the number of bits in each dimension 
 *  used to specify each voxel (n_bits).  It returns the number of vertices on
 *  the space curve.
 * Arguments:
 *  int n_dims: Number of dimensions for filling curve.
 *  int n_bits: Number of bits in each dimension used to specify each voxel.
 * Return: int; The number of vertices on the space curve
 * Known Issues: None
 **/
int vcbScurveSize(int n_dims, int n_bits);

/** Directions for vcbspcurve_z
 * <h2>Description:</h2>
 *	The following three functions are designed to generate vertices on a z space curve
 *	one by one:<br>
 *	vcbBeginIncrZscurve <br>
 *	vcbIncrZscurve <br>
 *	vcbEndIncrZscurve
 *
 *	<h2>The intended usage:</h2><pre>
 *	vcbBeginIncrZscurve
 *		for (...)
 *			vcbIncrZscurve
 *	vcbEndIncrZscurve</pre>
 *
 * The two input parameters to vcbBeginIncrZscurve: "n_dims" and "n_bits", are the same
 * as in vcbGenZscurve. Also, the input parameter to vcbIncrZscurve: coords, is the same
 * as the "coords" parameter in vcbGenZscurve.
 **/

/** Function
 * Name: vcbBeginIncrZscurve
 * Version: 0.9
 * Description:
 *	vcbBeginIncrZscurve is to be used along with vcbIncrZscurve and vcbEndIncrZscurve to generate
 *	vertices on a z space curve one by one. 
 * Arguments:
 *  int n_dims: Number of dimensions for filling curve.
 *  int n_bits: Number of bits in each dimension used to specify each voxel.
 * Return: None
 * Known Issues: None
 **/
void vcbBeginIncrZscurve(int n_dims, int n_bits);

/** Function
 * Name: vcbIncrZscurve
 * Version: 0.9
 * Description:
 *	vcbIncrZscurve is called between vcbBeginIncrZscurve and vcbEndIncrZscurve to generate
 *	vertices on a z space curve one by one. 
 * Arguments:
 *	int* coords: A pointer to the coordinates generated for each vertex on the space filling curve.
 *		All coordinate values are generated as integers, and the memory for the coordinates must
 *		be preallocated.
 * Return: None
 * Known Issues: None
 **/
void vcbIncrZscurve(int * coords);

/** Function
 * Name: vcbEndIncrZscurve
 * Version: 0.9
 * Description:
 *	vcbEndIncrZscurve ends the usage of the API corresponding to vcbBeginIncrZscurve and vcbIncrZscurve. 
 * Arguments: None
 * Return: None
 * Known Issues: None
 **/
void vcbEndIncrZscurve();


/** Directions for vcbspcurve_hilbert
 * <h2>Description:</h2>
 * The following three functions are designed to generate vertices on a
 *	hilbert space curve one by one:<br>
 *	vcbBeginIncrHscurve <br>
 *	vcbIncrHscurve <br>
 *	vcbEndIncrHscurve
 *
 *	<h2>The intended usage:</h2><pre>
 *	vcbBeginIncrHscurve
 *		for (...)
 *			vcbIncrHscurve
 *	vcbEndIncrHscurve</pre>
 *
 *  The two input parameters to vcbBeginIncrHscurve: "n_dims" and "n_bits", are the same
 *	as in vcbGenHscurve. Also, the input parameter to vcbIncrHscurve, "coords", is the same
 *	as the "coords" parameter in vcbGenHscurve.
 **/

/** Function
 * Name: vcbBeginIncrHscurve
 * Version: 0.9
 * Description:
 *	vcbBeginIncrHscurve is to be used along with vcbIncrHscurve and vcbEndIncrHscurve to generate
 *	vertices on a h space curve one by one. 
 * Arguments:
 *  int n_dims: Number of dimensions for filling curve.
 *  int n_bits: Number of bits in each dimension used to specify each voxel.
 * Return: None
 * Known Issues: None
 **/
void vcbBeginIncrHscurve(int n_dims, int n_bits);

/** Function
 * Name: vcbIncrHscurve
 * Version: 0.9
 * Description:
 *	vcbIncrHscurve is called between vcbBeginIncrHscurve and vcbEndIncrHscurve to generate
 *	vertices on a h space curve one by one. 
 * Arguments:
 *	int* coords: A pointer to the coordinates generated for each vertex on the space filling curve.
 *		All coordinate values are generated as integers, and the memory for the coordinates must
 *		be preallocated.
 * Return: None
 * Known Issues: None
 **/
void vcbIncrHscurve(int * coords);

/** Function
 * Name: vcbEndIncrHscurve
 * Version: 0.9
 * Description:
 *	vcbEndIncrHscurve ends the usage of the API corresponding to vcbBeginIncrHscurve and vcbIncrHscurve. 
 * Arguments: None
 * Return: None
 * Known Issues: None
 **/
void vcbEndIncrHscurve();

#ifdef __cplusplus
}  /* extern C */
#endif

#endif

