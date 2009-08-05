#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vcbtrimesh.h"
#include "exhash.h"
#include "vcbmath.h"
#include "vcbutils.h"

static int read_len(const char *filename)
/* 
 * internal function that discovers how many triangles there are in an STL file.
 * return -1 for failure, and >0 for actual number of facets found
 */
{
  
	char  header[10], objectname[80], tmpstr[120], * expstr;
	int   count=0;

	FILE  *fp;
	
	if ((fp=fopen(filename,"r"))==NULL) {
		fprintf(stderr,"read_len: cannot open file\n");
		return -1;
	}

	fscanf(fp,"%s",header);
	if(strcmp(header,"solid")!=0) {
		fprintf(stderr,"read_len: bad file format ...\n");
		return -1;
	}

	fscanf(fp,"%s",objectname);
	
	expstr="facet";
	while (1) {
    
		fscanf(fp,"%s",tmpstr);
		if (strcmp(tmpstr,"endsolid")==0)
			return count;
		if (strcmp(tmpstr,expstr)==0)
			count=count+1;
	}
 
	fclose(fp);

	return -1;
}

static int read_in_bstl(const char *filename, float ** normals, float ** facets) 
{
	FILE    *fd;
	char    partnameb[80], partname[80], bla[2];
    int     i, j, nfacets, inv_border;
	float   * vptr, tbuf[3];

    if ((fd = fopen (filename, "rb")) == NULL) {        
		fprintf(stderr,"%s can't be opened ... \n", filename);       
		return -1;
	}
     
	if (fread (partnameb,1,80,fd) != 80) {
		fprintf(stderr,"read_b_len: problems reading file %s\n",filename);
        return -1;    
	}

	sscanf(partnameb,"%s",partname);
	fprintf(stderr,"read_in_stl: reading solid %s\n",partname);

	inv_border = vcbBigEndian();

	fread(&nfacets, sizeof(int), 1, fd);
	if (inv_border) vcbToggleEndian(&nfacets, sizeof(int));

	(*normals) = (float *) malloc (nfacets * sizeof(float) * 3);
	(*facets)  = (float *) malloc (nfacets * sizeof(float) * 9);
	if (((*normals) == NULL) || ((*facets) == NULL)) {
		fprintf(stderr,"read_in_stl: cannot allocate memory for %s\n",filename);
		return -1;
	}

	vptr = (*facets);
	for (i = 0; i < nfacets; i++) {
		fread(&tbuf[0],sizeof(float),1,fd);
		fread(&tbuf[1],sizeof(float),1,fd);
		fread(&tbuf[2],sizeof(float),1,fd);
		if (inv_border) {
			vcbToggleEndian(&tbuf[0],sizeof(float));
			vcbToggleEndian(&tbuf[0],sizeof(float));
			vcbToggleEndian(&tbuf[0],sizeof(float));
		}

		vcbNormalize3(tbuf);
		(*normals)[i*3 + 0] = tbuf[0];
		(*normals)[i*3 + 1] = tbuf[1];
		(*normals)[i*3 + 2] = tbuf[2];

		for (j = 0; j < 9; j++) {
			fread(vptr,sizeof(float),1,fd); 
			if (inv_border) vcbToggleEndian(vptr, sizeof(float)); 
			vptr++;
		}

        if (fread (bla, 1,2,fd)!=2) {
			fprintf(stderr,"read_b_stl: get bla failed\n");
			free(*normals);
			free(*facets);
			return -1;
        }
	}

	fclose(fd);
	return nfacets;
}

