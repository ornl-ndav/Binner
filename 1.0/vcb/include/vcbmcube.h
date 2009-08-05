/****************************************************************************
 *   Copyright (C) 2004  by Jian Huang										*
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

#ifndef _VCB_MCUBE_H 
#define _VCB_MCUBE_H

/** Struct
 * Name: vcbMCT
 * Version: 0.92
 * Description:
 *  typedefined&#58; struct MCubeTable * vcbMCT <BR>
 *  MCubeTable is the structure that contains the information for the Marching 
 *  Cubes triangulator. The triangulator data is stored in a statically declared variable&#58; <br>
 *	static struct MCubeTable mcube_table [256]{...};
 * Elements:
 *  int which_case: The value (0-14) of the cases of the Marching Cube.
 *  int permute[8]:	A permutation of 8 numbers 1-8
 *  int nbrs:		(0-63)
 **/
typedef struct MCubeTable {
	/* "which_case" is the value (0-14) of the cases of the Marching Cube */
	int which_case; 
	int permute[8];
	int nbrs; 
} * vcbMCT;

#ifndef VCB_WITHNORMAL
#define VCB_WITHNORMAL 1
#endif
#ifndef VCB_ONLYVERTEX
#define VCB_ONLYVERTEX 0
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Directions for vcbmcube
 * <h2>Description:</h2>
 * There are two basic methods by which one can use VCB's Marching Cube Algorthim.
 * <p>
 * One method proceeds iterativly first calling vcbMCVertexBegin and then invoking
 * either vcbMCVNoNormals or vcbMCVWithNormals for each vertex in the volume for which
 * the triangles are to be extracted. <br>
 * The process is completed by calling vcbMCVertexEnd. This method is similar
 * to the OpenGL method of drawing triangles with glBegin, glVertex and glEnd.<br>
 *  <h3>For example:</h3><pre>
 *	int ntri = 0;
 *	vcbMCVertexBegin() &#47;*set up all the constants and basic constructs for each voxel*\/
 *		ntri += vcbMCVWithNormals(i,j,k,f); &#47;*extract triangles in vertex i,j,k*\/
 *	vcbMCVertexEnd()   &#47;*clean up all constructs*\/
 *	printf("got %d triangles\\n",ntri);  </pre>
 * <p>
 * The other method consists of one call to either vcbMarchingCubeBlk or vcbMarchingCubeList
 *	by the user, which returns the extraced isosurface.
 * <p>
 * <h3>See the following test programs:</h3>
 *	t_amcube_main for an example using vcbMarchingCubeList or <br>
 *	t_mcube_main for an example using vcbMarchingCubeBlk.
 **/

/** Function
 * Name: vcbMCVertexBegin
 * Version: 0.92
 * Description:
 *  This function is used for the iterative method of using VCB's Marching Cube. <BR>
 *	See the Directions for vcbmcube.
 *	Also see vcbMarchingCubeBlk for an alternate method.
 * Arguments: 
 *   vcbdatatype datatype: This is the basic format of the volume, which can be either
 *               VCB_UNSIGNEDBYTE, VCB_SHORT, VCB_INT or VCB_FLOAT.
 *   int wn:     This is a flag to signal VCB's Marching Cube Algorthim to extract the isosurface
 *               with or without normals. To extract the isosurface without normals is about
 *               30% ~ 40% faster. Correspondingly, the choices are VCB_WITHNORMAL or VCB_ONLYVERTEX,
 *               which are both macros defined in vcbmcube.h.
 *   int* dsizes:   dsizes[0] - the size of the x dimension <BR>
 *                  dsizes[1] - the size of the y dimension <BR>
 *                  dsizes[2] - the size of the z dimension
 *   void* data:     The pointer to the volume to be extracted. If wn == VCB_ONLYVERTEX, the volume
 *                   is expected to be compacted in the space pointed to by data in this
 *                   sequence&#58; <pre>
 *	voxel(i,j,k) is the ((i*dsizes[1]+j)*diszes[2]+k)th value. </pre>
 *	<p>
 *             However, if wn == VCB_WITHNORMAL, then user must provide per voxel normal,
 *             and the voxel value on (i,j,k) should be at&#58; 
 *                 ((i*dsizes[1]+j)*diszes[2]+k)*4
 *	<p>
 *             The x, y, and z component of the per vertex normal are at&#58; <br>
 *                 ((i*dsizes[1]+j)*diszes[2]+k)*4 + 1 <BR>
 *                 ((i*dsizes[1]+j)*diszes[2]+k)*4 + 2 <BR>
 *                 ((i*dsizes[1]+j)*diszes[2]+k)*4 + 3, respectively. </p>
 *  float isoval:    A floating point describing the isovalue for which an isosurface
 *               should be extracted
 * Return: None
 * Known Issues: None
 **/
