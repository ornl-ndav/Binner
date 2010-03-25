/***************************************************************************
 *   Copyright (C) 2004  Markus Glatter                                    *
 *   seelab@cs.utk.edu                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "vcbutils.h"
#include "vcbops.h"

/*
 * =============================================================
 * vr data quantitative operator functions to derive indicators  
 * =============================================================
 */

static double vcbops_convert(void *data, vcbdatatype datatype) {
	/*converting from a datatype to an actual float value*/
	VCBubyte    *vrub1 = (VCBubyte *)data;
	VCBbyte     *vrb1 = (VCBbyte *)data; 
	VCBushort   *vrus1 = (VCBushort *)data;     
	VCBshort    *vrs1 = (VCBshort *)data;            
	VCBuint     *vrui1 = (VCBuint *)data;       
	VCBint      *vri1 = (VCBint *)data; 
	VCBfloat    *vrf1 = (VCBfloat *)data;  
	VCBdouble   *vrd1 = (VCBdouble *)data;  
	VCBbitfield *vrbf1  = (VCBbitfield *)data; 

	double retVal;

	switch (datatype) {
	case VCB_UNSIGNEDBYTE:
		retVal = (double)(*vrub1);
	break;
	case VCB_BYTE:
		retVal = (double)(*vrb1);
		break;
	case VCB_UNSIGNEDSHORT:
		retVal = (double)(*vrus1);
		break;
	case VCB_SHORT:
		retVal = (double)(*vrs1);
		break;
	case VCB_UNSIGNEDINT:
		retVal = (double)(*vrui1);
		break;
	case VCB_INT:
		retVal = (double)(*vri1);
		break;
	case VCB_FLOAT:
		retVal = (double)(*vrf1);
		break;
	case VCB_DOUBLE:
		retVal = (double)(*vrd1);
		break;
	case VCB_BITFIELD:
		retVal = (double)(*vrbf1);
		break;
	}

	return retVal;
}

static int vcbops_dfindex(int ndims, int * dsizes, int * coords)
/* VR default indexing func, coords values start from zero */
{
	int i, id = coords[0];
	for (i = 1; i < ndims; i ++)
		id = id*dsizes[i] + coords[i];

	return id;
}

void vcbDerivative(int ndims, int * dsizes, int nattribs, 
			 int dim, vcbdatatype datatype, int attrib, 
			 void * rawdata, VCBfloat * result) {
/**
 * "vcbDerivative" calculates the derivative of a given data set with regard to a given dimension.
 *
 * "ndims"        contains the number of dimensions of the rawdata.
 * "dsizes"       is an array of integers that specifies the number of voxels in each dimension.  
 * "nattribs"     is number of attibutes on each voxel. (all attributes are of the rawdatatype type).
 * "dim"          is the dimension with regard to which the derivative will be computed.
 * "datatype"     is the type of binary data in the file as defined in the _vcbDATATYPE enumerator. 
 * "attrib"       is the attribute the derivative is computed on.
 * "rawdata"      is a pointer to the data set in memory.
 * "result"       is a pointer to the resulting data set in memory. The array has to be appropriatly allocated before using this function.
 */
	int i, cindex1, cindex2;
	VCBfloat gM1, gM2;
	int * coords;
	
	coords = (int *) calloc(ndims+1,sizeof(int));

	while (coords[ndims] == 0) {
		/* boundary check and determine lower value */
		if (coords[dim] > 0) {
			coords[dim]--;
			cindex1 = vcbops_dfindex(ndims, dsizes, coords) * nattribs + attrib;
			coords[dim]++;
		} else {
			cindex1 = vcbops_dfindex(ndims, dsizes, coords) * nattribs + attrib;
		}

		/* boundary check and determine upper value */
		if (coords[dim] < (dsizes[dim] - 1)) {
			coords[dim]++;
			cindex2 = vcbops_dfindex(ndims, dsizes, coords) * nattribs + attrib;
			coords[dim]--;
		} else {
			cindex2 = vcbops_dfindex(ndims, dsizes, coords) * nattribs + attrib;
		}

		/* access data and extract information */
		gM1 = vcbops_convert(&((char*)rawdata)[cindex1 * vcbSizeOf(datatype)], datatype);
		gM2 = vcbops_convert(&((char*)rawdata)[cindex2 * vcbSizeOf(datatype)], datatype);

		/* calculate result */
		result[vcbops_dfindex(ndims, dsizes, coords)] = (gM2 - gM1) * .5f;

		/* increase index (coords) */
		coords[0]++;
		for (i = 0; i < ndims; i++) {
			if (coords[i] >= dsizes[i]) {
				coords[i] = 0;
				coords[i+1]++;
			}
		}
	}

}