static int read_in_stl(const char *filename, float ** normals, float ** facets) 
/* internal function for vr_tmread_stl */
/* return -1 for failure */
{
	char  header[10], objectname[80], tmpstr[120];
	char  *expstr, *innerstr;
	int   count=0;
	int   polyindx=0, nfacets;
	FILE  * fp;
	float tbuf[3];

	nfacets = read_len(filename);
	if (nfacets <= 0) {
		fprintf(stderr,"read_in_stl: found 0 facets in %s\n", filename);
		return -1;
	}

	(*normals) = (float *) malloc (nfacets * sizeof(float) * 3);
	(*facets)  = (float *) malloc (nfacets * sizeof(float) * 9);
	if (((*normals) == NULL) || ((*facets) == NULL)) {
		fprintf(stderr,"read_in_stl: cannot allocate memory for %s\n",filename);
		return -1;
	}

	if ((fp=fopen(filename,"r"))==NULL){
		fprintf(stderr,"read_in_stl: file reading error...\n");
		return -1;
	}
  
	fscanf(fp,"%s",header);
	if(strcmp(header,"solid")!=0) {
		fprintf(stderr,"read_in_stl: bad file format ...\n");
		fclose(fp);
		return -1;
	}
	
	innerstr="vertex";
	fscanf(fp,"%s",objectname);
	
	expstr="facet";
	while (1) {
    
		fscanf(fp,"%s",tmpstr);
		if (strcmp(tmpstr,"endsolid")==0) {
			/* proper point of exit */
			fclose(fp);
			return polyindx; /* number of facets (polygons) */
		}

		if ((strcmp(tmpstr,expstr)==0)||(strcmp(tmpstr,innerstr)==0)) {

			if (strcmp(tmpstr,"facet")==0)
              expstr="normal";
			else if (strcmp(tmpstr,"normal")==0)
				expstr="outer";
            else if (strcmp(tmpstr,"outer")==0)
				expstr="loop";
			else if (strcmp(tmpstr,"vertex")==0)
				expstr="endloop";
            else if (strcmp(tmpstr,"endloop")==0)
				expstr="endfacet";

			if (strcmp(tmpstr,"normal")==0) {
				fscanf(fp,"%f %f %f",&tbuf[0],&tbuf[1],&tbuf[2]);
				vcbNormalize3(tbuf);
				(*normals)[polyindx * 3 + 0] = tbuf[0];
				(*normals)[polyindx * 3 + 1] = tbuf[1];
				(*normals)[polyindx * 3 + 2] = tbuf[2];
			}
			else if (strcmp(tmpstr,"vertex")==0) {
				if (count<3) {
					fscanf(fp,"%f %f %f",&tbuf[0],&tbuf[1],&tbuf[2]);
				}
				else {
					fprintf(stderr,"read_in_stl: got more than 3 vertices for polygon #%d\n",polyindx);
					fclose(fp);
					return -1;
				}
				(*facets)[polyindx * 9 + count*3 + 0] = tbuf[0];
				(*facets)[polyindx * 9 + count*3 + 1] = tbuf[1];
				(*facets)[polyindx * 9 + count*3 + 2] = tbuf[2];
				count=count+1;
			}
			else if (strcmp(tmpstr,"loop")==0)
                count=0;
			else if (strcmp(tmpstr,"endloop")==0)
                ;  /*fprintf(stderr,"endloop detected  ");*/
            else if (strcmp(tmpstr,"endfacet")==0) {
                /* fprintf(stderr,"endfacet detected\n");*/
                 polyindx=polyindx+1;
                /* fprintf(stderr," %d",polyindx);*/
                 expstr="facet";
			}
		}
		else {
			fprintf(stderr,"read_in_stl: bad file format\n");
			fclose(fp);
			return -1;
		}
	}
  
	return polyindx;
}

int read_in_raw(const char *filename, float ** normals, float ** facets) 
{
	FILE    *fp;
    int     i, nfacets, type;
	float  * vptr;

    if ((fp = fopen (filename, "rb")) == NULL) {        
		fprintf(stderr,"%s can't be opened ... \n", filename);       
		return -1;
	}

	fscanf(fp, "%d %d", &nfacets, &type); 
	fprintf(stderr,"reading %d facets in %d type\n",nfacets, type);

	(*normals) = NULL;
	(*facets)  = (float *) malloc (nfacets * sizeof(float) * 9);
	if ((*facets) == NULL) {
		fprintf(stderr,"read_in_raw: cannot allocate memory for %s\n",filename);
		return -1;
	}

	vptr = (*facets);
	for (i=0; i<3*nfacets; i++) {
		fscanf(fp, "%f %f %f", &vptr[0], &vptr[1], &vptr[2]);
		vptr += 3;
	}

	fclose(fp);
	return nfacets;
}

