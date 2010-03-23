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

#ifndef _VCB_RANGE_H
#define _VCB_RANGE_H

#include "vcbhist.h"

/* define struct RANGE */

/** Struct
 * Name: vcbRange
 * Version: 0.93
 * Description:
 *   Used to store cluster ranges (upper and lower bounds) for any number of dimensions.
 * Elements:
 *   int ndims:		The number of dimensions stored.
 *   int** ranges:	A 2d array containing the ranges.
 **/
typedef struct rangepointer{
	int ndims;
	int** ranges;
}* vcbRange;

/* define enum vcbClusterMode */

/** Enum
 * Name: vcbClusterMode
 * Version: 0.91
 * Description:
 *   The elements are "modes" used by vcbGetCluster to indicate if clusters should be fully
 *		or partially inside or outside a range.
 * Elements:
 *   VCB_FULLY_INSIDE:         All clusters are fully inside range
 *   VCB_FULLY_OUTSIDE:        All clusters are fully outside range
 *   VCB_PARTIALLY_INSIDE:     All clusters are either fully or partially inside range
 *   VCB_PARTIALLY_OUTSIDE:    All clusters are either fully or partially outside range
 **/
typedef enum _vrCLUSTERMODE {
     VCB_FULLY_INSIDE,         /* all clusters are fully inside range */
     VCB_FULLY_OUTSIDE,        /* all clusters are fully outside range */
     VCB_PARTIALLY_INSIDE,     /* all clusters are either fully or partially inside range */
     VCB_PARTIALLY_OUTSIDE     /* all clusters are either fully or partially outside range */
} vcbClusterMode;

/* start function declarations */

#ifdef __cplusplus
extern "C" {
#endif

/** Function
 * Name: vcbRangeGetFull
 * Version: 0.9
 * Description:
 *  Returns a vcbRange with lower and upper bounds for each dimension
 *   set to 0 and 214748367 respectively. <br>
 * Arguments:
 *  int ndims: The desired number of dimensions to be allocated.
 * Return: vcbRange; The newly created vcbRange struct with all values initialized.
 * Known Issues: None
 **/
vcbRange vcbRangeGetFull(int ndims);

/** Function
 * Name: vcbRangeGetEmpty
 * Version: 0.9
 * Description:
 *  Returns a vcbRange with lower and upper bounds for each dimension
 *  set to 0 and -1 respectively. <br>
 * Arguments:
 *  int ndims: The desired number of dimensions to be allocated.
 * Return: vcbRange; The newly created vcbRange struct with all values initialized.
 * Known Issues: None
 **/
vcbRange vcbRangeGetEmpty(int ndims);

/** Function
 * Name: vcbRangeIsEmpty
 * Version: 0.9
 * Description:
 *  Indicates if a vcbRange is empty (lower = 0, upper = -1).
 * Arguments:
 *  vcbRange r: The vcbRange to be tested.
 * Return: int; 0 if "r" is empty, otherwise 1.
 * Known Issues: None
 **/
int vcbRangeIsEmpty(vcbRange r);

/** Function
 * Name: vcbRangeExtract
 * Version: 0.9
 * Description:
 *  Allocate and extract a vcbRange from a cluster in the given histfile. <br>
 *  See vcbhist.h for vcbHistFile info.
 * Arguments:
 *  vcbHistFile hf: The histfile to read ranges from.
 *  int custernr:	The desired cluster index.
 * Return: vcbRange; The newly allocated and initialized vcbRange struct containing the ranges
 *  in the specified cluster in histfile.
 * Known Issues: None
 **/
vcbRange vcbRangeExtract(vcbHistFile hf, int clusternr);

/** Function
 * Name: vcbRangeSetDim
 * Version: 0.9
 * Description:
 *  Set the lower and upper values of the specified dimension.
 * Arguments:
 *  vcbRange r: The vcbRange struct.
 *  int dim:	The index of the desired dimension.
 *	int lower:	The new lower bound value.
 *	int upper:	The new upper bound value.
 * Return: None
 * Known Issues: None
 **/
void vcbRangeSetDim(vcbRange r, int dim, int lower, int upper);

/** Function
 * Name: vcbRangeGetDimLower
 * Version: 0.9
 * Description:
 *  Get the lower bound at the specified dimension.
 * Arguments:
 *  vcbRange r: The vcbRange struct.
 *  int dim:	The index of the desired dimension.
 * Return: int; The lower bound at the specified dimension.
 * Known Issues: None
 **/
int vcbRangeGetDimLower(vcbRange r, int dim);

/** Function
 * Name: vcbRangeGetDimUpper
 * Version: 0.9
 * Description:
 *  Get the upper bound at the specified dimension.
 * Arguments:
 *  vcbRange r: The vcbRange struct.
 *  int dim:	The index of the desired dimension.
 * Return: int; The upper bound at the specified dimension.
 * Known Issues: None
 **/
int vcbRangeGetDimUpper(vcbRange r, int dim);

/** Function
 * Name: vcbRangeFreeDim
 * Version: 0.9
 * Description:
 *  Resets the specified dimension to [0, 2147483647] (full).
 * Arguments:
 *  vcbRange r: The vcbRange struct.
 *  int dim:	The index of the desired dimension.
 * Return: None
 * Known Issues: None
 **/
void vcbRangeFreeDim(vcbRange r, int dim);

/** Function
 * Name: vcbRangeUnion
 * Version: 0.9
 * Description:
 *  Union the lower and upper bounds for every dimension in two vcbRange structs. <br>
 *	The result will be stored in the first vcbRange.
 * Arguments:
 *  vcbRange result: The union of both structs will be stored here.
 *	vcbRange second: The vcbRange to be unioned with the first.
 * Return: None
 * Known Issues: None
 **/
void vcbRangeUnion(vcbRange result, vcbRange second);

/** Function
 * Name: vcbRangeIntersection
 * Version: 0.9
 * Description:
 *  Intersect the lower and upper bounds for every dimension in two vcbRange structs. <br>
 *	The result will be stored in the first vcbRange.
 * Arguments:
 *  vcbRange result: The intersection of both structs will be stored here.
 *	vcbRange second: The vcbRange to be unioned with the first.
 * Return: None
 * Known Issues: None
 **/
void vcbRangeIntersection(vcbRange result, vcbRange second);

/** Function
 * Name: vcbRangeFree
 * Version: 0.9
 * Description:
 *  Frees the memory allocated in the struct.
 * Arguments:
 *  vcbRange r: The vcbRange struct to be freed.
 * Return: None
 * Known Issues: None
 **/
void vcbRangeFree(vcbRange r);

/** Function
 * Name: vcbGetClusters
 * Version: 0.9
 * Description:
 *  Stores all clusters from "hf" into "clusterlist" according to the "mode" and the range "r". <br>
 *	Note&#58; clusterlist will be allocated by vcbGetClusters. In the calling function, it
 *    should be declared as "int* list" and passed as "&list".
 * Arguments:
 *	vcbHistFile hf: The histogram file struct containing the clusters.
 *	vcbRange r:		The struct containing the desired ranges.
 *	vcbClusterMode mode:	The desired mode for extraction. See the vcbClusterMode enum.
 *  int** clusterlist:	The array for storing the clusters, passed by reference.
 * Return: int; The number of clusters stored in clusterlist, or 0 for an error.
 * Known Issues: None
 **/
int vcbGetClusters(vcbHistFile hf, vcbRange r, vcbClusterMode mode, int** clusterlist);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
