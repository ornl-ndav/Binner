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

#ifndef _VCBUTILS_H
#define _VCBUTILS_H

#include <stdio.h>

#ifndef VCB_MAXVAL
#define VCB_MAXVAL(x,y) ((x) > (y) ? (x) : (y))
#endif
#ifndef VCB_MINVAL
#define VCB_MINVAL(x,y) ((x) > (y) ? (y) : (x))
#endif

/* VCB_BIGENDIAN signifies workstation type byte order */
#ifndef VCB_BIGENDIAN
#define VCB_BIGENDIAN    0 
#endif
/* VCB_LITTLEENDIAN signifies PC type byte order */
#ifndef VCB_LITTLEENDIAN
#define VCB_LITTLEENDIAN 1 
#endif

/** Define
 * Name: vcb_tl_interp
 * Version: 0.95
 * Description:  This macro handles trilinear interpolation
 **/
#ifndef vcb_tl_interp
#define vcb_tl_interp(s,r,t,f1,f2,f3,f4,f5,f6,f7,f8)  ( \
                                  ((1-(s))*(1-(r))*(1-(t))*(f1))  +  \
                                  ((  s  )*(1-(r))*(1-(t))*(f2))  +  \
                                  ((1-(s))*(  r  )*(1-(t))*(f3))  +  \
                                  ((  s  )*(  r  )*(1-(t))*(f4))  +  \
                                  ((1-(s))*(1-(r))*(  t  )*(f5))  +  \
                                  ((  s  )*(1-(r))*(  t  )*(f6))  +  \
                                  ((1-(s))*(  r  )*(  t  )*(f7))  +  \
                                  ((  s  )*(  r  )*(  t  )*(f8))   )
#endif

/** Enum
 * Name: vcbdatatype 
 * Version: 0.96
 * Description:
 *  typedefined&#58; vcbdatatype <br>
 *  vcbdatatype is an enumerator that the VCB uses to describe the type of 
 *  data contained within a data set.
 * Elements:
 *  VCB_UNSIGNEDBYTE: 8  bits unsigned integer
 *  VCB_BYTE: 8  bits integer 
 *  VCB_UNSIGNEDSHORT: 16 bits unsigned short integer
 *  VCB_SHORT: 16 bits short integer 
 *  VCB_UNSIGNEDINT: 32 bits unsigned binary integer 
 *  VCB_INT: 32 bits binary integer 
 *  VCB_FLOAT: 32 bits floating point values 
 *  VCB_DOUBLE: 64 bits floating point values 
 *  VCB_BITFIELD: 32 bits bitmask value 
 **/
typedef enum _vcbDATATYPE {
	 VCB_UNSIGNEDBYTE,     /* 8  bits unsigned integer */
	 VCB_BYTE,             /* 8  bits integer */
	 VCB_UNSIGNEDSHORT,    /* 16 bits unsigned short integer */
	 VCB_SHORT,            /* 16 bits short integer */
	 VCB_UNSIGNEDINT,      /* 32 bits unsigned binary integer */
	 VCB_INT,              /* 32 bits binary integer */
	 VCB_FLOAT,            /* 32 bits floating point values */
	 VCB_DOUBLE,           /* 64 bits floating point values */
	 VCB_BITFIELD          /* 32 bits bitmask value */
} vcbdatatype;

typedef unsigned char  VCBubyte;
typedef char           VCBbyte;
typedef unsigned short VCBushort;
typedef short          VCBshort;
typedef unsigned int   VCBuint;
typedef int            VCBint;
typedef float          VCBfloat;
typedef double         VCBdouble;
typedef unsigned int   VCBbitfield;

/** Struct
 * Name: VCBdata1d
 * Version: 0.95
 * Description:
 *  typedefined&#58; * VCBdata1d <br>
 *  VCBdata1d is a one dimensional data structure to be used mainly for storing 
 *  the topology of a dataset, but has further applications.
 *	<p>
 *  VCBdata1d is used mainly to store data objects such as trianglemeshes and 
 *  vertexlists.  However, the functionality of VCBdata1d can also
 *  be applied for other applications that can make use of a one dimensional 
 *  data structure where the data members have some number of attributes.
 * Elements:
 *  vcbdatatype type: The type of data contained in the structure
 *    as defined in the vcbdatatype enumerator. 
 *  int isize1stDim: The number of data points.
 *  int inumAttribs: The number of attributes per data point. 
 *  void* v_list: A linear list of data point attributes 
 *   (density, pressue, entrophy), which is interleaved. 
 **/
typedef struct data1dStruct{
	
	/* "vcbdatatype" is the type of data contained in the structure as defined in the vcbdatatype enumerator. */
	vcbdatatype type;
	
	/* "isize1stDim" is the number of data points. */
	int    isize1stDim;

	/* "inumAttribs" is the number of attributes per data point. */
	int    inumAttribs;

	/* "v_list" is a linear list of data point attributes (density, pressue, entrophy), which is interleaved. */
	void * v_list;

}* VCBdata1d;

/* 
 * for triangle mesh, need to use a mf1 datastructure to keep topology
 *     MF1 * trianglemesh; interleaving v1, v2, v3, v1, v2, v3 ... 
 *     MF1 * vertexlist;   interleaving x, y, z, x, y, z ....
 */

/** Struct
 * Name: VCBdata2d
 * Version: 0.96
 * Description:
 *  typedefined&#58; * VCBdata2d <br>
 *  VCBdata2d is a two dimensional data structure that's main usage is to store 
 *  images or textures, but has further applications.
 * Elements:
 *  vcbdatatype type: The type of data contained in the structure
 *    as defined in the vcbdatatype enumerator. 
 *  int isize1stDim: The number of data points in first dimension.
 *  int isize2ndDim: The number of data points in second dimension.
 *  float  fscale1stDim: The scaling of 1st dimension
 *  float  fscale2ndDim: The scaling of 2nd dimension
 *  int inumAttribs: The number of attributes per data point. 
 *  void* v_list: A linear list of data point attributes 
 *   (density, pressue, entrophy), which is interleaved. 
 **/