int tmstl_hashfunc(void * data, void * param)
/* default hash function used by vr_trimesh* functions for extensible hashing */
/* this one has been modified to be more numerically stable */
{
	int id = 0;
	float * vert, * coeff;

        vert = (float *) data;
        coeff = (float *) param;

        id += (int)(((int)(vert[0] - coeff[0])*coeff[3]+0.5f) * coeff[6] * coeff[6]); 
        id += (int)(((int)(vert[1] - coeff[1])*coeff[4]+0.5f) * coeff[6]);
        id += (int)(((int)(vert[2] - coeff[2])*coeff[5]+0.5f));

	return id;
}


int tmstl_cmp (void * src, void * dst)
/* default compare function used by vr_trimesh* functions for extensible hashing */
{
	int i;
	float diff, tdiff, * vert1, * vert2;
	vert1 = (float *) src;
	vert2 = (float *) dst;

	diff = 0.f;
	for (i = 0; i < 3; i ++) {
		tdiff = vert1[i] - vert2[i];
		diff += ( tdiff >= 0) ? tdiff : (-1.f*tdiff);
	}

	return (diff < 0.00001) ? 1 : -1;
}

void vcbTrimeshStlb(char * filename, int * nverts, int * nfacets, float ** vdata, int ** fdata, float ** fnormals)
/*
 * vcbTrimeshStl: generates a triangle mesh from a binary STL file, where all triangles are stored as independent
 *               facets. It calls two internal functions read_len and read_in_stl to parse STL format,
 *               and calls vcbTrimeshRaw to construct vertex arrays to represent the triangle mesh.
 * Input:
 *   filename:  name of the stl file to read
 * Output:
 * (*nverts):   # of vertices
 * (*nfacets):  # of triangles
 * (*vdata):    pointer to the vertex array
 * (*fdata):    pointer to the indices array
 * (*fnormals): pointer to an array of normals (note, stl has per triangle normal only)
 * Return value: None
 */
{
	float * facets;
	int numfacets;

	if((numfacets = read_in_bstl(filename, fnormals, &facets)) < 0) {
		fprintf(stderr,"vr_tmread_stl: failed to read %s\n",filename);
		return;
	}

	vcbTrimeshRaw(facets, 3, numfacets, nverts, vdata, fdata);
	(*nfacets) = numfacets;

	free(facets);
}

void vcbTrimeshStl(char * filename, int * nverts, int * nfacets, float ** vdata, int ** fdata, float ** fnormals)
/*
 * vcbTrimeshStl: generates a triangle mesh from a ASCII STL file, where all triangles are stored as independent
 *               facets. It calls two internal functions read_len and read_in_stl to parse STL format,
 *               and calls vcbTrimeshRaw to construct vertex arrays to represent the triangle mesh.
 * Input:
 *   filename:  name of the stl file to read
 * Output:
 * (*nverts):   # of vertices
 * (*nfacets):  # of triangles
 * (*vdata):    pointer to the vertex array
 * (*fdata):    pointer to the indices array
 * (*fnormals): pointer to an array of normals (note, stl has per triangle normal only)
 * Return value: None
 */
{
	float * facets;
	int numfacets;

	if((numfacets = read_in_stl(filename, fnormals, &facets)) < 0) {
		fprintf(stderr,"vr_tmread_stl: failed to read %s\n",filename);
		return;
	}

	vcbTrimeshRaw(facets, 3, numfacets, nverts, vdata, fdata);
	(*nfacets) = numfacets;

	free(facets);
}

