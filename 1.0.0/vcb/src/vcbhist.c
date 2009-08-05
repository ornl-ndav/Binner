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
#include <float.h>
#include "vcbhist.h"
#include "vcbutils.h"

/*
 * ============================================================================================================
 *  vr sparse histogramm function to build and combine volume information to multidimensional histogramm space
 * ============================================================================================================
 */

#ifndef VCB_HIST_DEBUG
#define VCB_HIST_DEBUG if (0)
#endif

static double vcbHistConvert(void *data, vcbdatatype datatype) {
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

/**
 * "vcbHistAssign"
 */
vcbHistFile vcbHistAssign(char *filename) {
	vcbHistFile hf;
	int i, ncells, tmpi, *coords, *bbox, clusterno, fileseekids;
/*	float tmpf;*/
	long tmp_fpos;

	/* initialize vcbHistFile */
	hf = (struct histfilepointer *)malloc(sizeof(struct histfilepointer));
	hf->filename = filename;
	hf->fp = fopen(hf->filename, "rb");
	hf->status = (hf->fp != NULL);
	hf->nclusters = 0;
	if (!hf->status) return NULL;

	/* read endian byte */
	fread(&hf->file_endian, sizeof(unsigned char), 1, hf->fp);
	hf->change_endian = (vcbBigEndian() == hf->file_endian) ? VCB_USECUR_ENDIAN : VCB_USEINV_ENDIAN;

	/* read clustered-tag byte */
	fread(&hf->clustag, sizeof(unsigned char), 1, hf->fp);

	/* read dimension sizes of original volume*/
	fread(&hf->vol_dims, sizeof(int), 1, hf->fp);
	if (hf->change_endian == VCB_USEINV_ENDIAN)
		vcbToggleEndian(&hf->vol_dims,sizeof(int));
	hf->vol_dsize = (int *)malloc(sizeof(int)*hf->vol_dims);
	fread(hf->vol_dsize, sizeof(int), hf->vol_dims, hf->fp);
	for (i = 0; i < hf->vol_dims; i++) {
		if (hf->change_endian == VCB_USEINV_ENDIAN) 
			vcbToggleEndian(&hf->vol_dsize[i], sizeof(int));
	}
	
	/* read number of dimensions */
	fread(&hf->ndims, sizeof(int), 1, hf->fp); 
	if (hf->change_endian == VCB_USEINV_ENDIAN)
		vcbToggleEndian(&hf->ndims,sizeof(int));

	/* read dimension data (number of bins, datatype, max value, min value)*/
	hf->nbin = (int *)malloc(sizeof(int)*hf->ndims);
	hf->dtype = (vcbdatatype *)malloc(sizeof(vcbdatatype)*hf->ndims);
	hf->min = (double *)malloc(sizeof(double)*hf->ndims);
	hf->max = (double *)malloc(sizeof(double)*hf->ndims);
	for (i = 0; i < hf->ndims; i++) {
		fread(&hf->nbin[i], sizeof(int), 1, hf->fp);
		if (hf->change_endian == VCB_USEINV_ENDIAN)
			vcbToggleEndian(&hf->nbin[i], sizeof(int));

		fread(&hf->dtype[i], sizeof(vcbdatatype), 1, hf->fp);
		if (hf->change_endian == VCB_USEINV_ENDIAN)
			vcbToggleEndian(&hf->dtype[i], sizeof(vcbdatatype));

		fread(&hf->min[i], vcbSizeOf(hf->dtype[i]), 1, hf->fp);
		if (hf->change_endian == VCB_USEINV_ENDIAN)
			vcbToggleEndian(&hf->min[i], vcbSizeOf(hf->dtype[i]));

		fread(&hf->max[i], vcbSizeOf(hf->dtype[i]), 1, hf->fp);
		if (hf->change_endian == VCB_USEINV_ENDIAN)
			vcbToggleEndian(&hf->max[i], vcbSizeOf(hf->dtype[i]));
	}

	/* determine number of clusters */
	tmp_fpos = ftell(hf->fp); /* store file pointer */
	while (feof(hf->fp) == 0) {
		/* read over coordinates, bounding box, and number of cells*/
		for (i=0; i < hf->ndims; i++) fread(&tmpi, sizeof(int), 1, hf->fp);
		if (hf->clustag != 0) for (i=0; i < 2*hf->ndims; i++) fread(&tmpi, sizeof(int), 1, hf->fp);
		fread(&ncells, sizeof(int), 1, hf->fp);
		if (hf->change_endian == VCB_USEINV_ENDIAN)
			vcbToggleEndian(&ncells, sizeof(int));

		/* read over ids */
		for (i=0; i < ncells; i++) 
			fread(&tmpi, sizeof(int), 1, hf->fp);

		if (feof(hf->fp) !=0) break;

		/* increase cluster number */
		hf->nclusters++;
	}

	/* allocate memory */
	coords = (int *)malloc((hf->ndims)*sizeof(int));
	bbox = (int *)malloc(2*(hf->ndims)*sizeof(int));
	hf->cluster = (int **) malloc(sizeof(int *) * hf->nclusters); /* allocate space for pointers */
	clusterno=0;

	/* read in start clusters */
	fseek(hf->fp, tmp_fpos, SEEK_SET); /* set file pointer */
	clearerr(hf->fp); /* clear all file error and eof flags */
	while (feof(hf->fp) == 0) {
		/* read coordinates, bounding box, and number of cells*/
		for (i=0; i < hf->ndims; i++) {
			fread(&coords[i], sizeof(int), 1, hf->fp);
			if (hf->change_endian == VCB_USEINV_ENDIAN)
				vcbToggleEndian(&coords[i], sizeof(int));
		}
		if (hf->clustag != 0) {
			for (i=0; i < 2*hf->ndims; i++) {
				fread(&bbox[i], sizeof(int), 1, hf->fp);
				if (hf->change_endian == VCB_USEINV_ENDIAN)
					vcbToggleEndian(&bbox[i], sizeof(int));
			}
		} else {
			for (i=0; i < 2*hf->ndims; i++) {
				bbox[i] = coords[i%hf->ndims];
			}
		}
		fread(&ncells, sizeof(int), 1, hf->fp);
		if (hf->change_endian == VCB_USEINV_ENDIAN)
			vcbToggleEndian(&ncells, sizeof(int));
		fileseekids = (int)ftell(hf->fp);

		if (feof(hf->fp) != 0) break;

		/* read over ids */
		for (i=0; i < ncells; i++) 
			fread(&tmpi, sizeof(int), 1, hf->fp);

		/* allocate space for new start cluster */
		hf->cluster[clusterno] = (int *)malloc(sizeof(int)*(3*hf->ndims+3));
		
		/* fill in information*/
		for (i=0; i < hf->ndims; i++) hf->cluster[clusterno][i] = coords[i];
		for (i=0; i < 2*hf->ndims; i++) hf->cluster[clusterno][hf->ndims+i] = bbox[i];
		hf->cluster[clusterno][3*hf->ndims] = clusterno;
		hf->cluster[clusterno][3*hf->ndims+1] = ncells;
		hf->cluster[clusterno][3*hf->ndims+2] = fileseekids;

		/* increase clusterno */
		clusterno++;
	}
	free(coords);
	free(bbox);

	return hf;
}

/**
 * "vcbHistClose"
 */
void vcbHistClose(vcbHistFile hf) {
	int i;

	if (!hf) return;
	if (!hf->status) return;
	fclose(hf->fp);
	hf->status = 0;

	for(i = 0; i < hf->nclusters; i++) free(hf->cluster[i]);

	free(hf->cluster);

	free(hf->nbin);
	free(hf->dtype);
	free(hf->min);
	free(hf->max);
	
	return;
}

/**
 * "vcbHistGetNumClusters"
 */
int vcbHistGetNumClusters(vcbHistFile hf) {
	if (!hf) return 0;
	if (!hf->status) return 0;
	return hf->nclusters;
}
	
/**
 * "vcbHistGetClusterSize"
 */
int vcbHistGetClusterSize(vcbHistFile hf, int clusterno) {
	if (!hf) return 0;
	if (!hf->status) return 0;
	if (clusterno >= hf->nclusters) return 0;

	return hf->cluster[clusterno][3*hf->ndims+1];
}

/**
 * "vcbHistGetCluster"
 */
int *vcbHistGetCluster(vcbHistFile hf, int clusterno) {
	int *c, i;

	if (!hf) return NULL;
	if (!hf->status) return NULL;
	if (clusterno >= hf->nclusters) return NULL;

	fseek(hf->fp, hf->cluster[clusterno][3*hf->ndims+2], SEEK_SET); /* set file pointer */
	clearerr(hf->fp); /* clear all file error and eof flags */

	c = (int *)malloc(sizeof(int)*(1+hf->cluster[clusterno][3*hf->ndims+1]));
	if (!c) return NULL;

	for (i = 0; i < hf->cluster[clusterno][3*hf->ndims+1]; i++) {
		fread(&c[i], sizeof(int), 1, hf->fp);
		if (hf->change_endian == VCB_USEINV_ENDIAN)
			vcbToggleEndian(&c[i], sizeof(int));
	}
	c[hf->cluster[clusterno][3*hf->ndims+1]+1] = -1;

	return c;
}

unsigned char vcbHistIsBigendian(vcbHistFile hf) {
	return hf->file_endian;
}

int vcbHistGetNumVolDims(vcbHistFile hf) {
	return hf->vol_dims;
}

int *vcbHistGetVolDims(vcbHistFile hf) {
	return hf->vol_dsize;
}

int vcbHistGetNumHistDims(vcbHistFile hf) {
	return hf->ndims;
}

int *vcbHistGetHistDims(vcbHistFile hf) {
	return 	hf->nbin;
}

vcbdatatype vcbHistGetDatatype(vcbHistFile hf, int dim) {
	return hf->dtype[dim];
}

double vcbHistGetMin(vcbHistFile hf, int dim) {
	return hf->min[dim];
}

double vcbHistGetMax(vcbHistFile hf, int dim) {
	return hf->max[dim];
}

int *vcbHistGetClusterRange(vcbHistFile hf, int clusterno) {
	int *range = malloc(sizeof(int)*2*hf->ndims), i;
	for (i = 0; i < hf->ndims; i++) {
		range[          i] = hf->cluster[clusterno][  hf->ndims+i];
		range[hf->ndims+i] = hf->cluster[clusterno][2*hf->ndims+i];
	}
	return range;
}

/**
 * "vcbSparseHistAutoRange" 
 */
int vcbSparseHistAutoRange(char *binfile, char *histfile_in, char *histfile_out, int nbins, int threshold, int attrib) {
	return vcbSparseHist(binfile, histfile_in, histfile_out, 0.0f, 0.0f, nbins, threshold, attrib);
}


/**
 * "vcbSparseHist" 
 */
int vcbSparseHist(char *binfile, char *histfile_in, char *histfile_out, double lower, double upper, int nbins, int threshold, int attrib) {
	int nattribs, bdims, *bdsize, i, j, rawsize, ndims, endian, nbin_tmp, *coords, change_endian, ncells, **bins, currentcell, maxcbin, histinbdims, *histinbdsize, *cellc, dummy;
	long tmp_fpos;
	vcbdatatype dtype, dtype_tmp;
	char * raw, errormsg [100], * sdtype, * minp, *maxp, min_tmp[8], max_tmp[8];
	double minv, maxv, eval, * brange, range, currentvalue;
	unsigned char file_endian, clustag;
	FILE * datain, * dataout;

	datain = NULL;
	coords = NULL;
	bdsize = (int *)malloc(sizeof(int)*100);
	endian = vcbBigEndian();
	clustag = 0;

	/* determine datatype by file extension */
	sdtype = strrchr(binfile, '.');
	if (sdtype == NULL) {
		sprintf(errormsg,"cannot determine datatype of %s\n", binfile);
		perror(errormsg);
		return 1;
	}
	sdtype++;

	dtype = -2;
	if (strcmp(sdtype,"ub") == 0) dtype = VCB_UNSIGNEDBYTE;
	if (strcmp(sdtype,"b")  == 0) dtype = VCB_BYTE;
	if (strcmp(sdtype,"us") == 0) dtype = VCB_UNSIGNEDSHORT;
	if (strcmp(sdtype,"s")  == 0) dtype = VCB_SHORT;
	if (strcmp(sdtype,"ui") == 0) dtype = VCB_UNSIGNEDINT;
	if (strcmp(sdtype,"i")  == 0) dtype = VCB_INT;
	if (strcmp(sdtype,"f")  == 0) dtype = VCB_FLOAT;
	if (strcmp(sdtype,"d")  == 0) dtype = VCB_DOUBLE;
	if (strcmp(sdtype,"bf") == 0) dtype = VCB_BITFIELD;
	if (dtype == -2) {
		sprintf(errormsg,"cannot determine datatype of %s\n", binfile);
		perror(errormsg);
		return 1;
	}

	/* read in rawdata file */
	raw = vcbReadBinf(binfile, dtype, vcbBigEndian(), &bdims, bdsize, &nattribs);

	if (raw == NULL) {
		sprintf(errormsg,"cannot open %s to read", binfile);
		perror(errormsg);
		return 1;
	}

	/* determine min/max values of rawdata if not given by (lower, upper) */
	rawsize = 1;
	for (i = 0; i < bdims; i++) 
		rawsize *= bdsize[i];

	if (lower == upper) {
		minv = 1E+38;
		maxv = -1E+38;

		for (j = 0; j < rawsize; j++) {
			eval = vcbHistConvert(&raw[j * vcbSizeOf(dtype) * nattribs + vcbSizeOf(dtype) * attrib], dtype);
			if (eval < minv) {
				minv = eval;
				minp = &raw[j * vcbSizeOf(dtype) * nattribs + vcbSizeOf(dtype) * attrib];
			}
			if (eval > maxv) {
				maxv = eval;
				maxp = &raw[j * vcbSizeOf(dtype) * nattribs + vcbSizeOf(dtype) * attrib];
			}
		}

		if (maxv == minv) nbins = 1;
	} else {
		minv = lower;
		minp = (char *)&minv;
		maxv = upper;
		maxp = (char *)&maxv;
	}
	printf("\nmin-max: %f - %f\n", minv, maxv);

	/* open file for output */
	dataout = fopen(histfile_out,"wb");
	if (dataout == NULL) {
		sprintf(errormsg,"cannot open %s to write\n", histfile_out);
		perror(errormsg);
		return 1;
	}

	/* read in original histogram file header (if not NULL), and write header of output file */
	if (!((histfile_in == NULL) || (strcmp(histfile_in,"") == 0) || (strcmp(histfile_in,"NULL") == 0))) {
		/* open file for input */
		datain = fopen(histfile_in,"rb");
		if (datain == NULL) {
			sprintf(errormsg,"cannot open %s to read", histfile_in);
			perror(errormsg);
			return 1;
		}

		/* read and write endian byte */
		fread(&file_endian, sizeof(unsigned char), 1, datain);
		change_endian = (endian == file_endian) ? VCB_USECUR_ENDIAN : VCB_USEINV_ENDIAN;
		fwrite(&endian, sizeof(unsigned char), 1, dataout);

		/* write and read clustered-tag byte */
		fwrite(&clustag, sizeof(unsigned char), 1, dataout);
		fread(&clustag, sizeof(unsigned char), 1, datain);
		
		/* read, compare, and write dimension sizes of original volume data */
		fread(&histinbdims, sizeof(int), 1, datain);
		if (change_endian == VCB_USEINV_ENDIAN) 
			vcbToggleEndian(&histinbdims,sizeof(int));
		histinbdsize = (int *)malloc(sizeof(int)*histinbdims);
		fread(histinbdsize, sizeof(int), histinbdims, datain);
		for (i = 0; i < histinbdims; i++) {
			if (change_endian == VCB_USEINV_ENDIAN) 
				vcbToggleEndian(&histinbdsize[i], sizeof(int));
			if (histinbdsize[i] != bdsize[i]) {
				sprintf(errormsg,"dimension sizes for volume space of histogram file and binary data file do not match\n");
				perror(errormsg);
				return 1;
			}
		}
		fwrite(&bdims, sizeof(int), 1, dataout);
		fwrite(bdsize, sizeof(int), bdims, dataout);
		
		/* read and write number of dimensions */
		fread(&ndims, sizeof(int), 1, datain); 
		if (change_endian == VCB_USEINV_ENDIAN) 
			vcbToggleEndian(&ndims,sizeof(int));
		ndims++; /* increase number of dimensions accordingly */
		fwrite(&ndims, sizeof(int), 1, dataout); 	
		VCB_HIST_DEBUG printf("%d\n%d\n", endian, ndims);

		/* read and write dimension data (number of bins, datatype, max value, min value)*/
		for (i = 0; i <(ndims-1); i++) {
			fread(&nbin_tmp, sizeof(int), 1, datain);
			if (change_endian == VCB_USEINV_ENDIAN)
				vcbToggleEndian(&nbin_tmp, sizeof(int));
			fread(&dtype_tmp, sizeof(vcbdatatype), 1, datain);
			if (change_endian == VCB_USEINV_ENDIAN)
				vcbToggleEndian(&dtype_tmp, sizeof(vcbdatatype));
			fread(&min_tmp, vcbSizeOf(dtype_tmp), 1, datain);
			if (change_endian == VCB_USEINV_ENDIAN)
				vcbToggleEndian(&min_tmp, vcbSizeOf(dtype_tmp));
			fread(&max_tmp, vcbSizeOf(dtype_tmp), 1, datain);
			if (change_endian == VCB_USEINV_ENDIAN)
				vcbToggleEndian(&max_tmp, vcbSizeOf(dtype_tmp));
			fwrite(&nbin_tmp, sizeof(int), 1, dataout);
			fwrite(&dtype_tmp, sizeof(vcbdatatype), 1, dataout);
			fwrite(&min_tmp, vcbSizeOf(dtype_tmp), 1, dataout);
			fwrite(&max_tmp, vcbSizeOf(dtype_tmp), 1, dataout);
			VCB_HIST_DEBUG printf("%d,%d,%d,%d\n", nbin_tmp, dtype_tmp, min_tmp, max_tmp);
		}
	} else {
		/* if no input file, write basic header */
		ndims = 1;
		fwrite(&endian, sizeof(unsigned char), 1, dataout);
		fwrite(&clustag, sizeof(unsigned char), 1, dataout);
		fwrite(&bdims, sizeof(int), 1, dataout);
		fwrite(bdsize, sizeof(int), bdims, dataout);
		fwrite(&ndims, sizeof(int), 1, dataout); 	
		VCB_HIST_DEBUG printf("%d\n%d\n", endian, ndims);
	}

	/* write new dimension data (as above) */
	fwrite(&nbins, sizeof(int), 1, dataout);
	fwrite(&dtype, sizeof(vcbdatatype), 1, dataout);
	fwrite(&minp, vcbSizeOf(dtype), 1, dataout);
	fwrite(&maxp, vcbSizeOf(dtype), 1, dataout);
	VCB_HIST_DEBUG printf("%d,%d,%f,%f\n", nbins, dtype, minv, maxv);

	/* compute ranges for bins */
	range = (maxv - minv) / ((double)nbins);
	brange = (double *)malloc((nbins)*sizeof(double));
	for (i = 0; i < nbins; i++)
		brange[i] = minv + ((double)(i+1))*range;
	brange[nbins-1] = maxv;

	/* if histogram needs to be extended, do so ... */
	if (ndims != 1) {
		coords = (int *)malloc((ndims-1)*sizeof(int));
		while (feof(datain) == 0) {
			/* read coordinates and number of cells*/
			for (i=0; i < (ndims-1); i++) {
				fread(&coords[i], sizeof(int), 1, datain);
				if (change_endian == VCB_USEINV_ENDIAN)
					vcbToggleEndian(&coords[i], sizeof(int));
			}
			if (clustag != 0) for (i=0; i < 2*(ndims-1); i++) fread(&dummy, sizeof(int), 1, datain);
			tmp_fpos = ftell(datain); /* store file pointer */
			fread(&ncells, sizeof(int), 1, datain);
			if (change_endian == VCB_USEINV_ENDIAN)
				vcbToggleEndian(&ncells, sizeof(int));

			/* determine maximum number of cells in one bin */
			cellc = (int *)calloc(nbins, sizeof(int));
			maxcbin = 0;
			for (i = 0; i < ncells; i++) {
				fread(&currentcell, sizeof(int), 1, datain);
				if (change_endian == VCB_USEINV_ENDIAN)
					vcbToggleEndian(&currentcell, sizeof(int));
				currentvalue = vcbHistConvert(&raw[vcbSizeOf(dtype) * nattribs * currentcell + vcbSizeOf(dtype) * attrib], dtype);
				if ((currentvalue >= minv) && (currentvalue <= maxv)) {
					j = 0;
					while (brange[j] < currentvalue) j++;
					cellc[j]++;
					if (maxcbin < cellc[j]) maxcbin = cellc[j];
				}
			}
			clearerr(datain); /* clear all file error and eof flags */
			fseek(datain, tmp_fpos, SEEK_SET); /* restore file pointer */
			fread(&ncells, sizeof(int), 1, datain);
			if (change_endian == VCB_USEINV_ENDIAN)
				vcbToggleEndian(&ncells, sizeof(int));


			/* allocate space for bins */
			bins = (int **)malloc(sizeof(int *) * nbins);
			for (i = 0; i < nbins; i++) {
				bins[i] = (int *)calloc(cellc[i]+1,sizeof(int));
				if (!bins[i]) {
					sprintf(errormsg,"Cannot allocate memory for bins (%d out of %d failed)", i, nbins);
					perror(errormsg);
					return 1;
				}
			}

			if (feof(datain) == 0) {
				/* distribute cells among bins */
				for (i = 0; i < ncells; i++) {
					fread(&currentcell, sizeof(int), 1, datain);
					if (change_endian == VCB_USEINV_ENDIAN)
						vcbToggleEndian(&currentcell, sizeof(int));
					currentvalue = vcbHistConvert(&raw[vcbSizeOf(dtype) * nattribs * currentcell + vcbSizeOf(dtype) * attrib], dtype);
					if ((currentvalue >= minv) && (currentvalue <= maxv)) {
						j = 0;
						while (brange[j] < currentvalue) j++;
						bins[j][++bins[j][0]] = currentcell;
					}
				}

				/* write cells to disk with regard to threshold */
				for (i = 0; i < nbins; i++) {
					if (bins[i][0] > threshold){
						fwrite(&coords[0], sizeof(int), ndims - 1, dataout);
						fwrite(&i, sizeof(int), 1, dataout);
						fwrite(&bins[i][0], sizeof(int), bins[i][0]+1, dataout);
						VCB_HIST_DEBUG printf(""); for (j = 0; j < (ndims-1); j++) VCB_HIST_DEBUG printf("%d,", coords[j]);VCB_HIST_DEBUG printf("%d, %d\n", i, bins[i][0]);
						/* for (j = 0; j < bins[i][0]+1; j++)
						   VCB_HIST_DEBUG printf(",%d", bins[i][j]); VCB_HIST_DEBUG printf("\n\n\n");*/
					}
				}
			}
			/* free bin space */
			for (i = 0; i < nbins; i++) free(bins[i]);
			free(bins);
		}
	}
	/* ... or create new histogram */
	else {
		/* determine maximum number of cells in one bin */
		cellc = (int *)calloc(nbins, sizeof(int));
		maxcbin = 0;
		for (i = 0; i < rawsize; i++) {
			currentvalue = vcbHistConvert(&raw[vcbSizeOf(dtype) * nattribs * i + vcbSizeOf(dtype) * attrib], dtype);
			if ((currentvalue >= minv) && (currentvalue <= maxv)) {
				j = 0;
				while (brange[j] < currentvalue) j++;
				cellc[j]++;
				if (maxcbin < cellc[j]) maxcbin = cellc[j];
			}
		}


		/* allocate space for bins */
		bins = (int **)malloc(sizeof(int *) * nbins);
		for (i = 0; i < nbins; i++) {
			bins[i] = (int *)calloc(/*maxcbin*/cellc[i]+1,sizeof(int));
			if (!bins[i]) {
				sprintf(errormsg,"Cannot allocate memory for bins (%d out of %d failed)", i, nbins);
				perror(errormsg);
				return 1;
			}
		}

		
		/* distribute cells among bins */
		for (i = 0; i < rawsize; i++) {
			currentvalue = vcbHistConvert(&raw[vcbSizeOf(dtype) * nattribs * i + vcbSizeOf(dtype) * attrib], dtype);
			if ((currentvalue >= minv) && (currentvalue <= maxv)) {
				j = 0;
				while (brange[j] < currentvalue) j++;
				bins[j][++bins[j][0]] = i;
			}
		}


		/* write cells to disk with regard to threshold */
		for (i = 0; i < nbins; i++) {
			if (bins[i][0] > threshold){
				fwrite(&i, sizeof(int), 1, dataout);
				fwrite(&bins[i][0], sizeof(int), bins[i][0]+1, dataout);
				VCB_HIST_DEBUG printf("%d, %d\n", i, bins[i][0]); 
				/*for (j = 0; j < bins[i][0]+1; j++)
				  VCB_HIST_DEBUG printf(",%d", bins[i][j]); VCB_HIST_DEBUG printf("\n\n\n");*/
			}
		}

		/* free bin space */
		for (i = 0; i < nbins; i++) free(bins[i]);
		free(bins);
	}

	/* clean up */
	free(cellc);
	free(bdsize);
	if (coords != NULL) free(coords);
	free(brange);
	if (datain != NULL) fclose(datain);
	fclose(dataout);

	/* report success */
	return 0;
}

