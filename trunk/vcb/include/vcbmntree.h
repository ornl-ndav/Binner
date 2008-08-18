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

#ifndef _VCB_MNTREE_H
#define _VCB_MNTREE_H

typedef void  (*vcbmntKeyUBFunc)    (unsigned char * key, void * dptr);
typedef void  (*vcbBoundKeyUBFunc)  (unsigned char * key, int * bf);
typedef int   (*vcbKeyCmpUBFunc)    (unsigned char * k1, unsigned char * k2);
typedef void  (*vcbKeyDecompUBFunc) (int nattrib, unsigned char * key, int * cl);

typedef void  (*vcbmntKeyFunc)    (float * key, void * dptr);
typedef void  (*vcbBoundKeyFunc)  (float * key, float * bf);
typedef int   (*vcbKeyCmpFunc)    (float * k1,  float * k2);
typedef void  (*vcbKeyDecompFunc) (int nattrib, float * key, float * cl);

/** Struct
 * Name: VCB_mntleaf
 * Version: 0.9
 * Description:
 *  The leaf node struct contains a list of items and the number of items in the list.
 * Elements:
 *	int nitems:          number of items stored 
 *	unsigned char* q:    contains a list of items
 **/
typedef struct _mnt_leaf {
	int nitems;          /* number of items stored */
	unsigned char * q;   /* contains a list of records */
} * VCB_mntleaf;

/** Struct
 * Name: VCB_mntd
 * Version: 0.9
 * Description:
 *	The mntree descriptor struct.
 *	At all times, either lower or leave remains NULL to distinguish
 *  whether or not this node is a leaf.
 * Elements:
 *	int              ilevel:      the current level in the mn_tree 
 *	int              degree:      the degree "m" 
 *	struct _mnt_d *  parent:      the parent of this tree node 
 *	int              whichkid:    which among the m child of the parent 
 *	unsigned char *  keys:        keys as unsigned integers 
 *	struct _mnt_d ** lower:		  will be NULL if this node points to a leaf
 *	VCB_mntleaf    * leave:		  will be NULL if this node points to a lower level
 **/
typedef struct _mnt_d {
	int              ilevel;     /* which level in the mn_tree */
	int              degree;     /* m */
	struct _mnt_d *  parent;     /* the parent of this tree node */
	int              whichkid;   /* among the m child of the parent, which is this one */

	/* at all times, either lower or leave remains NULL */
	unsigned char *  keys;       /* keys as unsigned integers */
	struct _mnt_d ** lower;
	VCB_mntleaf    *  leave;

} * VCB_mntd;                     /* mntree descriptor */

#ifndef VCB_MAXLEVELS
#define VCB_MAXLEVELS 10
#endif

/** Struct
 * Name: VCB_mntree
 * Version: 0.9
 * Description:
 *  The struct containing the root of the mntree and other attributes.<br>
 *  NOTE&#58; The current VCB_MAXLEVELS is 10
 * Elements:
 *	VCB_mntd root:	the root node
 *	int bracket_l[VCB_MAXLEVELS]:  left of the current bracket
 *	int bracket_r[VCB_MAXLEVELS]:  right of the current bracket
 *	int travers_i[VCB_MAXLEVELS]:  a temp index used throughout vcbmntree.c
 *	int already_i[VCB_MAXLEVELS]:  a temp index used throughout vcbmntree.c
 *	int numlevels:			the number of levels "n" in the mntree (VCB_MAXLEVELS 10)
 *	int degree:				the degree "m" of the mntree
 *	int keysz:				the total number of bytes for the key of each item/record (i.e. voxel)
 *	int qrecsz:				the record size
 *	int _itemsz:			the number of bytes of each item to be indexed in the mntree
 *	int nattr:				the number of attributes in each item
 *	vcbBoundKeyFunc  mntbfunc: compute a key given a boundary data
 *	vcbKeyCmpFunc    mntcfunc: compares two keys for logic relation
 *	vcbKeyDecompFunc mntdfunc: decomposes a key to an array of floating points
 *	vcbBoundKeyUBFunc  ubmntbfunc: compute a key given a boundary data
 *	vcbKeyCmpUBFunc    ubmntcfunc: compares two keys for logic relation
 *	vcbKeyDecompUBFunc ubmntdfunc: decomposes a key to an array of floating points
 **/