void vcbTrimeshRawEx(float * facets, int nval, int nfacets, int * nverts, float ** vdata, int ** fdata)
/*
 * vcbTrimeshRaw: is the core function within the vr_trimesh library. It takes a linear list of
 *               floating point attributes organized in facets. For example:
 * v0_0, v0_1, v0_2, v0_3, ... v0_k, v1_0, ... v1_k, v2_0, ..., v2_k, v3_0, ... v3_k, v4_0 ...
 *               Here there a k attributes on each vertex, and v0, v1, v2 form the 1st triangle,
 *               v3, v4, and v5 form the 2nd triangle.
 *               We require that the first 3 attributes must be the coordinate of the vertex
 *
 *               vcbTrimeshRaw gives back a vertex array in the same format as the facets array, 
 *               but only containing distinctive vertices in (*vdata), and indices on each
 *               triangle into (*vdata) in (*fdata).
 * Inputs:
 *   facets - array of raw facet data, organized triangle by triangle
 *   nval   - the number of attributes per vertex, k
 *   nfaces - number of triangles
 *
 * Outputs:
 *   (*nverts) - number of distinctive vertices identified
 *   (*vdata)  - vertex array
 *   (*fdata)  - indices array
 *
 * Usage:
 *   The ability to handle more than 3 attributes per vertex is for situations where one needs
 *   to have per vertex normal as well. In that case, nval == 6. In the (*vdata) that is returned,
 *   we then have coordinates0, normal0, coordinates1, normal1, coordinates2, normal2, ...
 *   To use (*vdata) in glVertexArray and glNormalArray, just be sure to set
 *                stride = 6*sizeof(float)
 *   The case where only coordinates are handled, just do nval = 3. Obviously, (*vdata) can be
 *   used in glVertexArray with stride = 3 * sizeof(float).
 */
{
#ifndef vcb_parti
#define vcb_parti 1024
#endif

#ifndef VCB_RANGETOL
#define VCB_RANGETOL 0.02f
#endif
#ifndef VCB_RANGEEXT
#define VCB_RANGEEXT 1.f + VCB_RANGETOL*2
#endif

	float minx, miny, minz, maxx, maxy, maxz, coeff[7];
	float rangex, rangey, rangez;

	int i, j, numfacets, numverts;
	exhTable exht;
	exhStat * stat = (exhStat *) malloc (sizeof(exhStat));

	numfacets = nfacets;

	minx = miny = minz = 1e8;
	maxx = maxy = maxz = -1e8;
	for (i = 0; i < numfacets*3; i ++) {
		minx = VCB_MINVAL(facets[i*nval + 0], minx);
		miny = VCB_MINVAL(facets[i*nval + 1], miny);
		minz = VCB_MINVAL(facets[i*nval + 2], minz);
		maxx = VCB_MAXVAL(facets[i*nval + 0], maxx);
		maxy = VCB_MAXVAL(facets[i*nval + 1], maxy);
		maxz = VCB_MAXVAL(facets[i*nval + 2], maxz);
	}

	printf("(%f, %f, %f) - (%f, %f, %f)\n",minx,miny, minz, maxx, maxy, maxz);
	rangex = maxx - minx;
	rangey = maxy - miny;
	rangez = maxz - minz;

        coeff[0] = minx - rangex * VCB_RANGETOL;
        coeff[1] = miny - rangey * VCB_RANGETOL;
        coeff[2] = minz - rangez * VCB_RANGETOL;
        coeff[3] = vcb_parti/(rangex * VCB_RANGEEXT);
        coeff[4] = vcb_parti/(rangey * VCB_RANGEEXT);
        coeff[5] = vcb_parti/(rangez * VCB_RANGEEXT);
	coeff[6] = vcb_parti;

	exht = exhNew(20, 6, sizeof(float)*nval, tmstl_hashfunc, coeff, sizeof(float)*7, tmstl_cmp);

	for (i = 0; i < numfacets; i ++)
		for (j = 0; j < 3; j ++)
			if (exhQueryItem(exht, &facets[(i*3+j)*nval])<0) {
				if (exhAddItem(exht, &facets[(i*3+j)*nval]) < 0) {
					fprintf(stderr,"after %d facet, %d vertex, exhAddItem failed\n",i,j);
					exhFree(exht);
					exit(-1);
				}
			}
	
	numverts = exhFinalize(exht,stat);
	(*vdata) = (float *) malloc (sizeof(float) * nval * numverts);
	exhSerialize(exht, (*vdata));

	(*fdata) = (int *) malloc (sizeof(int) * 3 * numfacets);
	for (i = 0; i < numfacets; i ++)
	  for (j = 0; j < 3; j ++)
	    (*fdata)[i * 3 + j] = exhItemID(exht,&facets[(i*3+j)*nval]);

	(*nverts) = numverts;

	printf("obtained %d triangles and %d vertices\n",numfacets, numverts);
	/* uncomment this line if want to see statistics of extendible hash*/
	/*exhPrintStat(stdout,stat);*/ 

	exhFree(exht);

#undef VCB_RANGETOL
#undef VCB_RANGEEXT
#undef vcb_parti
}