typedef struct data2dStruct{
	
	/* "vcbdatatype" is the type of data contained in the structure as defined in the vcbdatatype enumerator. */
	vcbdatatype type;

	/* "isize1stDim" and "isize2ndDim" is the size of first and second dimension, respectivly. */
	int    isize1stDim, isize2ndDim;

	/* "fscale1stDim" and "fscale2ndDim" is the scaling of 1st and 2nd dimension, respectivly. */
	float  fscale1stDim, fscale2ndDim;

	/* "inumAttribs" is the number of attributes per data point. */
	int    inumAttribs;

	/* "v_list" is a linear list of data point attributes (density, pressue, entrophy), which is interleaved. */
	void * v_list;

}* VCBdata2d; /* image, or texture */


/** Struct
 * Name: VCBdata3d
 * Version: 0.96
 * Description:
 *  typedefined&#58; * VCBdata3d <br>
 *  VCBdata3d is a three dimensional data structure for visualizing a three 
 *  dimensional data set. 
 *
 *  VCBdata3d has medical and scietific applications as far ranging as 
 *  representing a volumetric texture or a fuel ingection simulation.
 * Elements:
 *  vcbdatatype type: The type of data contained in the structure
 *    as defined in the vcbdatatype enumerator. 
 *  int isize1stDim: The number of data points in first dimension.
 *  int isize2ndDim: The number of data points in second dimension.
 *  int isize3rdDim: The number of data points in third dimension.
 *  float  fscale1stDim: The scaling of 1st dimension
 *  float  fscale2ndDim: The scaling of 2nd dimension
 *  float  fscale3rdDim: The scaling of 3rd dimension
 *  int inumAttribs: The number of attributes per data point. 
 *  void* v_list: A linear list of data point attributes 
 *   (density, pressue, entrophy), which is interleaved. 
 **/
typedef struct data3dStruct{
	
	/* "vcbdatatype" is the type of data contained in the structure as defined in the vcbdatatype enumerator. */
	vcbdatatype type;

	/* "isize1stDim", "isize2ndDim", and "isize3rdDim" is the size of first, second, and third dimension, respectivly. */
	int    isize1stDim, isize2ndDim, isize3rdDim;

	/* "fscale1stDim", "fscale2ndDim", and "fscale3rdDim" is the scaling of the 1st, 2nd, and 3rd dimension, respectivly. */
	float  fscale1stDim, fscale2ndDim, fscale3rdDim;

	/* "inumAttribs" is the number of attributes per data point. */
	int    inumAttribs;

	/* a linear list of data point attributes (density, pressue, entrophy), interleaved. */
	void * v_list;

}* VCBdata3d;

#ifndef vcbID1
#define vcbID1(d1d,i)     ((i)*(d1d)->inumAttribs)
#endif
#ifndef vcbID2
#define vcbID2(d2d,i,j)   (((j)*(d2d)->isize1stDim + (i))*(d2d)->inumAttribs)
#endif
#ifndef vcbID3
#define vcbID3(d3d,i,j,k) (((i)*(d3d)->isize2ndDim*(d3d)->isize3rdDim + (j)*(d3d)->isize3rdDim + (k))*(d3d)->inumAttribs)
#endif

#ifndef vcbAID1
#define vcbAID1(d1d,i,attrib)     (((i)*(d1d)->inumAttribs)+(attrib))
#endif
#ifndef vcbAID2
#define vcbAID2(d2d,i,j,attrib)   ((((j)*(d2d)->isize1stDim + (i))*(d2d)->inumAttribs)+(attrib))
#endif
#ifndef vcbAID3
#define vcbAID3(d3d,i,j,k,attrib) ((((i)*(d3d)->isize2ndDim*(d3d)->isize3rdDim + (j)*(d3d)->isize3rdDim + (k))*(d3d)->inumAttribs) + (attrib))
#endif

#ifndef vcb_p_ub
#define vcb_p_ub(vptr)  ((VCBubyte    *) vptr)
#endif
#ifndef vcb_p_b
#define vcb_p_b(vptr)   ((VCBbyte     *) vptr)
#endif
#ifndef vcb_p_us
#define vcb_p_us(vptr)  ((VCBushort   *) vptr)
#endif
#ifndef vcb_p_s
#define vcb_p_s(vptr)   ((VCBshort    *) vptr)
#endif
#ifndef vcb_p_ui
#define vcb_p_ui(vptr)  ((VCBuint     *) vptr)
#endif
#ifndef vcb_p_i
#define vcb_p_i(vptr)   ((VCBint      *) vptr)
#endif
#ifndef vcb_p_f
#define vcb_p_f(vptr)   ((VCBfloat    *) vptr)
#endif
#ifndef vcb_p_d
#define vcb_p_d(vptr)   ((VCBdouble   *) vptr)
#endif
#ifndef vcb_p_bit
#define vcb_p_bit(vptr) ((VCBbitfield *) vptr)
#endif

/** Struct
 * Name:  VCBpqe
 * Version: 0.95
 * Description:
 *  VCBpqe is a priority queue element used in vcbGrablk
 * Elements:
 *  int id: Identification number.
 *  int priority: Priority in the queue.
 **/
typedef struct 
{   /* Element's identification number. */
	int id;      
    /* Element's priority in the queue. */ 
	int priority;
} VCBpqe;

typedef int (* VCB_indexfunc)(int, int *, int *);

/* start function declarations */