void vcbMCVertexBegin(vcbdatatype datatype, void * data, int *dsizes, float isoval, int wn);

/** Function
 * Name: vcbMCVNoNormals
 * Version: 0.92
 * Description:
 *  This function is for use with VCB_ONLYVERTEX. After the call returns, the user program can
 *  copy the memory pointed to by f to other memory allocations. <BR>
 *             The results in f should be interpreted as&#58;
 *				 <table width=400>
 *               <tr><td>  floats 0-2   <td>  coordinate of v1
 *               <tr><td>  floats 3-5   <td>  normal of v1 
 *               <tr><td>  floats 6-8   <td>  coordinate of v2
 *               <tr><td>  floats 9-11  <td>  normal of v2
 *               <tr><td>  floats 12-14 <td>  coordinate of v3
 *               <tr><td>  floats 15-17 <td>  normal of v3 
 *               <tr><td>  floats 18-20 <td>  coordinate of v4
 *               <tr><td>  floats 21-23 <td>  normal of v4
 *               <tr><td>  ......		<td>  </table>
 *             v1, v2, v3 forms the first triangle. <BR>
 *             If exist, v4, v5, v6 forms the second triangle, ...
 * Arguments:
 *  int x: x coordinate input
 *  int y: y coordinate input
 *  int z: z coordinate input
 *  float* f:  Output. f is a pointer to a memory allocation that is assumed to be
 *             valid. When wn == VCB_ONLYVERTEX, only 36 floats are needed.
 * Return: int; The number of triangles generated within the given voxel. The value
 *      is always in the range [0,4].
 * Known Issues: None             
 **/
int  vcbMCVNoNormals(int x, int y, int z, float * f);

/** Function
 * Name: vcbMCVWithNormals
 * Version: 0.92
 * Description:
 *  This function is for use with VCB_WITHNORMAL. After the call returns, the user program can
 *  copy the memory pointed to by f to other memory allocations. <BR>
 *             The results in f should be interpreted as&#58;
 *				 <table width=400>
 *               <tr><td>  floats 0-2   <td>  coordinate of v1
 *               <tr><td>  floats 3-5   <td>  normal of v1 
 *               <tr><td>  floats 6-8   <td>  coordinate of v2
 *               <tr><td>  floats 9-11  <td>  normal of v2
 *               <tr><td>  floats 12-14 <td>  coordinate of v3
 *               <tr><td>  floats 15-17 <td>  normal of v3 
 *               <tr><td>  floats 18-20 <td>  coordinate of v4
 *               <tr><td>  floats 21-23 <td>  normal of v4
 *               <tr><td>  ......		<td>  </table>
 *             v1, v2, v3 forms the first triangle. <BR>
 *             If exist, v4, v5, v6 forms the second triangle, ...
 * Arguments:
 *  int x: x coordinate input
 *  int y: y coordinate input
 *  int z: z coordinate input
 *  float* f:  Output. f is a pointer to a memory allocation that is assumed to be
 *             valid. when wn == VCB_WITHNORMAL, it must have space for at least 72
 *             floating values, i.e. 288 bytes. This is because marching cube
 *             generates at most 4 triangles per voxel, thus&#58; <pre>
 *3 coordinates + 3 normal components = 6 floats/vertex
 *                                      3 vertices per triangle
 *                                      4 triangles/voxel
 * -------------------------------------------------------
 *                                      72 floats/voxel </pre>
 * Return: int; The number of triangles generated within the given voxel. The value
 *      is always in the range [0,4].
 * Known Issues: None             
 **/
int  vcbMCVWithNormals(int x, int y, int z, float * f);

/** Function
 * Name: vcbMCVertexEnd
 * Version: 0.92
 * Description:
 *  This function is used as an ending call to vcbMCVertexBegin calls. 
 * Arguments: None
 * Return: None
 * Known Issues: None
 **/
void vcbMCVertexEnd();

