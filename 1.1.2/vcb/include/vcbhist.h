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

#ifndef _VCB_HIST_H
#define _VCB_HIST_H

#include "vcbutils.h"
#ifndef VCB_USECUR_ENDIAN
#define VCB_USECUR_ENDIAN 0
#endif
#ifndef VCB_USEINV_ENDIAN
#define VCB_USEINV_ENDIAN 1
#endif

/* define struct vcbHistFile */

/** Struct
 * Name: vcbHistFile
 * Version: 0.93
 * Description: This structure contains the information about a histogram file.
 * Elements:
 *	char* filename:			The filename of the histogram.
 *	FILE* fp:				The file pointer (open status indicated by "status").
 *	char status:			The status of the file pointer&#58; 0 = closed, 1 = open 
 *	int change_endian:		Indicates if the endian order of the histogram file needs to be changed.
 *	unsigned char file_endian:	The file endian byte in the file&#58; 1 if file is big-endian otherwise 0
 *	unsigned char clustag:	The clustered-tag byte in the file.
 *	int vol_dims:			The number of volume dimensions.
 *	int* vol_dsize:			The dimension sizes of the volume.
 *	int ndims:				The number of histogram dimensions.
 *	int* nbin:				The histogram dimensions.
 *	vcbdatatype* dtype:		An array containing the datatypes of each dimension.
 *	double* min:			The minimum value of the histogram.
 *	double* max:			The maximum value of the histogram.
 *	int nclusters:			The number of clusters in the histogram.
 *	int** cluster:			The 2D array of clusters.
 **/
typedef struct histfilepointer{
	char *filename;
	FILE *fp;
	char status; /* 0 = closed, 1 = open */
	int change_endian;
	unsigned char file_endian;
	unsigned char clustag;
	int vol_dims;
	int *vol_dsize;
	int ndims;
	int *nbin;
	vcbdatatype *dtype;
	double *min;
	double *max;
	int nclusters;
	int **cluster;
}* vcbHistFile;


/* start function declarations */