#ifdef __cplusplus
extern "C" {
#endif

char * vcbStrDataType(vcbdatatype d);

/** Function
 * Name: vcbSizeOf
 * Version: 0.95
 * Description:
 *  "vcbSizeOf" returns the proper size of a given vcbdatatype.
 * Arguments:
 *  vcbdatatype type: The vcbdatatype who's size will be returned.
 * Return: int; size of type
 * Known Issues: None
 **/
int      vcbSizeOf(vcbdatatype type);

/** Function
 * Name: new_data1dstruct
 * Version: 0.95
 * Description:
 *  This is the function call for initilizing a VCBdata1d struct, 
 *  it returns NULL if it fails to initilize and a pointer to the VCBdata1d 
 *  struct when successful.
 * Arguments:
 *  vcbdatatype type: The type of data to be contained in the structure as
 *    defined in the vcbdatatype enumerator.
 *  int isizeDim: An integer describing the size of the first (and only) 
 *    dimension of the structure.
 *  int numAttribs: The number of attributes per data point.
 *  void* attribs: A pointer to the list of data point attributes to be used in 
 *   the VCBdata1d struct. The attribute list should have this format&#58 
 *   [attrib(0,0), attrib(0,1), ..., attrib(0, "numAttribs"), <br>
 *   attrib(1,0), attrib(1,1), ..., attrib("isizeDim","numAttribs")
 * Return: VCBdata1d; New data1dstruct
 * Known Issues: None
 **/
VCBdata1d new_data1dstruct (vcbdatatype type, int isizeDim, int numAttribs, void * attribs);

/** Function
 * Name: free_data1dstruct
 * Version: 0.95
 * Description:
 *  This deletes from memory a VCBdata1d struct. "vcbdata1d" 
 *   is the VCBdata1d structure to be deleted.
 * Arguments:
 *  VCBdata1d vcbdata1d: Data 1d struct to be deleted.
 * Return: None
 * Known Issues: None
 **/
void     free_data1dstruct(VCBdata1d vcbdata1d);

/** Function
 * Name: new_data2dstruct
 * Version: 0.95
 * Description:
 *  This is the function call for initilizing a VCBdata2d struct, 
 *  it returns NULL if it fails to initilize and a pointer to the VCBdata2d 
 *  struct when successful.
 *
 *  Note&#58 a VCBdata2d structure also needs the scaling of its dimensions to be 
 *  set before it can be used, so remember to also call "set_data2dscaling".
 * Arguments:
 *  vcbdatatype type: The type of data to be contained in the structure as
 *    defined in the vcbdatatype enumerator.
 *  int isize1stDim: An integer describing the size of the first dimension of 
 *    the structure.
 *  int isize2ndDim: An integer describing the size of the second dimension of 
 *    the structure.
 *  int numAttribs: The number of attributes per data point.
 *  void* attribs: A pointer to the list of data point attributes to be used in
 *   the VCBdata2d struct. The attribute list should have this format&#58 
 *   [attrib(0,0), attrib(0,1), ..., attrib(0, "numAttribs"), attrib(1,0), attrib(1,1), ..., attrib("isizeDim","numAttribs")
 * Return: VCBdata2d; New data2dstruct
 * Known Issues: None
 **/
VCBdata2d new_data2dstruct (vcbdatatype type, int isize1stDim, int isize2ndDim, int numAttribs, void * attribs);


/** Function
 * Name: set_data2dscaling
 * Version: 0.95
 * Description:
 *  This sets the scaling values to be used for the first and 
 *  second dimension.
 * Arguments:
 *  VCBdata2d vcbdata2d: The VCBdata2d structure to scale.
 *  int fscale1stDim: The scaling value to be used for the first dimension.
 *  int fscale2ndDim: The scaling value to be used for the second dimension.
 * Return: None
 * Known Issues: None
 **/
void     set_data2dscaling(VCBdata2d vcbdata2d, float fscale1stDim, float fscale2ndDim);

/** Function
 * Name: free_data2dstruct
 * Version: 0.95
 * Description:
 *  This deletes from memory a VCBdata2d struct. "vcbdata2d" is 
 *  the VCBdata2d structure to be deleted.
 * Arguments:
 *  VCBdata2d vcbdata2d: The VCBdata2d structure to be deleted.
 * Return: None
 * Known Issues: None
 **/
void     free_data2dstruct(VCBdata2d vcbdata2d);

/** Function
 * Name: new_data3dstruct
 * Version: 0.95
 * Description:
 *  This is the function call for initilizing a VCBdata3d struct,
 *  it returns NULL if it fails to initilize and a pointer to the VCBdata2d 
 *  struct when successful.
 *
 *  Note&#58 a VCBdata3d structure also needs the scaling of its dimensions to be 
 *  set before it can be used, so remember to also call "set_data3dscaling".
 * Arguments:
 *  vcbdatatype type: The type of data to be contained in the structure as
 *    defined in the vcbdatatype enumerator.
 *  int isize1stDim: An integer describing the size of the first dimension of 
 *    the structure.
 *  int isize2ndDim: An integer describing the size of the second dimension of 
 *    the structure.
 *  int isize3rdDim: An integer describing the size of the third dimension of 
 *    the structure.
 *  int numAttribs: The number of attributes per data point.
 *  void* attribs: A pointer to the list of data point attributes to be used in
 *   the VCBdata3d struct. The attribute list should have this format&#58 
 *   [attrib(0,0), attrib(0,1), ..., attrib(0, "numAttribs"), attrib(1,0), attrib(1,1), ..., attrib("isizeDim","numAttribs")
 * Return: VCBdata3d; New data3dstruct
 * Known Issues: None
 **/
VCBdata3d new_data3dstruct (vcbdatatype type, int isize1stDim, int isize2ndDim, int isize3rdDim, int numAttribs, void * attribs);

/** Function
 * Name: set_data3dscaling
 * Version: 0.95
 * Description:
 *  This sets the scaling values to be used for the first, 
 *  second and third dimensions.
 * Arguments:
 *  VCBdata2d vcbdata2d: The VCBdata2d structure to scale.
 *  int fscale1stDim: The scaling value to be used for the first dimension.
 *  int fscale2ndDim: The scaling value to be used for the second dimension.
 *  int fscale3rdDim: The scaling value to be used for the third dimension.
 * Return: None
 * Known Issues: None
 **/
void     set_data3dscaling(VCBdata3d vcbdata3d, float fscale1stDim, float fscale2ndDim, float fscale3rdDim);

/** Function
 * Name: free_data3dstruct
 * Version: 0.95
 * Description:
 *  This deletes from memory a VCBdata3d struct. "vcbdata3d" is 
 *  the VCBdata3d structure to be deleted.
 * Arguments:
 *  VCBdata3d vcbdata3d: The VCBdata3d structure to be deleted.
 * Return: None
 * Known Issues: None
 **/
void     free_data3dstruct(VCBdata3d vcbdata3d);


/* these are for handling binary files */

/** Function
 * Name: vcbBigEndian
 * Version: 0.95
 * Description:
 *  This tests whether the host machine is big/little endian.
 * Arguments: None
 * Return: int; 1 is big endian, 0 is little endian.
 * Known Issues: None
 **/
int      vcbBigEndian (void);

/** Function
 * Name: vcbToggleEndian
 * Version: 0.95
 * Description:
 *  This reverses the byte order of the given bytes from Big to
 *   Little endian or vice versa
 * Arguments: 
 *  void* dptr: Array whose bytes will be reversed.
 *  int nbytes: Number of bytes in dptr array.
 * Return: None
 * Known Issues: None
 **/
void     vcbToggleEndian(void * dptr, int nbytes);

/** Function
 * Name: vcbReadBinf
 * Version: 1.0
 * Description:
 *  This function reads in a standard VCB binary data file and returns a pointer
 *  to the multi-dimensional data loaded in (or NULL for failure).
 * Arguments:
 *  char* filename: A char array containing the name of file to read, the
 *     full name including extension (eg. *.bin.f or *.bin.ub)
 *  vcbdatatype rawdatatype: The type of binary data in the file as defined in
 *     the vcbdatatype enumerator.
 *  int endian: The endian order on the current machine (little/big endian).
 *  int* dims: The number of dimensions.
 *  int* dsize: An array of integers that specifies the number of voxels in
 *     each dimension. The array must be allocated outside the function.
 *  int* nattribs: Number of attibutes on each voxel. (all attributes are of
 *     the rawdatatype type)
 * Return: void*; vcbdatatype or NULL for failure.
 * Known Issues: None
 **/
void *   vcbReadBinf(char * filename, vcbdatatype rawdatatype, int endian, int * dims, int * dsize, int * nattribs);

/** Function
 * Name: vcbLoadBinm
 * Version: 0.95
 * Description:
 *  This function reads in an endian-safe VCB binary data file, which is mmap'ed
 *  into memory.
 * Arguments:
 *  void *: pointer to byte 0 of the mmap'ed file
 *  vcbdatatype rawdatatype: The type of binary data in the file as defined in
 *     the vcbdatatype enumerator.
 *  int endian: The endian order on the current machine (little/big endian).
 *  int* dims: The number of dimensions.
 *  int* dsize: An array of integers that specifies the number of voxels in
 *     each dimension. The array must be allocated outside the function.
 *  int* nattribs: Number of attibutes on each voxel. (all attributes are of
 *     the rawdatatype type)
 * Return: void*; pointer to the first byte of multi-dimensional array or
 *     NULL for failure.
 * Known Issues: None
 **/
void * vcbLoadBinm(void * raw, vcbdatatype * rawdatatype, int * dims, int * orig, int * dsize, int * nattribs);

/** Function
 * Name: vcbReadBinm
 * Version: 0.95
 * Description:
 *  This function reads in an endian-safe VCB binary data file from the given
 *  vcbdatatype. It returns 1 for failure and 0 for success.
 * Arguments:
 *  char* filename: A char array containing the name of file to read
 *  vcbdatatype rawdatatype: The type of binary data in the file as defined in
 *    the vcbdatatype enumerator. <br>
 *  int* dims: The number of dimensions.
 *  int* orig: The origin of the block contained in this file
 *    partitioned data use non-zero orig to define its true location
 *  int* dsize: An array of integers that specifies the number of voxels in
 *    each dimension. The array must be allocated outside the function.
 *  int* nattribs: Number of attibutes on each voxel. (all attributes are of
 *    the rawdatatype type)
 *  void* rawdata: A pointer to the array of raw data.
 * Return: void*; pointer to the first byte of multi-dimensional array  or
 *     NULL for failure.
 * Known Issues: None
 **/
void * vcbReadBinm(char * filename, vcbdatatype * rawdatatype, int * dims, int * orig, int * dsize, int * nattribs);

/** Function
 * Name: vcbWriteBinf
 * Version: 1.0
 * Description:
 *  This function writes a standard VCB binary data file from the given 
 *  vcbdatatype. It returns 0 for failure and 1 for success.
 * Arguments: 
 *  char* filename: A char array containing the name of file to write, the 
 *     full name excluding the extension (eg. *.bin). An extension signifying the
 *	   datatype will be appended (eg. *.bin.ub)
 *  vcbdatatype rawdatatype: The type of binary data in the file as defined in 
 *     the vcbdatatype enumerator. <br>
 *  int endian: The endian order on the current machine (little/big endian).
 *  int* dims: The number of dimensions.
 *  int* dsize: An array of integers that specifies the number of voxels in 
 *     each dimension. The array must be allocated outside the function.
 *  int* nattribs: Number of attibutes on each voxel. (all attributes are of 
 *     the rawdatatype type)
 *  void* rawdata: A pointer to the array of raw data.
 * Return: int; 0 for failure, 1 for success
 * Known Issues: None
 **/
int      vcbWriteBinf(char * filename, vcbdatatype rawdatatype, int endian, int dims, int * dsize, int nattribs, void * rawdata);

/** Function
 * Name: vcbGenBinm
 * Version: 0.95
 * Description:
 *  This function writes a endian-safe VCB binary data file from the given
 *  vcbdatatype. It returns 1 for failure and 0 for success.
 * Arguments:
 *  char* filename: A char array containing the name of file to write, the
 *    full name excluding the extension (eg. *.bin). An extension signifying the
 *    datatype will be appended (eg. *.bin.ub)
 *  vcbdatatype rawdatatype: The type of binary data in the file as defined in
 *    the vcbdatatype enumerator. <br>
 *  int dims: The number of dimensions.
 *  int* orig: The origin of the block contained in this file
 *    partitioned data use non-zero orig to define its true location
 *  int* dsize: An array of integers that specifies the number of voxels in
 *    each dimension. The array must be allocated outside the function.
 *  int* nattribs: Number of attibutes on each voxel. (all attributes are of
 *    the rawdatatype type)
 *  void* rawdata: A pointer to the array of raw data.
 * Return: int; 1 for failure, 0 for success
 * Known Issues: None
 **/
int      vcbGenBinm(char * filename, vcbdatatype rawdatatype, int dims, int * orig, int * dsize, int nattribs, void * rawdata);

/** Function
 * Name: vcbMakeBinm
 * Version: 0.95
 * Description:
 *  This function writes a endian-safe VCB binary data file from the given
 *  vcbdatatype, into a memory segment. Its intended use is for writing to
 *  a mmap'ed file. It returns 1 for failure and 0 for success.
 * Arguments:
 *  char* output: A memory segment to write to.
 *  vcbdatatype rawdatatype: The type of binary data in the file as defined in
 *    the vcbdatatype enumerator. <br>
 *  int dims: The number of dimensions.
 *  int* orig: The origin of the block contained in this file
 *    partitioned data use non-zero orig to define its true location
 *  int* dsize: An array of integers that specifies the number of voxels in
 *    each dimension. The array must be allocated outside the function.
 *  int* nattribs: Number of attibutes on each voxel. (all attributes are of
 *    the rawdatatype type)
 *  void* rawdata: A pointer to the array of raw data.
 * Return: int; 1 for failure, 0 for success
 * Known Issues: None
 **/
int      vcbMakeBinm(char * output, vcbdatatype rawdatatype, int dims, int * orig, int * dsize, int nattribs, void * rawdata);


/** Function
 * Name: vcbDefaultIndex
 * Version: 0.96
 * Description:
 *	This is the default indexing function assumed by all vcb functions. <br>
 *	coords values start from 0. The function is defined as follows&#58; <pre>
 *    int vcbDefaultIndex(int ndims, int * dsizes, int * coords)
 *    {
 *          int i, id = coords[0];
 *          for (i = 1; i < ndims; i ++)
 *              id = id*dsizes[i] + coords[i];
 *          return id;
 *    }</pre>
 * Arguments:
 *	int ndims:	The number of dimensions in coords
 *	int* dsizes:	The sizes of each dimenson
 *	int* coords:	The coords array
 * Return: int; The index
 * Known Issues: None
 **/
/* the default indexing function that all vcb functions assume by default */
int vcbDefaultIndex(int ndims, int * dsizes, int * coords);

/** Function
 * Name: vcbDefaultIndexToCoords
 * Version: 1.0
 * Description:
 *  Many volumes are indexed by increasing z-values, then increasing y-values,
 *  then increasing x-values, and so on.  For example, a 2 by 2 by 2 volume 
 *  identifies voxels in the following manner:
 *  <blockquote>
 *  Voxel 0: (0 0 0)
 *  Voxel 1: (0 0 1)
 *  Voxel 2: (0 1 0)
 *  Voxel 3: (0 1 1)
 *  Voxel 4: (1 0 0)
 *  Voxel 5: (1 0 1)
 *  Voxel 6: (1 1 0)
 *  Voxel 7: (1 1 1)
 *  </blockquote>
 *  This function's purpose is to map a voxel's id to its <i>n</i>-dimensional
 *  coordinates.  It's the inverse of vcbDefaultIndex, which maps coordinates
 *  to identifiers.
 *
 *  <p>Please note that this function assumes the voxel is contained within
 *  the bounds of the volume.  Indices that exceed this range will generate 
 *  no warning.
 * Arguments:
 *  int ndims: The number of dimensions in the space where voxels are being
 *   identified.
 *  int * dsizes: An array of <tt>ndims</tt> elements specifying the bounds
 *   of the volume whose voxels are being located.
 *  int index: The id of the voxel to be located.
 *  int * coords: A user-allocated array of <tt>ndims</tt> elements in
 *   the located voxels coordinates will be placed.
 *  Return: None
 *  Known Issues: None
 **/
void vcbDefaultIndexToCoords(int ndims, int *dims, int index, int *coords);

/** Function
 * Name: vcbGrablk
 * Version: 0.95
 * Description:
 *  This is a utility function that grabs a block from binary data. 
 *  It returns -1 for failure, >= 0 for the actual number of data elements 
 *  grabbed from raw binary data.
 *  <p>
 *  Note&#58; vcbGrablk will change ubounds and lbounds if they exceed the
 *   dimensional extent of the data.
 * Arguments: 
 *  void* src: The raw binary data of ndims that it is being read in.
 *  void* dest: A pointer to allocated memory that holds the grabbed block of data.
 *  int nattribs: The number of attributes on each data element.
 *  int tvalsize: The number of bytes of each value in the binary data.
 *  int tndims: The number of dimensions that the raw binary data (src) has.
 *  int* dsizes: An array of integers that specifies the number of voxels in 
 *    each dimension. The array is allocated outside the function
 *  int* lbounds: Describes the lower bounds for the block to be grabbed.
 *    The lower and upper bounds are inclusive when reading, and no
 *    clipping is done.  The function always assumes a malloc'ed buffer
 *    of a size specified by the bounds, and it fills all space outside
 *    of src's domain with zeros. <br>
 *    Note&#58; This value will be changed if it exceeds the dimensional
 *     extent of data.
 *  int* ubounds: Describes the upper bounds for the block to be grabbed.
 *    The lower and upper bounds are inclusive when reading, and no
 *    clipping is done.  The function always assumes a malloc'ed buffer
 *    of a size specified by the bounds, and it fills all space outside
 *    of src's domain with zeros. <br>
 *    Note&#58; This value will be changed if it exceeds the dimensional
 *     extent of data.
 *  int (*indexfunc): If you do not index your volume using the VCB convention, 
 *    please provide an index function
 *    that uses the same interface as the default below.
 *    If you would like to use the default, simply give NULL for indexfunc.
 *    The default index function used is as follows&#58;
 *    <pre>
 * int vcbDefaultIndex(int ndims, int * dsizes, int * coords)
 * {
 *	int i, id = coords[0];
 *	for (i = 1; i < ndims; i ++)
 *		id = id*dsizes[i] + coords[i];
 *	return id;
 * }</pre>
 * Return: int; -1 for failure, >= 0 for the actual number of data elements 
 *  grabbed from raw binary data.
 * Known Issues: None
 **/
int      vcbGrablk(void * src, void * dest, int nattribs, int valsize, int ndims, int * dsizes, int * lbounds, int * ubounds, int (* indexfunc)(int, int *, int *));

/** Function
 * Name: vcbFileGrablk
 * Version: 0.95
 * Description:
 *   This function is inherently the same as its counterpart vcbGrablk, which reads
 *    a block from a serialized multi-dimensional dataset in memory.
 *    <p>
 *    bindata is an opened stream containing the multi-dimensional dataset.
 *    firstbyte tells where in the dataset is the firstbyte of the volume itself; often
 *    dataset files have quite complex headers (firstbyte is where the volume starts)
 *   <p>
 *   Note&#58; FILE * bindata must be already opened. <br>
 *       If the header of the data file is ascii, although the volume data itself is
 *       binary, the safe way to get the value of firstbyte is to move the stream
 *       pointer to the end of the ascii part by using fgets, etc., and then call
 *       <pre> firstbyte = ftell(bindata);</pre>
 * Arguments:
 *  FILE* bindata: The opened input file that it is to be read in.
 *  long firstbyte: The location of the first byte of the volume.
 *  void* boutdata: A pointer to allocated memory that will hold the grabbed block of data.
 *  int nattribs: The number of attributes on each data element.
 *  int tvalsize: The number of bytes of each value in the binary data.
 *  int tndims: The number of dimensions that the raw binary data has.
 *  int* dsizes: An array of integers that specifies the number of voxels in 
 *    each dimension. The array is allocated outside the function
 *  int* lbounds: Describes the lower bounds for the block to be grabbed.
 *    The lower and upper bounds are inclusive when reading, and no
 *    clipping is done.  The function always assumes a malloc'ed buffer
 *    of a size specified by the bounds, and it fills all space outside
 *    of src's domain with zeros. <br>
 *    Note&#58; This value will be changed if it exceeds the dimensional
 *     extent of data.
 *  int* ubounds: Describes the upper bounds for the block to be grabbed.
 *    The lower and upper bounds are inclusive when reading, and no
 *    clipping is done.  The function always assumes a malloc'ed buffer
 *    of a size specified by the bounds, and it fills all space outside
 *    of src's domain with zeros. <br>
 *    Note&#58; This value will be changed if it exceeds the dimensional
 *     extent of data.
 *  int (*indexfunc): If you do not index your volume using the VCB convention, 
 *    please provide an index function
 *    that uses the same interface as the default below.
 *    If you would like to use the default, simply give NULL for indexfunc.
 *    The default index function used is as follows&#58;
 * <pre>
 * int vcbDefaultIndex(int ndims, int * dsizes, int * coords)
 * {
 *	int i, id = coords[0];
 *	for (i = 1; i < ndims; i ++)
 *		id = id*dsizes[i] + coords[i];
 *	return id;
 * }</pre>
 * Return: int; -1 for failure, >= 0 for the actual number of data elements 
 *  grabbed from raw binary data.
 * Known Issues: None
 **/
int      vcbFileGrablk(FILE * bindata, long firstbyte, void * boutdata, int nattribs, int valsize, int ndims, int * dsizes,int * lbounds, int * ubounds, int (* indexfunc)(int, int *, int *));

/** Function
 * Name: vcbReadHeaderBinf
 * Version: 0.95
 * Description:
 *  Reads endian order (endian), number of dimesnions (dims), "dims" voxels (dsize),
 *  and number of attributes on each voxel (nattribs) from the input file.
 * Arguments:
 *  char* filename:  The name of file to read, the full name including extension
 *  int endian:      The endian order on the current machine (little/big endian)
 *  int* dims:       The number of dimensions
 *  int* dsize:      The number of voxels in each dimension, in an array allocated outsize this function
 *  int* nattribs:   The  number of attibutes on each voxel.
 * Return: None
 * Known Issues: None
 **/		 
void vcbReadHeaderBinf(char * filename, int endian, int * dims, int * dsize, int * nattribs);

int vcbReadHeaderBinm(char * filename, vcbdatatype * file_datatype, int * dims, int * orig, int * dsize, int * nattribs);


/** Function
 * Name: vcbVoxelsBetweenPoints
 * Version: 1.1
 * Description: This function returns a list of voxel ids from a volume of 
 *  arbitrary dimension.  The ids returned are ones that lie on a line segment
 *  specified by two endpoints.  Intermediate voxels are linearly interpolated,
 *  allowing users to grab a spatial and/or temporal series.
 *
 *  This function is <i>O(ndims x m)</i>, where <i>m</i> is the number of
 *  voxels in the returned list.
 * Arguments:
 *  int ndims: The number of dimensions for the volume.
 *  int * dsizes: An array of <tt>ndims</tt> elements, with each element
 *   specifying the extent of the volume in the indexed dimension.
 *  float * p1: An array of <tt>ndims</tt> elements, specifying one endpoint
 *   of the line segment.
 *  float * p2: An array of <tt>ndims</tt> elements, specifying the other 
 *   endpoint of the line segment.
 *  int * nvoxels: A pointer whose value will be filled with the resultant
 *   number of voxels found on the line segment.  Any value stored here
 *   will be lost when this function is called.
 *  int (*index_function) (int, int *, int *): A pointer to a function that,
 *   when passed <tt>ndims</tt>, <tt>dsizes</tt>, and an array of integer 
 *   coordinates with <tt>ndims</tt> elements, will return the index of the
 *   corresponding voxel.  A value of NULL here implicitly causes the function  
 *   vcbDefaultIndex to be used.  See also vcbIndexXYZEtc.
 * Return: int *; A pointer to an ordered array of <tt>*nvoxels</tt> ids is 
 *  returned by this function.  Each element is an integer id returned by
 *  the specified <tt>index_function</tt>.  The first element of the list is
 *  the voxel closest to <tt>p1</tt> and the last, closest to <tt>p2</tt>. 
 *  When users are finished with this list, they should free the memory pointed
 *  to by this address.
 * Known Issues: None
 **/
int *vcbVoxelsBetweenPoints(int ndims, int *dsizes, float *p1, float *p2,
                            int *nvoxels,
                            int (*index_function) (int, int *, int *));
/** Function
 * Name: vcbNewVolume
 * Version: 1.0
 * Description: This function allocates space for a new volume given the
 *  specified dimensions and attributes.
 * Arguments:
 *  int ndims: The number of dimensions for the volume.
 *  int * dsizes: An array of <tt>ndims</tt> elements, with each element 
 *   specifying the extent of the volume in the indexed dimension.
 *  int nattribs: The number of attributes at each voxel in the volume.
 *  int valsize: The number of bytes required for each attribute.
 * Return: void *; A pointer to the freshly allocated volume is returned.  This
 *  is generically cast as a <tt>void *</tt>, but users may cast it into the
 *  attribute type for simpler indexing.  When users are finished with the
 *  volume, they should free the memory pointed to by this address.
 * Known Issues: None
 **/
void * vcbNewVolume(int ndims, int *dsizes, int nattribs, int valsize);

/** Function
 * Name: vcbVoxelsIntoVolume
 * Version: 1.01
 * Description: This function copies the values of certain voxels from one
 *  volume to another.  The volumes should be identical in "shape" and the
 *  the voxels specified should be a list of integer ids, perhaps returned
 *  from vcbVoxelsBetweenPoints.  The utility of this function comes in 
 *  generating several neighhboring line segments and picking out the 
 *  intersected voxels.  Interesting cross-sections and shapes of volumes can
 *  be extracted.
 * Arguments:
 *  int nids: The number of elements or ids in <tt>ids</tt>.
 *  int * ids: An array of integer voxel ids specifying which voxel values 
 *   should be copied between volumes.
 *  void * src: A pointer to source volume containing the voxel values to be
 *   copied.
 *  void * dst: A pointer to the destination volume where voxel values will be
 *   copied to.
 *  int ndims: The number of dimensions in either volume.
 *  int * dsizes: An array of <tt>ndims</tt> elements, with each element 
 *   specifying the extent of either volume in the indexed dimension.
 *  int nattribs: The number of attributes at each voxel in the volume.
 *  int valsize: The number of bytes required for each attribute.
 * Return: None
 * Known Issues: None
 **/
void vcbVoxelsIntoVolume(int nids, int *ids, void *src, void *dst, int ndims,
                         int *dsizes, int nattribs, int valsize);

/** Function
 * Name: vcbIndexXYZEtc
 * Version: 1.1
 * Description:
 *  Many volumes are indexed by increasing x-values, then increasing y-values,
 *  then increasing z-values, and so on.  For example, a 2 by 2 by 2 volume 
 *  identifies voxels in the following manner:
 *  <blockquote>
 *  Voxel 0: (0 0 0)
 *  Voxel 1: (1 0 0)
 *  Voxel 2: (0 1 0)
 *  Voxel 3: (1 1 0)
 *  Voxel 4: (0 0 1)
 *  Voxel 5: (1 0 1)
 *  Voxel 6: (0 1 1)
 *  Voxel 7: (1 1 1)
 *  </blockquote>
 *  This function then maps coordinates to the containing voxel's id for
 *  volumes arranged in this manner.
 * Arguments:
 *  int ndims: The number of dimensions in the space where voxels are being
 *   identified.
 *  int * dsizes: An array of <tt>ndims</tt> elements specifying the bounds
 *   of the volume whose voxels are being identified.
 *  int * coords: An array of <tt>ndims</tt> elements specifying a point in
 *   the voxel that needs to be identified.
 *  Return: int; An integer id of the voxel specified by <tt>coords</tt>.
 *  Known Issues: None
 **/
int vcbIndexXYZEtc(int ndims, int *dsizes, int *coords);

/** Function
 * Name: vcbIndexXYZEtcToCoords
 * Version: 1.0
 * Description:
 *  Many volumes are indexed by increasing x-values, then increasing y-values,
 *  then increasing z-values, and so on.  For example, a 2 by 2 by 2 volume 
 *  identifies voxels in the following manner:
 *  <blockquote>
 *  Voxel 0: (0 0 0)
 *  Voxel 1: (1 0 0)
 *  Voxel 2: (0 1 0)
 *  Voxel 3: (1 1 0)
 *  Voxel 4: (0 0 1)
 *  Voxel 5: (1 0 1)
 *  Voxel 6: (0 1 1)
 *  Voxel 7: (1 1 1)
 *  </blockquote>
 *  This function's purpose is to map a voxel's id to its <i>n</i>-dimensional
 *  coordinates.  It's the inverse of vcbIndexXYZEtc, which maps coordinates
 *  to identifiers.
 *
 *  <p>Please note that this function assumes the voxel is contained within
 *  the bounds of the volume.  Indices that exceed this range will generate 
 *  no warning.
 * Arguments:
 *  int ndims: The number of dimensions in the space where voxels are being
 *   identified.
 *  int * dsizes: An array of <tt>ndims</tt> elements specifying the bounds
 *   of the volume whose voxels are being located.
 *  int index: The id of the voxel to be located.
 *  int * coords: A user-allocated array of <tt>ndims</tt> elements in
 *   the located voxels coordinates will be placed.
 *  Return: None
 *  Known Issues: None
 **/
void vcbIndexXYZEtcToCoords(int ndims, int *dims, int index, int *coords);

/** Function
 * Name: vcbVolumeFromFunction
 * Version: 1.0
 * Description:
 *  Data in more than 3 dimensions can quickly become difficult to visualize.
 *  Even the added dimension of time can reduce interactivity, but this
 *  function serves to help reduce or rearrange volumes.  It's original purpose
 *  was to collapse time-variant 4-D data into a static 3-D volume that showed
 *  the data's evolution along the <tt>x</tt>-axis.  As <tt>x</tt> increased,
 *  data from increasing timesteps was displayed.
 *
 *  <p>Certainly the utility of this function extends beyond time reduction.
 *  Other ideas include rearrangement of voxels, volume expansion to
 *  introduce gaps and isolate individual voxels, volume filtering to remove
 *  voxels matching certain criteria or in certain bounds, and volume joining.
 *  The flexibility lies in the user-supplied function that maps destination
 *  volume voxels to the source volume.
 * 
 *  <p>Here is a function that consolidates a 4-D, time-variant volume into a
 *  3-D volume.  As the first dimension increases, it is mapped to voxels
 *  in the source volume that have the same spatial location but occur at 
 *  later timesteps.  The proportion of current <tt>x</tt> value to maximum
 *  <tt>x</tt> value is multiplied by the maximum time value to get the correct
 *  time value for a voxel.  See vcbDefaultIndex and vcbIndexXYZEtc for voxel
 *  indexing functions.
 *
 *  <pre>
 *    void dst_src_indices(int dst_ndims, int *dst_dsizes, int *dst_coords,
 *                         int src_ndims, int *src_dsizes, int *src_coords,
 *                         int *dst_index, int *src_index) {
 *    
 *       *dst_index = vcbIndexXYZEtc(dst_ndims, dst_dsizes, dst_coords);
 *       src_coords[0] = dst_coords[0];
 *       src_coords[1] = dst_coords[1];
 *       src_coords[2] = dst_coords[2];
 *       src_coords[3] = (int) (((float) dst_coords[0] / dst_dsizes[0]) *
 *                               src_dsizes[3]);
 *       *src_index = vcbIndexXYZEtc(src_ndims, src_dsizes, src_coords);
 *    
 *    }
 *  </pre>
 *
 *  <p align="center"><img src="imgs/along_x.jpg">
 * 
 *  <p>The mapping doesn't have to be linear.  Using the absolute value 
 *  function, the time value can rise and then fall with <tt>x</tt>:
 *
 *  <pre>
 *    src_coords[3] =
 *       (int) ((dst_dsizes[0] - 1 - fabs(2 * dst_coords[0] - dst_dsizes[0])) /
 *              dst_dsizes[0] * src_dsizes[3]);
 *  </pre>
 *
 *  <p align="center"><img src="imgs/abs.jpg">
 *
 *  <p>Similar and more complicated things can be done along arbitrary axes, 
 *  with logs, trigonometric functions, and so on.  To indicate that a 
 *  destination voxel should be left as is and not copied into from the source
 *  volume, assign <tt>dst_index</tt> or <tt>src_index</tt> as -1.  (But make
 *  sure that these voxels do have a value by either assigning it yourself or
 *  initially <tt>calloc</tt>ing the volume.)  This technique allows you to
 *  filter out unwanted voxels.  For instance, inserting the following code
 *  snippet at the start of your function retains only a spherical chunk
 *  (with a radius of 25, measured from the center of the volume) of the source
 *  volume:
 *
 *  <pre>
 *    float len; 
 *    int tmp;
 *    int i;
 *
 *    // Find coordinates' distance from the center of the volume.
 *    len = 0.0f;
 *    for (i = 0; i < dst_ndims; i++) {
 *       tmp = dst_coords[i] - dst_dsizes[i] / 2;
 *       len += tmp * tmp;
 *    }
 *    len = sqrt(len);
 *
 *    // Exclude any voxels outside sphere of radius 25.
 *    if (len > 25) {
 *       *dst_index = -1;
 *       *src_index = -1;
 *       return;
 *    }
 *  </pre>
 *
 *  <p align="center"><img src="imgs/ball.jpg">
 *
 *  <p>You can then map included voxels in any manner you wish to the source
 *  volume.
 *
 *  <p>Please note that this function works in voxel space, not with geometry.
 *  Resultant volumes may be written to disk for later use or you may perform
 *  surface extraction and display immediately.  Reducing a source volume of
 *  many dimensions to a destination volume of fewer dimensions is ideal for
 *  a fast visualization process.  Disk reads for the source volume will 
 *  dominate running time, but a compacted destination volume may be more 
 *  efficient to work with.
 *
 *  <p>This function is <i>O(number of destination voxels)</i>.
 *
 * Arguments:
 *  int src_ndims: The number of dimensions in the source volume.
 *  int * src_dsizes: An array of <tt>src_ndims</tt> elements specifying
 *   the extent of the source volume in each dimension.
 *  void * src: A pointer to the source volume.
 *  int dst_ndims: The number of dimensions in the destination volume.
 *  int * dst_dsizes: An array of <tt>src_ndims</tt> elements specifying
 *   the extent of the destination volume in each dimension.
 *  void * dst: A pointer to the destination volume.
 *  int voxel_size: The number of bytes consumed by each voxel.  This should
 *   include all attributes.
 *  void (* dst_src_index_function):
 *   A pointer to a function which determines the mapping between destination
 *   volume coordinates to destination voxel index, source volume coordinates,
 *   and source voxel index.  This function must assign values to
 *   <tt>*dst_index</tt> and <tt>*src_index</tt>.  The <tt>*src_coords</tt>
 *   array has been allocated but not assigned, since the mapping from
 *   destination coordinates to source coordinates is user-defined.  The
 *   first five variables contain information describing the source and
 *   destination volumes.  See examples in this ingredient's description,
 *   and the prototype below.
 *
 *   <pre>
 *    void (* dst_src_index_function)
 *              (int dst_ndims, int *dst_dsizes, int *dst_coords,
 *               int src_ndims, int *src_dsizes, int *src_coords,
 *               int *dst_index, int *src_index);
 *   </pre>
 * Return: None
 * Known Issues: None
 **/
void vcbVolumeFromFunction(int src_ndims, int *src_dsizes, void *src,
                           int dst_ndims, int *dst_dsizes, void *dst,
                           int voxel_size,
                           void (* dst_src_index_function) (int, int *, int *,
                                                            int, int *, int *,
                                                            int *, int *));

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
