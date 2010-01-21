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

#ifndef _VCB_TRIMESH_H 
#define _VCB_TRIMESH_H

#ifdef __cplusplus
extern "C" {
#endif

/* the trimesh generation API */

/** Function
 * Name: vcbTrimeshStl
 * Version: 0.9
 * Description:
 *  vcbTrimeshStl takes an ASCII STL file, where all triangles are stored as 
 *  independent facets, and creates a triangle mesh. In the STL file, the 
 *  coordinates of each vertex are given as each triangle is described.  The 
 *  function finds all distinctive vertices and produces a vertex array (vdata)
 *  and an indices array (fdata), which abide by OpenGL vertex array 
 *  definitions.
 * Arguments:
 *  char* filename: Input. The name of the file that contains the triangle mesh 
 *   data. Absolute path may be needed if outside of directory.
 *  int* nverts: Output. A pointer to an integer that will hold the number of 
 *   vertices in the mesh after it is read in.
 *  int* nfacets: Output. A pointer to an integer that will hold the number of
 *   facets in the mesh after it is read in.
 *  float** vdata: Output. A pointer to an array of vertices.  Each vertex 
 *   contains three floats.
 *  int** fdata: Output. A pointer to an array of facets. Each facet has three 
 *   integers that are the indices for three vertices in vdata.
 *  float** fnormals: Output. Once the file is read, fnormals is a pointer to
 *   arrays of floats that describe each facet's normal. One normal per triangle.
 * Return: None
 * Known Issues: None
 **/
void vcbTrimeshStl(char * filename, int * nverts, int * nfacets, float ** vdata, int ** fdata, float ** fnormals);

/** Function
 * Name: vcbTrimeshStlb
 * Version: 0.9
 * Description:
 *  vicbTrimeshStlb takes a binary STL file, where all triangles are stored as 
 *  ndependent facets, and creates a triangle mesh. In the STL file, the 
 *  coordinates of each vertex are given as each triangle is described.  The
 *  function finds all distinctive vertices and produces a vertex array (vdata)
 *  and an indices array (fdata), which abide by OpenGL vertex array 
 *  definitions.
 * Arguments:
 *  char* filename: Input. The name of the file that contains the triangle mesh 
 *   data. Absolute path may be needed if outside of directory.
 *  int* nverts: Output. A pointer to an integer that will hold the number of 
 *   vertices in the mesh after it is read in.
 *  int* nfacets: Output. A pointer to an integer that will hold the number of
 *   facets in the mesh after it is read in.
 *  float** vdata: Output. A pointer to an array of vertices.  Each vertex 
 *   contains three floats.
 *  int** fdata: Output. A pointer to an array of facets. Each facet has three 
 *   integers that are the indices for three vertices in vdata.
 *  float** fnormals: Output. Once the file is read, fnormals is a pointer to
 *   arrays of floats that describe each facet's normal. One normal per triangle.
 * Return: None
 * Known Issues: None
 **/
void vcbTrimeshStlb(char * filename, int * nverts, int * nfacets, float ** vdata, int ** fdata, float ** fnormals);

/** Function
 * Name: vcbTrimeshRaw
 * Version: 0.91
 * Description:
 *  vcbTrimeshRaw is the core function within the vr_trimesh library. It takes 
 *  a linear list of floating point attributes organized in facets. <br>
 *  Example of raw data&#58;<br>
 *  v0_0, v0_1, v0_2, v0_3, ... v0_k, <br>
 *	v1_0, ... v1_k, <br>
 *	v2_0, ... v2_k, <br>
 *	v3_0, ... v3_k, <br>
 *	v4_0 ...
 *	<p>
 *  Here there are k attributes on each vertex, and v0, v1, v2 form the 1st
 *  triangle, v3, v4, and v5 form the 2nd triangle, etc. We require that the 
 *  first 3 attributes must be the coordinate of the vertex.
 *	<p>
 *  vcbTrimeshRaw gives back a vertex array in the same format as the facets
 *  array, but (*vdata) conatains distinctive vertices, and (*fdata) contains 
 *  indices into (*vdata) for each triangle.
 *	<p>
 *  Usage&#58;<br>
 *   The ability to handle more than 3 attributes per vertex is for situations where one needs
 *   to have per vertex normals. In this case, nval = 6. In the (*vdata) that is returned,
 *   it contains coordinates0, normal0, coordinates1, normal1, coordinates2, normal2, ... <br>
 *   To use (*vdata) in glVertexArray and glNormalArray, just be sure to set
 *      <pre>  stride = 6 * sizeof(float); </pre>
 *   The case where only coordinates are handled, just have nval = 3. Obviously, 
 *   (*vdata) can be used in glVertexArray with 
 *		<pre>  stride = 3 * sizeof(float); </pre>
 * Arguments:
 *  float* facets: Input. An array of raw facet data, organized triangle by triangle.
 *  int nval: Input. The number of attributes per vertex, k.
 *  int nfaces: Input. The number of triangles in the mesh.
 *  int nverts: Output. A pointer to an integer that will hold the number of facets
 *    in the mesh after it is read in.
 *  float** vdata: Output. The vertex array, a pointer to an array of vertices.  Each vertex contains three floats.
 *  int** fdata: Output. The indices array, a pointer to an array of facets. Each facet has three integers
 *    that are the indices for three vertices in vdata.
 * Return: None
 * Known Issues: None
 **/
void vcbTrimeshRaw(float * facets, int nval, int nfacets, int * nverts, float ** vdata, int ** fdata);

/* an internal function for debugging purpose */
int read_in_raw(const char *filename, float ** normals, float ** facets);

#ifdef __cplusplus
}  /* extern C */
#endif


#endif