void vcbAverage(int ndims, int * dsizes, int nattribs,
			vcbdatatype datatype, int attrib, 
			void * rawdata, VCBfloat * result) {
/**
 * "vcbAverage" calculates an average value for every voxel averaging all it's neighboring voxels.
 *
 * "ndims"        contains the number of dimensions of the rawdata.
 * "dsizes"       is an array of integers that specifies the number of voxels in each dimension.  
 * "nattribs"     is number of attibutes on each voxel. (all attributes are of the rawdatatype type).
 * "datatype"     is the type of binary data in the file as defined in the _vcbDATATYPE enumerator. 
 * "attrib"       is the attribute the average is computed on.
 * "rawdata"      is a pointer to the data set in memory.
 * "result"       is a pointer to the resulting data set in memory. The array has to be appropriatly allocated before using this function.
 */
	int i, dim, cindex;
	VCBfloat count, sum;
	int * coords;
	int * npts;
	int * c_inuse;
	
	coords = (int *) calloc(ndims+1,sizeof(int));
	c_inuse = (int *) calloc(ndims+1, sizeof(int));
	npts = (int *) calloc(ndims+1, sizeof(int));

	while (coords[ndims] == 0) {
		for(i = 0; i <= ndims; i++) {
			npts[i] = 0;
		}
		sum = .0f;
		count = .0f;

		while (npts[ndims] == 0) {
			/* determine coords to use */
			for (i = 0; i < ndims; i++) {
				c_inuse[i] = coords[i] + npts[i] - 1;
			}

			/* check boundaries on all dimensions of coords in use */
			for (dim = 0; dim < ndims; dim++) {
				/* boundary check for lower value */
				if (c_inuse[dim] < 0) {
					c_inuse[dim] = 0;
				}
				/* boundary check for upper value */
				if (c_inuse[dim] >= dsizes[dim]) {
					c_inuse[dim] = dsizes[dim]-1;
				}
			}

			/* access data and extract information */
			cindex = vcbops_dfindex(ndims, dsizes, c_inuse) * nattribs + attrib;
			sum += vcbops_convert(&((char*)rawdata)[cindex * vcbSizeOf(datatype)], datatype);
			count += 1.0f;

			/* increase index (coords) */
			npts[0]++;
			for (i = 0; i < ndims; i++) {
				if (npts[i] > 2) {
					npts[i] = 0;
					npts[i+1]++;
				}
			}
		
		}

		/* calculate result */
		result[vcbops_dfindex(ndims, dsizes, coords)] = sum / count;

		/* increase index (coords) */
		coords[0]++;
		for (i = 0; i < ndims; i++) {
			if (coords[i] >= dsizes[i]) {
				coords[i] = 0;
				coords[i+1]++;
			}
		}
	}
}

