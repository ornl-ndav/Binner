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
#include <stdio.h>
#include <math.h>
#include "vcbcluster.h"
#include "vcbutils.h"

/*
 * ==========================================================================
 *  vr cluster function to cluster data in multidimensional histogram space
 * ==========================================================================
 */

#ifndef VCB_CLUSTER_DEBUG
#define VCB_CLUSTER_DEBUG if (0)
#endif

/**
 * "vr_clusterfunc"
 */
int vcbCluster(char *histfile_in, char *histfile_out, vcbClusterFunc function) {
#ifndef VCB_USECUR_ENDIAN
#define VCB_USECUR_ENDIAN 0
#endif
#ifndef VCB_USEINV_ENDIAN
#define VCB_USEINV_ENDIAN 1
#endif
#ifndef VCB_MAX_CLUSTER
#define VCB_MAX_CLUSTER 1000000
#endif
	char errormsg[100], min_tmp[8], max_tmp[8];
	FILE * datain, * dataout;
	int endian, change_endian, histinbdims, *histinbdsize, *coords, *bbox, ndims, nbin_tmp, ncells, clusterno, **cluster;
	int clust_check, fileseekids, currentcell, nclusters, i, j, k, kmin, kmax, cc, dens_lg, dens_no, mincells, maxcells;
/*	float dist; */
	unsigned char file_endian, clustag;
	vcbdatatype dtype_tmp;
	long tmp_fpos;

	/* init */
	endian = vcbBigEndian();
	clustag = 255;
	datain = NULL;
	dataout = NULL;
	nclusters = 0;

	/* open file for input */
	sprintf(errormsg,"cannot open %s to read", histfile_in);
	datain = fopen(histfile_in,"rb");
	if (datain == NULL) {
		perror(errormsg);
		return 1;
	}

	/* open file for output */
	sprintf(errormsg,"cannot open %s to write", histfile_out);
	dataout = fopen(histfile_out,"wb");
	if (dataout == NULL) {
		perror(errormsg);
		fclose(datain);
		return 1;
	}

	/* read and write endian byte */
	VCB_CLUSTER_DEBUG {printf("start reading in...\n"); fflush(stdout);}
	fread(&file_endian, sizeof(unsigned char), 1, datain);
	change_endian = (endian == file_endian) ? VCB_USECUR_ENDIAN : VCB_USEINV_ENDIAN;
	fwrite(&endian, sizeof(unsigned char), 1, dataout);

	/* write and read clustered-tag byte */
	fwrite(&clustag, sizeof(unsigned char), 1, dataout);
	fread(&clustag, sizeof(unsigned char), 1, datain);
	VCB_CLUSTER_DEBUG {printf("clustag: %d\n", clustag);} /*MG*/

	/* read, compare, and write dimension sizes of original volume data */
	fread(&histinbdims, sizeof(int), 1, datain);
	if (change_endian == VCB_USEINV_ENDIAN) 
		vcbToggleEndian(&histinbdims,sizeof(int));
	VCB_CLUSTER_DEBUG printf("%d\n", histinbdims); fflush(stdout);
	histinbdsize = (int *)malloc(sizeof(int)*histinbdims);
	fread(histinbdsize, sizeof(int), histinbdims, datain);
	for (i = 0; i < histinbdims; i++) {
		if (change_endian == VCB_USEINV_ENDIAN) 
			vcbToggleEndian(&histinbdsize[i], sizeof(int));
	}
	fwrite(&histinbdims, sizeof(int), 1, dataout);
	fwrite(histinbdsize, sizeof(int), histinbdims, dataout);
	
	/* read and write number of dimensions */
	fread(&ndims, sizeof(int), 1, datain); 
	if (change_endian == VCB_USEINV_ENDIAN) 
		vcbToggleEndian(&ndims,sizeof(int));
	fwrite(&ndims, sizeof(int), 1, dataout); 	
	VCB_CLUSTER_DEBUG printf("%d\n%d\n", endian, ndims); 

	/* read and write dimension data (number of bins, datatype, max value, min value)*/
	for (i = 0; i <ndims; i++) {
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
		VCB_CLUSTER_DEBUG printf("%d,%d,%d,%d\n", nbin_tmp, dtype_tmp, min_tmp, max_tmp);
	}

	/* determine number of clusters */
	tmp_fpos = ftell(datain); /* store file pointer */
	VCB_CLUSTER_DEBUG{printf("filepos: %d\n", tmp_fpos); fflush(stdout);}
	while (feof(datain) == 0) {
		/* read over coordinates, bounding box, and number of cells*/
		for (i=0; i < ndims; i++) {
			fread(&currentcell, sizeof(int), 1, datain);
		}
		if (clustag != 0) {
			for (i=0; i < 2*ndims; i++) {
				fread(&currentcell, sizeof(int), 1, datain);
			}
		}
		fread(&ncells, sizeof(int), 1, datain);
		if (change_endian == VCB_USEINV_ENDIAN)
			vcbToggleEndian(&ncells, sizeof(int));

		/* read over ids */
		for (i=0; i < ncells; i++) 
			fread(&currentcell, sizeof(int), 1, datain);

		if (feof(datain) !=0) break;

		/* increase cluster number */
		nclusters++;
	}
	VCB_CLUSTER_DEBUG printf("%d\n",nclusters); fflush(stdout);

	/* allocate memory */
	clusterno=0;
	coords = (int *)malloc((ndims)*sizeof(int));
	bbox = (int *)malloc((ndims)*2*sizeof(int));
	cluster = (int **) malloc(sizeof(int *) * 3 * nclusters); /* allocate space for pointers */
	if ((coords == NULL) || (bbox == NULL) || (cluster == NULL)) {
		perror("cannot allocate memory");
		fclose(datain);
		fclose(dataout);
		return 1;
	}

	/* read in start clusters */
	fseek(datain, tmp_fpos, SEEK_SET); /* set file pointer */
	clearerr(datain); /* clear all file error and eof flags */
	maxcells = 0; mincells = 2147483647; /* MAX_INT */
	while (feof(datain) == 0) {
		/* read coordinates, bounding box, and number of cells*/
		for (i=0; i < ndims; i++) {
			fread(&coords[i], sizeof(int), 1, datain);
			if (change_endian == VCB_USEINV_ENDIAN)
				vcbToggleEndian(&coords[i], sizeof(int));
		}
		if (clustag != 0) {
			for (i=0; i < 2*ndims; i++) {
				fread(&bbox[i], sizeof(int), 1, datain);
				if (change_endian == VCB_USEINV_ENDIAN)
					vcbToggleEndian(&bbox[i], sizeof(int));
			}
		} else {
			for (i=0; i < 2*ndims; i++) {
				bbox[i] = coords[i%ndims];
			}
		}
		fread(&ncells, sizeof(int), 1, datain);
		if (change_endian == VCB_USEINV_ENDIAN)
			vcbToggleEndian(&ncells, sizeof(int));
		fileseekids = (int)ftell(datain);

		if (feof(datain) != 0) break;

		/* read over ids */
		for (i=0; i < ncells; i++) 
			fread(&currentcell, sizeof(int), 1, datain);

		/* allocate space for new start cluster */
		cluster[clusterno] = (int *)malloc(sizeof(int)*(3*ndims+3));
		if (cluster[clusterno] == NULL) {
			perror("cannot allocate memory");
			fclose(datain);
			fclose(dataout);
			return 1;
		}
		
		/* fill in information*/
		for (i=0; i < ndims; i++) cluster[clusterno][i] = coords[i];
		for (i=0; i < 2*ndims; i++) cluster[clusterno][ndims+i] = bbox[i];
		cluster[clusterno][3*ndims] = clusterno;
		cluster[clusterno][3*ndims+1] = ncells;
		cluster[clusterno][3*ndims+2] = fileseekids;
		VCB_CLUSTER_DEBUG {for (i=0; i < 3*ndims+3; i++) printf("%d,", cluster[clusterno][i]); printf("\n");} /*MG*/

		/* calculate max and min cells */
		if (ncells > maxcells) maxcells = ncells;
		if (ncells < mincells) mincells = ncells;

		/* increase clusterno */
		clusterno++;
	}

	/* start clustering data */
	VCB_CLUSTER_DEBUG {printf("start clustering..."); fflush(stdout);}
	for (i=0; i < nclusters; i++) {
		for (j=0; j < nclusters; j++) {
			/* cluster check only if necessary*/
			if ((cluster[i][3*ndims] != cluster[j][3*ndims]) ) {
				clust_check = 0;
				dens_lg = -1;
				dens_no = -1;
				/*VCB_CLUSTER_DEBUG {if ((i%500==0) && (j%500==0)) printf("%d <-> %d\n", i, j);}*/
				/* evaluate cluster function */
				switch(function) {
				/* proximity functions */
				case VCB_PROXIMITY:
					clust_check = 1;
					for(k = 0; (k < ndims) && (clust_check); k++) {
						if (abs(cluster[i][k] - cluster[j][k]) > 1) 
							clust_check = 0;
					}
					break;
				case VCB_PROXIMITY_FUZZY:
					clust_check = 1;
					for(k = 0; (k < ndims) && (clust_check); k++) {
						if (abs(cluster[i][k] - cluster[j][k]) > 2) 
							clust_check = 0;
					}
					break;
				/* density functions */
				case VCB_DENSITY_10:
					dens_no = 10;
				case VCB_DENSITY_100:
					if (dens_no<0) dens_no = 100;
				case VCB_DENSITY_1000:
					if (dens_no<0) dens_no = 1000;
				case VCB_DENSITY_10000:
					if (dens_no<0) dens_no = 10000;
					dens_lg = 0;
				case VCB_DENSITY_LG_10:
					if (dens_no<0) dens_no = 10;
				case VCB_DENSITY_LG_100:
					if (dens_no<0) dens_no = 100;
				case VCB_DENSITY_LG_1000:
					if (dens_no<0) dens_no = 1000;
				case VCB_DENSITY_LG_10000:
					if (dens_no<0) dens_no = 10000;
					if (dens_lg<0) dens_lg = 1;

					/* fall-through: start density algorithm here */
					if (dens_lg == 0) { /* no log(...) */
						/* if i and j's density values are close enough */
						if (floor(((double)(cluster[i][3*ndims+1]-mincells))/((double)(maxcells-mincells))*((double)dens_no))
							== floor(((double)(cluster[j][3*ndims+1]-mincells))/((double)(maxcells-mincells))*((double)dens_no)))
								/* cluster them */
								clust_check=1;
					} else { /* with log(...) */
						/* if i and j's density values are close enough */
						if (floor((log(cluster[i][3*ndims+1]-mincells))/(log(maxcells-mincells))*((double)dens_no))
							== floor((log(cluster[j][3*ndims+1]-mincells))/(log(maxcells-mincells))*((double)dens_no)))
								/* cluster them */
								clust_check=1;
					}
	
					/* break at the end */
					break;
				}
				/* do clustering if necessary */
				if (clust_check) {
					kmin = cluster[i][3*ndims];
					kmax = cluster[j][3*ndims];

					/* compute new bounding box */
					for (k = 0; k < ndims; k++) {
						cluster[kmax][  ndims+k] = ((cluster[kmin][  ndims+k] < cluster[kmax][  ndims+k]) ? cluster[kmin][  ndims+k] : cluster[kmax][  ndims+k]);
						cluster[kmax][2*ndims+k] = ((cluster[kmin][2*ndims+k] > cluster[kmax][2*ndims+k]) ? cluster[kmin][2*ndims+k] : cluster[kmax][2*ndims+k]);
					}

					cc = cluster[kmin][3*ndims];
					for (k = 0; k < nclusters; k++) {
						if (cluster[k][3*ndims] == cc) {
							cluster[k][3*ndims] = cluster[kmax][3*ndims];
							/*cradius[k] = cradius[kmax];*/
						}
					}
					
					/*					
					kmin = ((cradius[cluster[i][ndims]] > cradius[cluster[j][ndims]]) ? cluster[j][ndims] : cluster[i][ndims]);
					kmax = ((cradius[cluster[i][ndims]] > cradius[cluster[j][ndims]]) ? cluster[i][ndims] : cluster[j][ndims]);
					
					&#47;* compute distance *
					dist = 0.f;
					for(k = 0; k < ndims; k++) {
						dist += (float)pow(cluster[kmax][k] - cluster[kmin][k],2.0f);
						dist = sqrt(dist);
					}

					if (fabs(cradius[kmax] - cradius[kmin]) >= dist) {
						&#47;* one circle encloses other circle *
						for (k = 0; k < nclusters; k++) {
							cc = cluster[kmin][ndims];
							if (cluster[k][ndims] == cc) {
								cluster[k][ndims] = cluster[kmax][ndims];
								&#47;*cradius[k] = cradius[kmax];*
							}
						}
					} else {
						&#47;* non-enclosing case *
						cradius[kmax] = cradius[kmin] + dist;
						for (k = 0; k < nclusters; k++) {
							cc = cluster[kmin][ndims];
							if (cluster[k][ndims] == cc) {
								cluster[k][ndims] = cluster[kmax][ndims];
								&#47;*cradius[k] = cradius[kmax];*
							}
						}
					}
					*/
				}
			}
		}
	}
	VCB_CLUSTER_DEBUG {printf("done!\n"); fflush(stdout);}
	VCB_CLUSTER_DEBUG {for(k=0; k<nclusters; k++){for (i=0; i < 3*ndims+3; i++) printf("%d,", cluster[k][i]); printf("\n");}} /*MG*/

	/* write clusters to output file */
	for (i=0; i < nclusters; i++) {
		if (cluster[i][3*ndims] == i) {
			/* write single cluster to output file */
			fwrite(&cluster[i][0], sizeof(int), 3*ndims, dataout);
			
			/* determine number of ids for cluster */
			for(j=0,ncells=0; j < nclusters; j++) 
				if (cluster[j][3*ndims] == i) 
					ncells += cluster[j][3*ndims+1];
			fwrite(&ncells, sizeof(int), 1, dataout);
			VCB_CLUSTER_DEBUG {printf("cluster to disk(id, #cells): %d,%d\n", i,ncells);}

			/* search and combine lists for cluster */
			for(j=0;j < nclusters; j++) {
				if (cluster[j][3*ndims] == i) {
					/* add ids to cluster */
					fseek(datain, cluster[j][3*ndims+2], SEEK_SET);
					clearerr(datain);
					for(k=0; k < cluster[j][3*ndims+1]; k++) {
						fread(&currentcell, sizeof(int), 1, datain);
						if (change_endian == VCB_USEINV_ENDIAN)
							vcbToggleEndian(&currentcell, sizeof(int));
						fwrite(&currentcell, sizeof(int), 1, dataout);
					}
				}
			}
		}
	}

	/* clean up */
	fclose(datain);
	fclose(dataout);
	free(histinbdsize);
	for (i=0; i < nclusters; i++) free(cluster[i]);
	free(cluster);
	free(coords);
	free(bbox);

	/* report success */
	return 0;
}
/*
int main(int args, char **argv) {
	if (args < 3) exit(0);
	return vcbCluster(argv[1], argv[2], VCB_DENSITY_10000);
}
*/

