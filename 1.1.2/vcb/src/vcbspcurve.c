/***************************************************************************
 *   Copyright (C) 2004  Jian Huang                                        *
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

#include <string.h>
#include <stdlib.h>
#include "vcbspcurve.h"


static unsigned char * bits;       /* global for z curve */
static bitmask_t *point;           /* global for hilbert curve */
static unsigned int nBits, nDims;  /* global for all space curves */
static firstime;                   /* global for all space curves */

int vcbScurveSize(int n_dims, int n_bits)
{
	int i, sizedim, size;

	/* calc # voxels in each dimension */
	for (i = 0, sizedim = 1; i < n_bits; sizedim *= 2, i++)
		;

	/* calc # voxels along the whole space filling curve */
	for (i = 0, size = 1; i < n_dims; size *= sizedim, i++)
		;

	return size;

}

void vcbGenZscurve(int n_dims, int n_bits, int * coords)
{
	/*unsigned int nBits;*/
	int i, size;
    /*unsigned char *bits;*/

	nBits = (unsigned int)(n_bits * n_dims);
	bits = (unsigned char *)malloc(nBits * sizeof(unsigned char));
    memset(bits, 0, nBits * sizeof(unsigned char));

	size = vcbScurveSize(n_dims,n_bits);

	/* the first vertex is always at zero coordinates */
	memset(coords, 0, n_dims*sizeof(int));

	for (i=1; i<size; i++)
		zcurve_incr((unsigned int)n_dims, nBits, bits, & coords[i*n_dims]);

	free(bits);
}

void vcbGenHscurve(int n_dims, int n_bits, int * coords)
{
	int i, j, size;

	point = (bitmask_t *)malloc(n_dims * sizeof(bitmask_t));
    memset(point, 0, n_dims * sizeof(bitmask_t));

	nBits = (unsigned int)(n_bits * n_dims);

	size = vcbScurveSize(n_dims,n_bits);

	/* the first vertex is always at zero coordinates */
	memset(coords, 0, n_dims*sizeof(int));

	for (i=1; i<size; i++) {
		hilbert_incr(n_dims, nBits, point);
		for (j = 0; j < n_dims; j ++)
			coords[i*n_dims+j] = (int) point[j][0];
	}

	free(point);
}



void vcbBeginIncrZscurve(int n_dims, int n_bits)
{
	nBits = (unsigned int)(n_bits * n_dims);
	nDims = (unsigned int) n_dims;
	bits = (unsigned char *)malloc(nBits * sizeof(unsigned char));
    memset(bits, 0, nBits * sizeof(unsigned char));

	firstime = 1;
}

void vcbIncrZscurve(int * coords)
{
	if (firstime) {
		/* the first vertex is always at zero coordinates */
		memset(coords, 0, nDims*sizeof(int));
		firstime = 0;
	}
	else
		zcurve_incr(nDims, nBits, bits, coords);
}

void vcbEndIncrZscurve()
{
	free(bits);
}



void vcbBeginIncrHscurve(int n_dims, int n_bits)
{

	point = (bitmask_t *)malloc(n_dims * sizeof(bitmask_t));
    memset(point, 0, n_dims * sizeof(bitmask_t));
	
	nBits = (unsigned int)(n_bits * n_dims);
	nDims = (unsigned int) n_dims;

	firstime = 1;
}

void vcbIncrHscurve(int * coords)
{
	if (firstime) {
		/* the first vertex is always at zero coordinates */
		memset(coords, 0, nDims*sizeof(int));
		firstime = 0;
	}
	else {
		unsigned int j;
		hilbert_incr(nDims, nBits, point);
		for (j = 0; j < nDims; j ++)
			coords[j] = (int) point[j][0];
	}
}

void vcbEndIncrHscurve()
{
	free(point);
}