#include "jrb.h"

int rbtm_cmp (Jval first, Jval second)
/* default compare function used by vr_trimesh* functions for rbtree */
{
	int i;
	float diff, tdiff, * vert1, * vert2;
	vert1 = (float *) first.v;
	vert2 = (float *) second.v;

	for (i = 0; i < 3; i ++) {
		if (vert1[i] < vert2[i]) 
		    return -1;
		else if (vert1[i] > vert2[i])
		    return 1;
	}

	return 0;
}

void vcbTrimeshRaw(float * facets, int nval, int nfacets, int * nverts, float ** vdata, int ** fdata)
/*
 * vcbTrimeshRaw: is the core function within the vr_trimesh library. It takes a linear list of
 *               floating point attributes organized in facets. For example:
 * v0_0, v0_1, v0_2, v0_3, ... v0_k, v1_0, ... v1_k, v2_0, ..., v2_k, v3_0, ... v3_k, v4_0 ...
 *               Here there a k attributes on each vertex, and v0, v1, v2 form the 1st triangle,
 *               v3, v4, and v5 form the 2nd triangle.
 *               We require that the first 3 attributes must be the coordinate of the vertex
 *
 *               vcbTrimeshRaw gives back a vertex array in the same format as the facets array, 
 *               but only containing distinctive vertices in (*vdata), and indices on each
 *               triangle into (*vdata) in (*fdata).
 * Inputs:
 *   facets - array of raw facet data, organized triangle by triangle
 *   nval   - the number of attributes per vertex, k
 *   nfaces - number of triangles
 *
 * Outputs:
 *   (*nverts) - number of distinctive vertices identified
 *   (*vdata)  - vertex array
 *   (*fdata)  - indices array
 *
 * Usage:
 *   The ability to handle more than 3 attributes per vertex is for situations where one needs
 *   to have per vertex normal as well. In that case, nval == 6. In the (*vdata) that is returned,
 *   we then have coordinates0, normal0, coordinates1, normal1, coordinates2, normal2, ...
 *   To use (*vdata) in glVertexArray and glNormalArray, just be sure to set
 *                stride = 6*sizeof(float)
 *   The case where only coordinates are handled, just do nval = 3. Obviously, (*vdata) can be
 *   used in glVertexArray with stride = 3 * sizeof(float).
 */
{

	int i, j, found, d, numfacets, numverts;
	JRB b, bn;
	Jval key;

	numfacets = nfacets;
	b = make_jrb();

	numverts = 0;
	for (i = 0; i < numfacets; i ++)
		for (j = 0; j < 3; j ++) {
		    key = new_jval_v(&facets[(i*3+j)*nval]);
			if (jrb_find_gen(b, key, rbtm_cmp) == NULL) {
			    (void) jrb_insert_gen(b, key, new_jval_i(-1), rbtm_cmp);
				numverts ++;
			}
		}

	if (NULL == ((*vdata) = (float *) malloc (sizeof(float) * nval * numverts))) {
	    perror("vcbtrimeshraw allocating vdata");
	    exit(1);
	}

    i = 0;
	jrb_traverse(bn, b){
	  bn->val.i = i;
	  memcpy(&((*vdata)[i*nval]), bn->key.v, nval*sizeof(float));
	  i ++;
	}

	if (NULL == ((*fdata) = (int *) malloc (sizeof(int) * 3 * numfacets))) {
	    perror("vcbtrimeshraw allocating fdata");
	    exit(1);
	}

	d = 0;
	for (i = 0; i < numfacets; i ++)
	  for (j = 0; j < 3; j ++) {
	     key = new_jval_v(&facets[(i*3+j)*nval]);
	     bn = jrb_find_gte_gen(b, key, rbtm_cmp, &found);
		 d += (1 - found);
	     (*fdata)[i * 3 + j] = bn->val.i; 
	  }

	(*nverts) = numverts;

	printf("vcbtrimesh obtained %d triangles and %d vertices, with %d unmatch\n",numfacets, numverts, d);

	jrb_free_tree(b); /* frees up teh rbtree */

}