#ifdef __cplusplus
extern "C" {
#endif

/** Function
 * Name: vcbHistAssign
 * Version: 0.91
 * Description: Constructs a vcbHistFile from the given hist file, returning a vcbHistFile pointer.
 * Arguments:
 *  char* filename: The filename of the input hist file.
 * Return: vcbHistFile; The resulting hist file structure.
 * Known Issues: None
 **/
vcbHistFile vcbHistAssign(char *filename);

/** Function
 * Name: vcbHistClose
 * Version: 0.91
 * Description: Closes the hist file pointed to in the given vcbHistFile.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 * Return: None
 * Known Issues: None
 **/
void vcbHistClose(vcbHistFile hf);

/** Function
 * Name: vcbHistGetNumClusters
 * Version: 0.91
 * Description: Returns the number of clusters or 0 if "hf" is null or if the file in "hf" is closed.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 * Return: int; The number of clusters. Will be 0 if "hf" is null or if the file is closed.
 * Known Issues: None
 **/
int vcbHistGetNumClusters(vcbHistFile hf);

/** Function
 * Name: vcbHistGetClusterSize
 * Version: 0.91
 * Description: Returns the size of the cluster at index "clusterno".
 *  Returns 0 if "hf" is null, the file in "hf" is closed or "clusterno" is out of range.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 *  int clusterno: The index of the cluster.
 * Return: int; The size of the cluster at index "clusterno". Otherwise, returns 0 if "hf" is null,
 *  the file in "hf" is closed or "clusterno" is out of range.
 * Known Issues: None
 **/
int vcbHistGetClusterSize(vcbHistFile hf, int clusterno);

/** Function
 * Name: vcbHistGetCluster
 * Version: 0.91
 * Description: Returns an int array containing the histogram cell ids of cells in the cluster.
 *  The last id is -1 to indicate end of array.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 *  int clusterno: The index of the cluster.
 * Return: int*; An int array containing the histogram cell ids of cells in the cluster.
 *  The last id is -1 to indicate end of array.
 * Known Issues: None
 **/
int *vcbHistGetCluster(vcbHistFile hf, int clusterno);

/** Function
 * Name: vcbHistIsBigendian
 * Version: 0.91
 * Description: Returns the "file_endian" value in the given vcbHistFile.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 * Return: unsigned char; The "file_endian" of "hf".  True if the file is big endian.
 * Known Issues: None
 **/
unsigned char vcbHistIsBigendian(vcbHistFile hf);

/** Function
 * Name: vcbHistGetNumVolDims
 * Version: 0.91
 * Description: Returns the number of dimensions of the volume.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 * Return: int; The number of dimensions of the volume.
 * Known Issues: None
 **/
int vcbHistGetNumVolDims(vcbHistFile hf);

/** Function
 * Name: vcbHistGetVolDims
 * Version: 0.91
 * Description: Returns an int array containing the volume dimensions.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 * Return: int*; The int array containing the volume dimensions.
 * Known Issues: None
 **/
int *vcbHistGetVolDims(vcbHistFile hf);

/** Function
 * Name: vcbHistGetNumHistDims
 * Version: 0.91
 * Description: Returns the number of histogram dimensions.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 * Return: int; The number of histogram dimensions.
 * Known Issues: None
 **/
int vcbHistGetNumHistDims(vcbHistFile hf);

/** Function
 * Name: vcbHistGetHistDims
 * Version: 0.91
 * Description: Returns an int array containing the histogram dimensions.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 * Return: int*; The int array containing the histogram dimensions.
 * Known Issues: None
 **/
int *vcbHistGetHistDims(vcbHistFile hf);

/** Function
 * Name: vcbHistGetDatatype
 * Version: 0.91
 * Description: Returns the datatype of the specified dimension.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 *	int dim:		The index of the desired dimension.
 * Return: vcbdatatype; The datatype of the specified dimension (see vcbutils.h).
 * Known Issues: None
 **/
vcbdatatype vcbHistGetDatatype(vcbHistFile hf, int dim);

/** Function
 * Name: vcbHistGetMin
 * Version: 0.91
 * Description: Returns the minimum value of the specified dimension of the histogram.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 *	int dim:		The index of the desired dimension.
 * Return: double; The minimum value of the specified dimension of the histogram.
 * Known Issues: None
 **/
double vcbHistGetMin(vcbHistFile hf, int dim);

/** Function
 * Name: vcbHistMax
 * Version: 0.91
 * Description: Returns the maximum value of the specified dimension of the histogram.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 *	int dim:		The index of the desired dimension.
 * Return: double; The maximum value of the specified dimension of the histogram.
 * Known Issues: None
 **/
double vcbHistGetMax(vcbHistFile hf, int dim);

/** Function
 * Name: vcbHistGetClusterRange
 * Version: 0.91
 * Description: Returns an int array containing ranges for each dimension of the specified cluster.
 * Arguments:
 *  vcbHistFile hf: The pointer to the hist file structure.
 *	int clusterno:	The index of the desired cluster.
 * Return: int*; The int array containing the volume data.
 * Known Issues: None
 **/
int *vcbHistGetClusterRange(vcbHistFile hf, int clusterno);

/** Function
 * Name: vcbSparseHist
 * Version: 0.91
 * Description:  Converts a binfile from spatial space to histogram space, and can also
 *   extend a given histogram file with the new data extracted from the binfile. <br>
 *   This will extract the original histogram file by one dimension.
 * Arguments:
 *  char* binfile:      The filename of the .bin.xx file to be read in.
 *  char* histfile_in:  The filename of the histogram source file that is to be extended
 *     by one dimension using histogram data extracted from binfile. <br>
 *     The filename can be empty (""), NULL, or "NULL" to indicate that no former histogram file
 *     is to be extended. In that case, the output will be a one-dimensional histogram file using
 *     histogram data extracted from the binfile.
 *  char* histfile_out: The filename of the output histogram file that has been extended by one dimension.
 *  double lower:       The lower boundary of the data range of "binfile". The data range is used
 *     to compute bin membership for single data points.
 *  double upper:       The upper boundary of the data range of "binfile". The data range is used
 *     to compute bin membership for single data points.
 *  int nbins:          The number of bins used for the data from binfile. The range from the max
 *     value to the min value of binfile will be divided into the given number of bins to build up a histogram.
 *  int threshold:      The integer threshold value that determines how many voxels will have to be
 *     in one bin for the bin not to be discarded. A bin with fewer voxels than the threshold value
 *     will not be used, and will not be put into the output histogram file.
 *  int attrib:         Determines which attribute of the binfile the histogram will be extended by.
 * Return: int; An indicator of success for the operation&#58;<br>
 *                0 - success<br>
 *                1 - indicates an error (see output for further information)
 * Known Issues: None
 **/
int vcbSparseHist(char *binfile, char *histfile_in, char *histfile_out, double lower, double upper, int nbins, int threshold, int attrib);

/** Function
 * Name: vcbSparseHistAutoRange
 * Version: 0.91
 * Description:  Converts a binfile from spatial space to histogram space, and can also
 *   extend a given histogram file with the new data extracted from the binfile. <br>
 *   This will extract the original histogram file by one dimension.<br>
 *   <p>
 *   The upper and lower boundaries of data in "binfile" are assumed to be 0.0. <br>
 *   See vcbSparseHist for the alternate version which specifies the upper and lower bounds.
 * Arguments:
 *  char* binfile:      The filename of the .bin.xx file to be read in.
 *  char* histfile_in:  The filename of the histogram source file that is to be extended
 *     by one dimension using histogram data extracted from binfile. <br>
 *     The filename can be empty (""), NULL, or "NULL" to indicate that no former histogram file
 *     is to be extended. In that case, the output will be a one-dimensional histogram file using
 *     histogram data extracted from the binfile.
 *  char* histfile_out: The filename of the output histogram file that has been extended by one dimension.
 *  int nbins:          The number of bins used for the data from binfile. The range from the max
 *     value to the min value of binfile will be divided into the given number of bins to build up a histogram.
 *  int threshold:      The integer threshold value that determines how many voxels will have to be
 *     in one bin for the bin not to be discarded. A bin with fewer voxels than the threshold value
 *     will not be used, and will not be put into the output histogram file.
 *  int attrib:         Determines which attribute of the binfile the histogram will be extended by.
 * Return: int; An indicator of success for the operation&#58;<br>
 *                0 - success<br>
 *                1 - indicates an error (see output for further information)
 * Known Issues: None
 **/
int vcbSparseHistAutoRange(char *binfile, char *histfile_in, char *histfile_out, int nbins, int threshold, int attrib);

#ifdef __cplusplus
}  /* extern C */
#endif


#endif
