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

#ifndef _VCB_CLUSTER_H
#define _VCB_CLUSTER_H

/** Enum
 * Name: vcbClusterFunc
 * Version: 0.9
 * Description: This is an enumerator that the vcbCluster code uses to
 *   describe the type of cluster function used with a data set.
 * Elements: 
 *	VCB_PROXIMITY:			Defines cluster by proximity of elements in histogram space
 *	VCB_PROXIMITY_FUZZY:		Defines a fuzzy cluster by proximity of elements in histogram space
 *	VCB_DENSITY_10:			Defines cluster by density values of elements in histogram space
 *	VCB_DENSITY_100:			Defines cluster by density values of elements in histogram space
 *	VCB_DENSITY_1000:		Defines cluster by density values of elements in histogram space
 *	VCB_DENSITY_10000:		Defines cluster by density values of elements in histogram space
 *	VCB_DENSITY_LG_10:		Defines cluster by logarithm of density values of elements in histogram space
 *	VCB_DENSITY_LG_100:		Defines cluster by logarithm of density values of elements in histogram space
 *	VCB_DENSITY_LG_1000:		Defines cluster by logarithm of density values of elements in histogram space
 *	VCB_DENSITY_LG_10000:	Defines cluster by logarithm of density values of elements in histogram space
 **/
typedef enum _vcbCLUSTERFUNC {
	VCB_PROXIMITY,			/* defines cluster by proximity of elements in histogram space */
	VCB_PROXIMITY_FUZZY,		
	VCB_DENSITY_10,			/* defines cluster by density values of elements in histogram space */
	VCB_DENSITY_100,
	VCB_DENSITY_1000,
	VCB_DENSITY_10000,
	VCB_DENSITY_LG_10,		/* defines cluster by logarithm of density values of elements in histogram space */
	VCB_DENSITY_LG_100,
	VCB_DENSITY_LG_1000,
	VCB_DENSITY_LG_10000
} vcbClusterFunc;

/* start function declarations */

#ifdef __cplusplus
extern "C" {
#endif

/** Function
 * Name: vcbCluster
 * Version: 0.9
 * Description: Clusters data according to a given cluster function.
 * Arguments:
 *	char* histfile_in:  The filename of the histogram source file that is to be extended
 *		by one dimension using histogram data extracted from binfile.<p>
 *      The filename can be empty (""), NULL, or "NULL" to indicate that no former
 *       histogram file is to be extended. In that case the output will be a
 *		 one-dimensional histogram file using histogram data extracted from binfile.</p>
 *  char* histfile_out: The filename of the output histogram file that has been extended by one dimension.
 *  vcbClusterFunc functions:  The type of cluster function to be used. See the vcbClusterFunc enum.
 * Return: int;   An indicator of success for the operation&#58;<BR>
 *                0 - success<BR>
 *                1 - indicates an error (see output for further information)
 * Known Issues: None
 **/
int      vcbCluster(char *histfile_in, char *histfile_out, vcbClusterFunc functions);

#ifdef __cplusplus
}  /* extern C */
#endif


#endif
