#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "binnerio.h"
#include "vcbutils.h"

void printvertexf(float *v, int ndim)
{
	int i;
	assert(ndim <= 4);
	for (i = 0; i < ndim; printf("%f ",v[i]), i ++);
}

void printvertexd(double *v, int ndim)
{
	int i;
	assert(ndim <= 4);
	for (i = 0; i < ndim; printf("%f ",(float)(v[i])), i ++);
}

int getquada3d(float * vdata)
/* 
 * return value: num_vertices read in successfully
 *               -1 on failure
 * getquad_a_: 'a' for ascii
 */
{
	int i;
	
	/* assume a quad in 3d space. 4 vertices, x-y-z coord each */	
	for (i = 0; i < 4; scanf("\n"), vdata += 3, i ++)
		if (3 > scanf("%f %f %f", &vdata[0], &vdata[1], &vdata[2]))
			break;
	
	return i;
}

int binnerin3d(int nfacets, float *vdata)
{
	int i;
	for (i = 0; i < nfacets; i ++)
		getquada3d(&vdata[i*4*3]);

	return nfacets;
}

void binnerout3d(int nfacets, float *v)
{
	int i, j;

	for (i = 0; i < nfacets; i ++) {
		for (j = 0; j < 4; j ++)
			printvertexf(&v[i*4*3+j*3], 3); 
		printf("\n");
	}
}

int get_polygonf(float * v)
/* 
 * return value: num_vertices actually read in successfully
 */
{
	int i, n;
	
	if (scanf("%d", &n) <= 0) return -1; 

	/* assume a quad in 3d space. 4 vertices, x-y-z coord each */	
	for (i = 0; i < n; v += 3, i ++)
		if (3 > scanf("%f %f %f", v, v+1, v+2))
			break;
			
	scanf("\n");

	return i;
}

int get_pixelf(int * id, float * cnt, float * err, float (* v)[4])
/* 
 * always read in 8 vertices and 3 preceding values: sliceID, cnt, err
 * return value: num_vertices actually read in successfully
 */
{
	int i, n;
	
	if (scanf("%d", id) <= 0) return -1; 
	if (scanf("%f", cnt) <= 0) return -1; 
	if (scanf("%f", err) <= 0) return -1; 

	/* assume a 8 vertices, x-y-z coord each */	
	for (i = 0; i < 8; v ++, i ++)
	{
		if (3 > scanf("%f %f %f", &v[0][0], &v[0][1], &v[0][2]))
			break;
		v[0][3] = 0.f;
	}
			
	scanf("\n");

	return i;
}

int binnerin_phcf(int nfacets, int * nverts, float * v)
{
	int i, total;

	total = 0;
	for (i = 0; i < nfacets; v += nverts[i]*3, i ++) {
		nverts[i] = get_polygonf(v);
		total += nverts[i];
	}

	return total;
}


int get_polygond(double * v)
/* 
 * return value: num_vertices actually read in successfully
 */
{
	int i, n;
	
	scanf("%d", &n);

	/* assume a quad in 3d space. 4 vertices, x-y-z coord each */	
	for (i = 0; i < n; v += 3, i ++)
		if (3 > scanf("%lf %lf %lf", v, v+1, v+2))
			break;
			
	scanf("\n");

	return i;
}

int binnerin_phcd(int nfacets, int * nverts, double * v)
{
	int i, total;

	total = 0;
	for (i = 0; i < nfacets; v += nverts[i]*3, i ++) {
		nverts[i] = get_polygond(v);
		total += nverts[i];
	}

	return total;
}

void binnerout_phcf(int nfacets, int * nverts, float *v)
{
	int i, j;

	for (i = 0; i < nfacets; i ++) {

		printf("%d ", nverts[i]);

		for (j = 0; j < nverts[i]; j ++)
		{
			printvertexf(v, 3); 
			v += 3;
		}
		printf("\n");
	}
}

void binnerout_phcd(int nfacets, int * nverts, double *v)
{
	int i, j;

	for (i = 0; i < nfacets; i ++) {

		printf("%d ", nverts[i]);

		for (j = 0; j < nverts[i]; j ++)
		{
			printvertexd(v, 3); 
			v += 3;
		}
		printf("\n");
	}
}