/** Function
 * Name: vcbMarchingCubeBlk
 * Version: 0.94
 * Description:
 *  The all-in-one method of using VCB's Marching Cube with a block of data is described below.<br>
 *	See vcbMarchingCubeList for a similar method using a list instead of a block. <br>
 *  See vcbMCVertexBegin for an iterative method.
 *  <p>
 *  vcbMarchingCubeBlk is designed to be called as a single function from which a user
 *   can call and obtain an extracted isosurface stored in a vertex array as defined
 *   by OpenGL standards.
 *   <p>
 *   The vertex array can be rendered using&#58; <pre>
 *    glEnableClientState(GL_NORMAL_ARRAY);
 *    glEnableClientState(GL_VERTEX_ARRAY);
 *    glNormalPointer(GL_FLOAT, 6*sizeof(float), vdata+3);
 *    glVertexPointer(3, GL_FLOAT, 6*sizeof(float), vdata);
 *    glDrawElements(GL_TRIANGLES, nfacets*3, GL_UNSIGNED_INT, fdata); </pre>
 * Arguments:
 *   vcbdatatype datatype: The basic format of the volume, which can be either
 *               VCB_UNSIGNEDBYTE, VCB_SHORT, VCB_INT or VCB_FLOAT.
 *   void* block: pointer to the volume to be extracted. If wn == VCB_ONLYVERTEX, the volume
 *               is expected to be compacted in the space pointed to by block in this
 *               sequence&#58; <br>
 *					voxel(i,j,k) is the ((i*dsizes[1]+j)*diszes[2]+k)th value
 *	 <p>
 *             However if wn == VCB_WITHNORMAL, then user must provide per voxel normal,
 *             and the voxel value on (i,j,k) should be at&#58; <br>
 *				((i*dsizes[1]+j)*diszes[2]+k)*4
 *	 <p>
 *             The x, y, and z component of the per vertex normal are at&#58; <br>
 *		((i*dsizes[1]+j)*diszes[2]+k)*4 + 1 <BR>
 *		((i*dsizes[1]+j)*diszes[2]+k)*4 + 2 <BR>
 *		((i*dsizes[1]+j)*diszes[2]+k)*4 + 3, respectively.
 *   float isoval: a floating point describing the isovalue for which an isosurface
 *               should be extracted
 *   int* dsizes: dsizes[0] - the size of the x dimension <BR>
 *             dsizes[1] - the size of the y dimension <BR>
 *             dsizes[2] - the size of the z dimension
 *   int wn: A flag to signal VCB's Marching Cube Algorthim to extract the isosurface
 *           with or without normals. To extract the isosurface without normals is about
 *           30% ~ 40% faster. Correspondingly, the choices are VCB_WITHNORMAL or VCB_ONLYVERTEX,
 *           which are both macros defined in vcbmcube.h.
 *  int* nverts: Number of vertices.
 *  float** varray_v: Output. An interleaved floating point vertex array in 
 *             the following order&#58;
 *				 <table width=400>
 *               <tr><td>  floats 0-2   <td>  coordinate of v1
 *               <tr><td>  floats 3-5   <td>  normal of v1 
 *               <tr><td>  floats 6-8   <td>  coordinate of v2
 *               <tr><td>  floats 9-11  <td>  normal of v2
 *               <tr><td>  floats 12-14 <td>  coordinate of v3
 *               <tr><td>  floats 15-17 <td>  normal of v3 
 *               <tr><td>  floats 18-20 <td>  coordinate of v4
 *               <tr><td>  floats 21-23 <td>  normal of v4
 *               <tr><td>  ......		<td>  </table>
 *  int** varray_i: Output. An integer index array which implicity references 
 *             vdata to describe each triangle. <BR>
 *             For instance, if fdata contains the following data&#58; <BR>
 *                0 1 2 3 4 5 0 6 4 2 5 3 <BR>
 *             then we have trianges formed by vertices&#58; <BR>
 *                0, 1, 2; 3, 4, 5; 0, 6, 4; 2, 5, 3; respectively.
 *
 * Return: int; Total number of triangles (nfacets) in the triangle mesh
 * Known Issues: None
 **/
int  vcbMarchingCubeBlk(vcbdatatype datatype, void * block, float isoval, int *dsizes, int wn, int * nverts, float ** varray_v, int ** varray_i);