typedef struct _mnt_tree {
	VCB_mntd root;

	int bracket_l[VCB_MAXLEVELS];  /* left of the current bracket */
	int bracket_r[VCB_MAXLEVELS];  /* right of the current bracket */
	int travers_i[VCB_MAXLEVELS];  /* a temp index used throughout vcbmntree.c */
	int already_i[VCB_MAXLEVELS];  /* a temp index used throughout vcbmntree.c */

	int numlevels;
	int degree;
	int keysz;
	int qrecsz;
	int _itemsz;
	int nattr;

	vcbBoundKeyFunc  mntbfunc;
	vcbKeyCmpFunc    mntcfunc;
	vcbKeyDecompFunc mntdfunc;

	vcbBoundKeyUBFunc  ubmntbfunc;
	vcbKeyCmpUBFunc    ubmntcfunc;
	vcbKeyDecompUBFunc ubmntdfunc;
} * VCB_mntree;

#ifdef __cplusplus
extern "C" {
#endif

/* mntree API */

/** Directions for vcbmntree
 * <h2>Description:</h2>
 * The mntree is a data structure designed to handle multi-variate data sets.
 * There is no preset limit of number of variables to handle.
 * <p>
 * The general idea is to use a very "wide" but "shallow" tree. This is similar
 * to how btree is designed to limit the amount of memory usage by the search
 * data structure.
 * <p>
 * The name "mntree" comes from a tree of m-degree and n-depth.
 * If used as intended, m >> n should be true.<br>
 * Common combinations could be:<br>
 *   m = 64, n = 3, or m = 128, n = 2, etc.
 * <p>
 * The API contains 4 general functions:<br><table width=700>
 *  <tr><td>vcbMntreeBuild<td>An mntree must be built before it can be used.
 *  <tr><td>vcbMntreeQuery<td>After an mntree has been built, one can perform range queries.
 *  <tr><td>vcbMntreeFree<td>Use this to free the mntree.
 *  <tr><td>vcbMntreeStat<td>Use to obtain attributes of the mntree.
 *  </table>
 * <p>
 * The mntree data structure in general can handle any data type. However, to
 * obtain the best performance, runtime determination of data types should be avoided.<br>
 * Two separate APIs are provided for raw data: unsigned byte and floating point.
 * These are the two most widely used data types in visualization of data sets.
 * <p>
 * This choice affects vcbMntreeBuild and vcbMntreeQuery. Since floating point
 * is the most general data type, those original api names are used for floating
 * point data. For unsigned byte data, a "ub" postfix is added to each api.
 * 
 * <h2> Building an mntree </h2>
 * Build an mntree using either vcbMntreeBuild for floating point data or 
 *  vcbMntreeBuildub for unsigned byte data. Both take the same basic inputs:<br>
 *   m       - degree of tree <br>
 *   n       - depth of tree  <br>
 *   keysize - the total number of bytes for the key of each item/record (i.e. voxel) <br>
 *   nitems  - number of items (voxels) for the entire mntree <br>
 *   itemsz  - the number of bytes of each item to be indexed in the mntree <br>
 *   items   - a pointer to a linear array of items <br>
 *   nattr   - number of attributes in each item 
 * <p>
 * mntree assumes four function pointers from caller in the following types:<br><pre>
 *   typedef void  (*vcbmntKeyFunc)    (float * key, void * dptr);  // kfunc
 *   typedef void  (*vcbBoundKeyFunc)  (float * key, float * bf);   // bfunc
 *   typedef int   (*vcbKeyCmpFunc)    (float * k1,  float * k2);   // cfunc
 *   typedef void  (*vcbKeyDecompFunc) (int nattrib, float * key, float * cl); // dfunc </pre>
 *
 *   They perform the following tasks:<br><table width=700>
 *   <tr><td> kfunc <td> compute a key given an item
 *   <tr><td> bfunc <td> compute a key given a boundary data
 *   <tr><td> cfunc <td> compares two keys for logic relation
 *   <tr><td> dfunc <td> decomposes a key to an array of floating points
 *	</table><br>
 *   Note: the function pointers for ub types are different as below:<br><pre>
 *   typedef void  (*vcbmntKeyUBFunc)    (unsigned char * key, void * dptr);
 *   typedef void  (*vcbBoundKeyUBFunc)  (unsigned char * key, int * bf);
 *   typedef int   (*vcbKeyCmpUBFunc)    (unsigned char * k1, unsigned char * k2);
 *   typedef void  (*vcbKeyDecompUBFunc) (int nattrib, unsigned char * key, int * cl); </pre>
 * <p>
 * These function pointers are provided for flexibilities to later developers.<br>
 * For users, these functions are actually often very standard. We recommend the
 * following implementations:<br><pre>
 *    int my_nattr = 7; // global
 *
 *    static void  sparseKeyFunc(float * key, void * dptr)
 *    {
 *  	memcpy(key, dptr, my_nattr*sizeof(float));
 *    }
 *
 *    static void   boundKeyFunc(float * key, float * data)
 *    {
 *      int i;
 *      for (i = 0; i < my_nattr; i ++)
 *        key[i] = data[i];
 *    }
 *
 *    static void  decompKeyFunc(int nattrib, float * key, float * cl)
 *    {
 *      int i;
 *      for (i = 0; i < my_nattr; i ++)
 *        cl[i] = key[i];
 *    }
 *
 *    static int  compareKeyFunc(float * k1, float * k2)
 *    {
 *      int i;
 *      for (i = 0; i < my_nattr; i ++){
 *        if (k1[i] < k2[i]) return -1;
 *        if (k1[i] > k2[i]) return  1;
 *      }
 *      return 0;
 *    }
 * </pre><br>
 * For raw data in unsigned bytes, the following would work well:<br><pre>
 *    void  sparseKeyFunc(unsigned char * key, void * dptr)
 *    {
 *      memcpy(key, dptr, my_nattr);
 *    }
 *
 *    void   boundKeyFunc(unsigned char * key, int * data)
 *    {
 *      int i;
 *      for (i = 0; i < my_nattr; i ++)
 *        key[i] = (unsigned char)data[i];
 *    }
 *
 *    void  decompKeyFunc(int nattrib, unsigned char * key, int * cl)
 *    {
 *      int i;
 *      for (i = 0; i < my_nattr; i ++)
 *        cl[i] = key[i];
 *    }
 *
 *    int  compareKeyFunc(unsigned char * k1, unsigned char * k2)
 *    {
 *      int i;
 *      for (i = 0; i < my_nattr; i ++){
 *        if (k1[i] < k2[i]) return -1;
 *        if (k1[i] > k2[i]) return  1;
 *      }
 *      return 0;
 *    }</pre>
 *
 * <h2> Querying the mntree </h2>
 * Query your mntree with vcbMntreeQuery[ub]. The API for both ub and floating
 *  points are very similar although the back end implementation is a little
 *  different in several minor ways.<br>
 *  The input is the same for both APIs:<br>
 *    mntree - the pointer returned by the vcbMntreeBuild call<br>
 *    nattr  - number of attributes that are indexed<br>
 *    lb     - nattr number of array elements describing the lower bound of the range query<br>
 *    ub     - nattr number of array elements describing the upper bound  of the range query<br>
 * <p>
 *  The void *** result type may be a bit confusing:<br>
 *    The queried results are a bunch of pointers, pointing to each record indexed by the mntree.
 *	  MntreeQuery dump head address of each record into results.
  *    Following the pointers, the user can copy all of the items by memcpy'ing itemsz bytes.
 *    An array of those pointers is then double asterisks. In order for the allocation for an array
 *     of those pointers to be passed back out of vcbMntreeQuery[ub], we then need triple asterisks.
 *	  <br><br>
 *    The user is expected to do the following:
 *   <pre>
 *    void ** results;
 *    vcbMntreeQuery(..., & results);
 *    ...
 *    free(results);
 *   </pre>
 *	  Then, results[4] is the pointer to the 5th item (voxel).
 **/

 /* assuming unsigned byte raw data */

/** Function
 * Name: vcbMntreeBuildub
 * Version: 0.9
 * Description:
 *	Builds and returns an mntree (using unsigned byte data type) given an array of itmes.<br>
 *  See vcbMntreeBuild for a variant which uses a floating point data type.
 * Arguments:
 *  int  m:					degree of tree
 *  int  n:					depth of tree
 *  int  keysize:			the total number of bytes for the key of each item/record (i.e. voxel)
 *  int  nitems:			number of items (voxels) for the entire mntree
 *  int  itemsz:			the number of bytes of each item to be indexed in the mntree
 *  void*  items:			a pointer to a linear array of items
 *  int  nattr:				number of attributes in each item
 *  vcbmntKeyUBFunc  kfunc: compute a key given an item
 *	vcbKeyCmpUBFunc  cfunc: compares two keys for logic relation
 *	vcbKeyDecompUBFunc dfunc: decomposes a key to an array of floating points
 *	vcbBoundKeyUBFunc  bfunc: compute a key given a boundary data
 * Return: VCB_mntree; The newly built mntree structure (using unsigned byte data type)
 * Known Issues: None
 **/
VCB_mntree vcbMntreeBuildub(int m, int n, int keysize,int nitems, int itemsz, void * items,
		int nattr, vcbmntKeyUBFunc kfunc, vcbKeyCmpUBFunc cfunc, vcbKeyDecompUBFunc dfunc, vcbBoundKeyUBFunc  bfunc);


/** Function
 * Name: vcbMntreeQueryub
 * Version: 0.9
 * Description:
 *   Allows for a query of the mntree given a lower and upper bounded range.<br>
 *   Returns the number of queried items, and stores the results in "result".<br>
 *   See vcbMntreeBuild for a variant which uses a floating point data type.
 * Arguments:
 *  VCB_mntree mntree:	the pointer returned by the vcbMntreeBuild call
 *  int nattr:			number of attributes that are indexed
 *  int* lb:			nattr number of array elements describing the lower bound of the range query
 *  int* ub:			nattr number of array elements describing the upper bound of the range query
 *  void*** result:		A segment of memory allocated to contain the queried results. See the Directions
 *							for vcbmntree for details on how result should be used.
 * Return: int; Number of queried items/records (may be 0).
 * Known Issues: None
 **/
int     vcbMntreeQueryub(VCB_mntree mntree, int nattr, int * lb, int * ub, void *** result);

/* assuming floating point raw data */

/** Function
 * Name: vcbMntreeBuild
 * Version: 0.9
 * Description:
 *	Builds and returns an mntree (using floating point data type) given an array of itmes.<br>
 *  See vcbMntreeBuildub for a variant which uses unsigned byte data type.
 * Arguments:
 *  int  m:					degree of tree
 *  int  n:					depth of tree
 *  int  keysize:			the total number of bytes for the key of each item/record (i.e. voxel)
 *  int  nitems:			number of items (voxels) for the entire mntree
 *  int  itemsz:			the number of bytes of each item to be indexed in the mntree
 *  void*  items:			a pointer to a linear array of items
 *  int  nattr:				number of attributes in each item
 *  vcbmntKeyFunc  kfunc: compute a key given an item
 *	vcbKeyCmpFunc  cfunc: compares two keys for logic relation
 *	vcbKeyDecompFunc dfunc: decomposes a key to an array of floating points
 *	vcbBoundKeyFunc  bfunc: compute a key given a boundary data
 * Return: VCB_mntree; The newly built mntree structure (using floating point data type)
 * Known Issues: None
 **/
VCB_mntree vcbMntreeBuild(int m, int n, int keysize,int nitems, int itemsz, void * items,
		int nattr, vcbmntKeyFunc kfunc, vcbKeyCmpFunc cfunc, vcbKeyDecompFunc dfunc, vcbBoundKeyFunc  bfunc);

/** Function
 * Name: vcbMntreeQuery
 * Version: 0.9
 * Description:
 *   Allows for a query of the mntree given a lower and upper bounded range.<br>
 *   Returns the number of queried items, and stores the results in "result".<br>
 *   See vcbMntreeBuildub for a variant which uses unsigned byte data type.
 * Arguments:
 *  VCB_mntree mntree:	the pointer returned by the vcbMntreeBuild call
 *  int nattr:			number of attributes that are indexed
 *  int* lb:			nattr number of array elements describing the lower bound of the range query
 *  int* ub:			nattr number of array elements describing the upper bound of the range query
 *  void*** result:		A segment of memory allocated to contain the queried results. See the Directions
 *							for vcbmntree for details on how result should be used.
 * Return: int; Number of queried items (may be 0).
 * Known Issues: None
 **/
int     vcbMntreeQuery(VCB_mntree mntree, int nattr, float * lb, float * ub, void *** result);

/* common to all types */

/** Function
 * Name: vcbMntreeFree
 * Version: 0.9
 * Description:
 *   Frees the memory allocation used by an entire mntree. This function is
 *   common to mntrees of all datatypes.
 * Arguments:
 *   VCB_mntree mntree: the root pointer to the mntree
 * Return: None
 * Known Issues: None
 **/
void    vcbMntreeFree(VCB_mntree mntree);

/** Function
 * Name: vcbMntreeStat
 * Version: 0.9
 * Description: 
 *   Obtains some statistics of an mntree.
 * Arguments:
 *   VCB_mntree mntree: the root pointer to the mntree
 * Return: None
 * Known Issues: None
 **/
int     vcbMntreeStat(VCB_mntree mntree);

/** Function
 * Name: vcbMntreeLoad
 * Version: 0.9
 * Description:
 *   Loads an mntree structure from a file into memory. See vcbMntreeLoadub for
 *   the unsigned byte variant of this function.
 * Arguments:
 *  char*  filename:		filename of the mntree structure to be loaded into memory
 *  unsigned char** items:	address of a pointer of where to find the items in memory after loading in
 *  int* dims:				will be filled in with the dimensions of the original dataset used to created the mntree in the first place
 *  vcbKeyCmpFunc cfunc:	keycmp function for this tree structure
 *  vcbKeyDecompFunc dfunc:	keydecomp function for this tree structure
 *  vcbBoundKeyFunc bfunc:	boundkey function for this tree structure
 * Return: VCB_mntree; The newly created mntree structure
 * Known Issues: None
 **/
VCB_mntree vcbMntreeLoad(char *filename, unsigned char **items, int *dims, vcbKeyCmpFunc cfunc, vcbKeyDecompFunc dfunc, vcbBoundKeyFunc bfunc);

/** Function
 * Name: vcbMntreeLoadub
 * Version: 0.9
 * Description:
 *   Loads an mntree structure (unsigned bytes) from a file into memory. See vcbMntreeLoad for
 *   the float variant of this function.
 * Arguments:
 *  char*  filename:		filename of the mntree structure to be loaded into memory
 *  unsigned char** items:	address of a pointer of where to find the items in memory after loading in
 *  int* dims:				will be filled in with the dimensions of the original dataset used to created the mntree in the first place
 *  vcbKeyCmpFunc cfunc:	keycmp function for this tree structure
 *  vcbKeyDecompFunc dfunc:	keydecomp function for this tree structure
 *  vcbBoundKeyFunc bfunc:	boundkey function for this tree structure
 * Return: VCB_mntree; The newly created mntree structure
 * Known Issues: None
 **/
VCB_mntree vcbMntreeLoadub(char *filename, unsigned char **items, int *dims, vcbKeyCmpUBFunc cfunc, vcbKeyDecompUBFunc dfunc, vcbBoundKeyUBFunc bfunc);

/** Function
 * Name: vcbMntreeStore
 * Version: 0.9
 * Description: stores an mntree with the items that it points to onto disk
 *
 * Arguments:
 *  VCB_mntree mntree: The mntree that is to be stored
 *  int* dims: The number of dimensions of the original dataset. They will be stored with the tree for convenience.
 *  char* filename: The filename of the destination for the mntree structure.
 * Return: int; 0 on error, 1 on success.
 * Known Issues: None
 **/
int vcbMntreeStore(VCB_mntree mntree, int *dims, char *filename);

#ifdef __cplusplus
}  /* extern C */
#endif


#endif