void output_with_compression(char * fname,
							int * sz,
							double * vol)
{
	int i, j, k, nvox;
	double         * dvol;
	unsigned int   * hash;
	unsigned short * xyz;
	int    ori[3], size[3];
	char fullname[256];

	nvox = 0;
	for (i = 0; i < sz[0]; i ++)
		for (j = 0; j <sz[1]; j ++)
			for (k = 0; k <sz[2]; k ++) {

				if (vol[(i * sz[1] + j)*sz[2] + k] > 1e-16)
				    nvox ++;
			}
	
	dvol = malloc(nvox * sizeof(double));
	hash = malloc(sz[0]*sz[1]*sizeof(int));
	xyz  = malloc(nvox * 3 * sizeof(unsigned short));

	nvox = 0;
	for (i = 0; i < sz[0]; i ++)
		for (j = 0; j <sz[1]; j ++)
		{
			hash[i * sz[1] + j] = nvox;
			for (k = 0; k <sz[2]; k ++) {

				if (vol[(i * sz[1] + j)*sz[2] + k] > 1e-16)
				{
					dvol[nvox] = vol[(i * sz[1] + j)*sz[2] + k];
					xyz[nvox*3+0] = (unsigned short)(i);
					xyz[nvox*3+1] = (unsigned short)(j);
					xyz[nvox*3+2] = (unsigned short)(k);
				    nvox ++;
				}
			}
		}

	ori[0] = ori[1] = ori[2] = 0;
	size[0] = nvox;
	sprintf(fullname, "%s.bin", fname);
	vcbGenBinm(fullname, VCB_DOUBLE, 1, ori, size, 1, dvol);
	/*
	sprintf(fullname, "%s.bin", fname);
	vcbGenBinm(fullname, VCB_UNSIGNEDINT, 2, ori, sz, 1, hash);
	*/
	sprintf(fullname, "%s.bin", fname);
	vcbGenBinm(fullname, VCB_UNSIGNEDSHORT, 1, ori, size, 3, xyz);

	free(dvol);
	free(hash);
	free(xyz);
}

void export_VTKvol(char * fname, double * orig, int * sz, double * cellsize, double * vol)
{
	FILE * fp;
	char fullname[256];
	int i, j, k;
	double val;
	float * cvol;
	
	sprintf(fullname, "%s.vtk", fname);
	fp = fopen(fullname, "w");
	
	fprintf(fp, "# vtk DataFile Version 1.0\n");
	fprintf(fp, "rebinned Qxyz histogram %s\n", fname);
	fprintf(fp, "BINARY\n");
	fprintf(fp, "DATASET STRUCTURED_POINTS\n");
	fprintf(fp, "DIMENSIONS %d %d %d\n", sz[0], sz[1], sz[2]);
	fprintf(fp, "ASPECT_RATIO 1.0 1.0 1.0\n");
	fprintf(fp, "ORIGIN %e %e %e\n",orig[0]*cellsize[0], 
									orig[1]*cellsize[1], 
									orig[2]*cellsize[2]);
	fprintf(fp, "SPACING %e %e %e\n", cellsize[0], cellsize[1], cellsize[2]);	
	fprintf(fp, "POINT_DATA %d\n", sz[0]*sz[1]*sz[2]);
	fprintf(fp, "SCALARS scalars float\n");
	fprintf(fp, "LOOKUP_TABLE default\n");
	
	cvol = malloc(sizeof(float)*sz[0]*sz[1]*sz[2]);
	for (i = 0; i < sz[0]; i ++)
		for (j = 0; j <sz[1]; j ++)
			for (k = 0; k <sz[2]; k ++) {
/*
				val = log10(vol[(i * sz[1] + j) * sz[2] +k]);
				val += 16;
				val *= 16;
				if (val < 0) val = 0;
*/
				val = vol[(i * sz[1] + j) * sz[2] +k];
				cvol[(k * sz[1] + j) * sz[0] +i] = (float)val;
			}

	for (i = 0; i < sz[0]*sz[1]*sz[2]; i ++)
		vcbToggleEndian(cvol+i, sizeof(float));

	fwrite(cvol, sizeof(float), sz[0]*sz[1]*sz[2], fp);
	free(cvol); 
	fclose(fp);
}

void export_VTKhdr(char * fname, int * orig, int * sz, double * cellsize)
{
	FILE * fp;
	char fullname[256];

	sprintf(fullname, "%s/run.hdr", fname);
	fp = fopen(fullname, "w");
	
	fprintf(fp, "REBINNED Qxyz HISTOGRAM\n%s\n", fname);
	fprintf(fp, "DIMENSIONS %d %d %d\n", sz[0], sz[1], sz[2]);
	fprintf(fp, "ORIGIN %e %e %e\n",orig[0]*cellsize[0], 
									orig[1]*cellsize[1], 
									orig[2]*cellsize[2]);
	fprintf(fp, "SPACING %e %e %e\n", cellsize[0], cellsize[1], cellsize[2]);	
	fprintf(fp, "Total NUMBER OF BINS %d\n", sz[0]*sz[1]*sz[2]);
	fprintf(fp, "BINARY FLOAT VOLUME REQUIRES %d byes\n", sz[0]*sz[1]*sz[2]*sizeof(float));
	
	fclose(fp);
}

