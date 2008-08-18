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
/*#include <string.h>*/
#include <stdio.h>
/*#include <float.h>*/
#include "vcbhist.h"
#include "vcbrange.h"
#include "vcbutils.h"

/*
 * ============================================================================================================
 *  
 * ============================================================================================================
 */

#ifndef VCB_RANGE_DEBUG
#define VCB_RANGE_DEBUG if (0)
#endif

vcbRange vcbRangeGetFull(int ndims) {
	vcbRange a;
	int i;

	a = (struct rangepointer *)malloc(sizeof(struct rangepointer));
	a->ndims = ndims;
	a->ranges = (int **)malloc(sizeof(int *) * ndims);
	for (i=0; i<ndims; i++) {
		a->ranges[i] = (int *)calloc(2, sizeof(int));
		a->ranges[i][1] = 2147483647;
	}

	return a;
}

vcbRange vcbRangeGetEmpty(int ndims) {
	vcbRange a;
	int i;

	a = vcbRangeGetFull(ndims);
	for (i=0; i < ndims; i++) a->ranges[i][1] = -1;

	return a;
}

int vcbRangeIsEmpty(vcbRange r) {
	int i;

	for(i=0; i < r->ndims; i++) {
		if (r->ranges[i][0] > r->ranges[i][1]) return 1;
		if (r->ranges[i][1] < 0) return 1;
	}

	return 0;
}

void vcbRangeSetDim(vcbRange r, int dim, int lower, int upper) {
	if (r->ndims <= dim) return;
	
	r->ranges[dim][0] = lower;
	r->ranges[dim][1] = upper;
}

void vcbRangeFreeDim(vcbRange r, int dim) {
	vcbRangeSetDim(r, dim, 0, 2147483647);
}

void vcbRangeFree(vcbRange r) {
	int i;

	for(i=0; i < r->ndims; i++) free(r->ranges[i]);
	free(r->ranges);
}

vcbRange vcbRangeExtract(vcbHistFile hf, int clusternr) {
	vcbRange a;
	int i;

	a = vcbRangeGetFull(hf->ndims);

	for (i = 0; i < hf->ndims; i++) {
		a->ranges[i][0] = hf->cluster[clusternr][  hf->ndims+i];
		a->ranges[i][1] = hf->cluster[clusternr][2*hf->ndims+i];
	}

	return a;
}

void vcbRangeUnion(vcbRange result, vcbRange second) {
	int i;

	for(i = 0; i < result->ndims; i++) {
		if (result->ranges[i][0] > second->ranges[i][0]) result->ranges[i][0] = second->ranges[i][0];
		if (result->ranges[i][1] < second->ranges[i][1]) result->ranges[i][1] = second->ranges[i][1];
	}
}


void vcbRangeIntersection(vcbRange result, vcbRange second) {
	int i;

	for(i = 0; i < result->ndims; i++) {
		if (result->ranges[i][0] < second->ranges[i][0]) result->ranges[i][0] = second->ranges[i][0];
		if (result->ranges[i][1] > second->ranges[i][1]) result->ranges[i][1] = second->ranges[i][1];
	}
}

int vcbRangeGetDimLower(vcbRange r, int dim) {
	if (dim > r->ndims) return -1;

	return r->ranges[dim][0];
}

int vcbRangeGetDimUpper(vcbRange r, int dim) {
	if (dim > r->ndims) return -1;

	return r->ranges[dim][1];
}

int vcbGetClusters(vcbHistFile hf, vcbRange r, vcbClusterMode mode, int** clusterlist) {
	vcbRange *cranges;
	int *clist;
	int nclusters, i, j, k = 0;
	int n_out = 0, n_border = 0, n_in;

	if (r->ndims != vcbHistGetNumHistDims(hf)) return 0;

	*clusterlist = NULL;
	nclusters = vcbHistGetNumClusters(hf);
	n_in = nclusters;
	cranges = (vcbRange *)malloc(2*nclusters * sizeof(vcbRange));
	clist = (int *)malloc(nclusters * sizeof(int));

	for(i = 0; i < nclusters; i++) {
		clist[i] = 1;
		cranges[i] = vcbRangeExtract(hf, i);
		cranges[nclusters+i] = vcbRangeExtract(hf, i);
		vcbRangeIntersection(cranges[nclusters+i], r);
		if (vcbRangeIsEmpty(cranges[nclusters+i])) {
			n_out++; n_in--;
			clist[i] = -1;
		}
	}

	for (i = 0; i < nclusters; i++) {
		for (j = 0; j < r->ndims; j++) {
			if (clist[i] == 1) {
				if ((cranges[i]->ranges[j][0] != cranges[nclusters+i]->ranges[j][0]) || 
					(cranges[i]->ranges[j][0] != cranges[nclusters+i]->ranges[j][0])) {
					n_border++; n_in--;
					clist[i] = 0;
				}
			}
		}
	}
	free(cranges);

	switch(mode) {
	case VCB_PARTIALLY_INSIDE:
		(*clusterlist) = (int *)malloc(sizeof(int)*(n_in+n_border));
		for (i = 0; i < nclusters; i++) if (clist[i] >=0) (*clusterlist)[k++] = i;
		free(clist);
		return (n_in+n_border);
		break;
	case VCB_FULLY_INSIDE:
		(*clusterlist) = (int *)malloc(sizeof(int)*(n_in));
		for (i = 0; i < nclusters; i++) if (clist[i] >0) (*clusterlist)[k++] = i;
		free(clist);
		return (n_in);
		break;
    case VCB_PARTIALLY_OUTSIDE:
		(*clusterlist) = (int *)malloc(sizeof(int)*(n_out+n_border));
		for (i = 0; i < nclusters; i++) if (clist[i] <=0) (*clusterlist)[k++] = i;
		free(clist);
		return (n_out+n_border);
		break;
	case VCB_FULLY_OUTSIDE:
		(*clusterlist) = (int *)malloc(sizeof(int)*(n_out));
		for (i = 0; i < nclusters; i++) if (clist[i] <0) (*clusterlist)[k++] = i;
		free(clist);
		return (n_out);
		break;
	}
    return 0;
}