void vcbLinearityError(int ndims, int * dsizes, int nattribs,
				  vcbdatatype datatype, int attrib, 
				  void * rawdata, VCBfloat * result) {
/**
 * "vcbLinearityError" calculates the linearity error for every voxel.
 *
 * "ndims"        contains the number of dimensions of the rawdata.
 * "dsizes"       is an array of integers that specifies the number of voxels in each dimension.  
 * "nattribs"     is number of attibutes on each voxel. (all attributes are of the rawdatatype type).
 * "datatype"     is the type of binary data in the file as defined in the _vcbDATATYPE enumerator. 
 * "attrib"       is the attribute the average is computed on.
 * "rawdata"      is a pointer to the data set in memory.
 * "result"       is a pointer to the resulting data set in memory. The array has to be appropriatly allocated before using this function.
 */
	VCBfloat **gradient, *average, count, sum, func;
	int i, maxIndex, dim, cindex, coindex, *coords, *npts, *c_inuse;

	/* allocate memory */
	for (i=0, maxIndex=1; i < ndims; i++) maxIndex *= dsizes[i];
	gradient = (VCBfloat **)malloc(ndims*sizeof(VCBfloat *));
	for (i=0; i < ndims; i++) gradient[i] = (VCBfloat *)malloc(maxIndex*sizeof(VCBfloat));
	average = (VCBfloat *)malloc(maxIndex*sizeof(VCBfloat));

	/* if failed, report */
	if (!average) {
		printf("vcbLinearityError(): Memory allocation failed!");
		exit(1);
	}

	/* fill in gradient and average */
	for (i=0; i < ndims; i++) vcbDerivative(ndims, dsizes, nattribs, i, datatype, attrib, rawdata, gradient[i]);
	vcbAverage(ndims, dsizes, nattribs, datatype, attrib, rawdata, average);

	/* start calculating linearity error */
	coords = (int *) calloc(ndims+1,sizeof(int));
	c_inuse = (int *) calloc(ndims+1, sizeof(int));
	npts = (int *) calloc(ndims+1, sizeof(int));

	while (coords[ndims] == 0) {
		for(i = 0; i <= ndims; i++) {
			npts[i] = 0;
		}
		sum = .0f;
		count = .0f;

		while (npts[ndims] == 0) {
			/* determine coords to use */
			for (i = 0; i < ndims; i++) {
				c_inuse[i] = coords[i] + npts[i] - 1;
			}

			/* check boundaries on all dimensions of coords in use */
			for (dim = 0; dim < ndims; dim++) {
				/* boundary check for lower value */
				if (c_inuse[dim] < 0) {
					c_inuse[dim] = 0;
				}
				/* boundary check for upper value */
				if (c_inuse[dim] >= dsizes[dim]) {
					c_inuse[dim] = dsizes[dim]-1;
				}
			}

			/* calculate indices */
			cindex  = vcbops_dfindex(ndims, dsizes, c_inuse) * nattribs + attrib;
			coindex = vcbops_dfindex(ndims, dsizes, coords ) * nattribs + attrib;

			/* compute linear function minus voxel value (squared) */
			func = vcbops_convert(&((char*)rawdata)[coindex * vcbSizeOf(datatype)], datatype);
			for (i = 0; i < ndims; i++) {
				func += (gradient[i][coindex])*((VCBfloat)(c_inuse[i]-coords[i]));
			}
			func -= vcbops_convert(&((char*)rawdata)[cindex * vcbSizeOf(datatype)], datatype);
			func *= func; /* squared */

			/* add to sum */
			sum += func;
			count += 1.0f;

			/* increase index (coords) */
			npts[0]++;
			for (i = 0; i < ndims; i++) {
				if (npts[i] > 2) {
					npts[i] = 0;
					npts[i+1]++;
				}
			}
		}

		/* calculate result */
		result[vcbops_dfindex(ndims, dsizes, coords)] = sum / count;

		/* increase index (coords) */
		coords[0]++;
		for (i = 0; i < ndims; i++) {
			if (coords[i] >= dsizes[i]) {
				coords[i] = 0;
				coords[i+1]++;
			}
		}
	}
}