/** Function
 * Name: vcbMarchingCubeList
 * Version: 0.91
 * Description:
 *  The all-in-one method of using VCB's Marching Cube with a list is described below.<br>
 *	See vcbMarchingCubeBlk for a similar method using a block instead of a list. <br>
 *  See vcbMCVertexBegin for an iterative method.
 *  <p>
 *  vcbMarchingCubeList is designed to be called as a single function from which a user
 *   can call and obtain an extracted isosurface stored in a vertex array as defined
 *   by OpenGL standards.
 *   <p>
 *   The vertex array can be rendered using&#58; <pre>
 *    glEnableClientState(GL_NORMAL_ARRAY);
 *    glEnableClientState(GL_VERTEX_ARRAY);
 *    glNormalPointer(GL_FLOAT, 6*sizeof(float), vdata+3);
 *    glVertexPointer(3, GL_FLOAT, 6*sizeof(float), vdata);
 *    glDrawElements(GL_TRIANGLES, nfacets*3, GL_UNSIGNED_INT, fdata); </pre>
 * Arguments:
 *   vcbdatatype datatype: The basic format of the volume, which can be either
 *               VCB_UNSIGNEDBYTE, VCB_SHORT, VCB_INT or VCB_FLOAT.
 *   void* block: pointer to the volume to be extracted. If wn == VCB_ONLYVERTEX, the volume
 *               is expected to be compacted in the space pointed to by block in this
 *               sequence&#58; <br>
 *					voxel(i,j,k) is the ((i*dsizes[1]+j)*diszes[2]+k)th value
 *	 <p>
 *             However if wn == VCB_WITHNORMAL, then user must provide per voxel normal,
 *             and the voxel value on (i,j,k) should be at&#58; <br>
 *				((i*dsizes[1]+j)*diszes[2]+k)*4
 *	 <p>
 *             The x, y, and z component of the per vertex normal are at&#58; <br>
 *		((i*dsizes[1]+j)*diszes[2]+k)*4 + 1 <BR>
 *		((i*dsizes[1]+j)*diszes[2]+k)*4 + 2 <BR>
 *		((i*dsizes[1]+j)*diszes[2]+k)*4 + 3, respectively.
 *   float isoval: a floating point describing the isovalue for which an isosurface
 *               should be extracted
 *   int* dsizes: dsizes[0] - the size of the x dimension <BR>
 *             dsizes[1] - the size of the y dimension <BR>
 *             dsizes[2] - the size of the z dimension
 *	 int ncells:	The number of cells in cellids
 *	 short* cellids:	The list of cell ids
 *   int wn: A flag to signal VCB's Marching Cube Algorthim to extract the isosurface
 *           with or without normals. To extract the isosurface without normals is about
 *           30% ~ 40% faster. Correspondingly, the choices are VCB_WITHNORMAL or VCB_ONLYVERTEX,
 *           which are both macros defined in vcbmcube.h.
 *  int* nverts: Number of vertices.
 *  float** varray_v: Output. An interleaved floating point vertex array in 
 *             the following order&#58;
 *				 <table width=400>
 *               <tr><td>  floats 0-2   <td>  coordinate of v1
 *               <tr><td>  floats 3-5   <td>  normal of v1 
 *               <tr><td>  floats 6-8   <td>  coordinate of v2
 *               <tr><td>  floats 9-11  <td>  normal of v2
 *               <tr><td>  floats 12-14 <td>  coordinate of v3
 *               <tr><td>  floats 15-17 <td>  normal of v3 
 *               <tr><td>  floats 18-20 <td>  coordinate of v4
 *               <tr><td>  floats 21-23 <td>  normal of v4
 *               <tr><td>  ......		<td>  </table>
 *  int** varray_i: Output. An integer index array which implicity references 
 *             vdata to describe each triangle. <BR>
 *             For instance, if fdata contains the following data&#58; <BR>
 *                0 1 2 3 4 5 0 6 4 2 5 3 <BR>
 *             then we have trianges formed by vertices&#58; <BR>
 *                0, 1, 2; 3, 4, 5; 0, 6, 4; 2, 5, 3; respectively.
 *
 * Return: int; Total number of triangles (nfacets) in the triangle mesh
 * Known Issues: None
 **/
int  vcbMarchingCubeList(vcbdatatype datatype, void * block, float isoval, int *dsizes, int ncells, short * cellids, int wn, int * nverts, float ** varray_v, int ** varray_i);


#ifdef __cplusplus
}  /* extern C */
#endif


#endif
