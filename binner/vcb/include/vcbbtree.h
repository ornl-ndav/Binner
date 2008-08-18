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

#include "vcbmntree.h"

#ifndef _VCB_BTREE_H
#define _VCB_BTREE_H

#ifdef __cplusplus
extern "C" {
#endif

/* btree API */

/** Directions for vcbbtree
 * <h2>Description:</h2>
 * The btree API is provided as a convenience utility to use mntree
 * based on floating point raw data types.
 * <p>
 * The three API functions:<br>
 *   vcbBuildBtree <br>
 *   vcbFreeBtree <br>
 *   vcbQueryBtree <br>
 * are all that is required for its use. In most cases, users are strongly
 * recommended to use this API directly.
 * <p>
 * See t_btree_main, t_cluster_main and t_amcube_main_f for examples of btree usage.
 * <h2>Usage example as seen in t_amcube_main_f:</h2><pre>
 * #include "vcbbtree.h"
 * ...
 * 
 * /*global variables*\/
 * VCB_mntree btree;
 * unsigned char * voxels;
 * int sz[3];
 * int changed;
 * 
 * void load_data(void){
 *	char * dataname = "datasets/vox/dataset2";
 *	if (load_vox(dataname, sz, &voxels) < 0) {
 *		fprintf(stderr, "load_data: error loading datafile\n");
 *		exit(-1);
 *	}
 *	pool = (float *) malloc((sz[0]-1)*(sz[1]-1)*(sz[2]-1)*3*sizeof(float));
 *	/*get poolcnt and store data from voxels in pool*\/
 *	...
 *	btree = (VCB_mntree)vcbBuildBtree(poolcnt, nattribs, pool, 32, 3);
 * }
 *  
 * void getmesh(void){ /*called if changed == 1 by display()*\/
 *	int * q_ids;
 *	short * cellids;
 *	/*assign lb and ub*\/
 *	ncells = vcbQueryBtree(btree, lb, ub, &q_ids);
 *	cellids = (short *) malloc(ncells * 3 * sizeof(short));
 *	...
 *	for (i = 0; i < ncells; i ++) {
 *		cellid = q_ids[i];
 *		...
 *	}
 *	/*call vcbMarchingCubeList(...)*\/
 *	free(cellids);
 *	free(q_ids);
 * }
 * 
 * void cleanup(void){ /*called at exit*\/
 *	vcbFreeBtree(btree);
 *	...
 * }
 * 
 * int main( int argc, char** argv){
 *	changed = 1;
 *	load_data();
 *	atexit(cleanup);
 *	/* INITIALIZE GLUT AND START MAIN LOOP *\/
 * }</pre>
 **/

/** Function
 * Name: vcbBuildBtree
 * Version: vcbmntree.h
 * Description: This API simplifies the use of an mntree based on floating
 *   point raw data types. The btree API assumes that in an independent
 *   memory address space, there can be only one btree in existence.<BR>
 *   See the Directions for vcbmntree for more information on mntrees.
 * Arguments:
 *    int nrecords: The total number of records to index.
 *    int nattribs: The total number of attributes in each record.
 *    void* records:  Records must be in the following format&#58;
 *		<p>
 *       There are nrecords, stored in a linear array pointed to by records.<BR>
 *       Each record contains "nattribs" floating point attributes AND
 *             one integer id of the record
 *		<p>
 *       For instance, assuming there are two attributes&#58;<BR>
 *             rec0_a0, rec0_a1, rec0_i, <br>
 *			   rec1_a0, rec1_a1, rec1_i, <br>
 *             rec2_a0, rec2_a1, rec2_i, ...
 *    int m: The degree of the mntree to be built.
 *    int n: The depth on the mntree to be built.
 * Return: void*; This function returns the btree.
 * Known Issues: Differences in attributes less than 1e-4f are too small for
 *    vr_btree to correctly retrieve using vcbQueryBtree, so please avoid
 *    this situation.
 **/
void * vcbBuildBtree(int nrecords, int nattribs, void * records, int m, int n);

/** Function
 * Name: vcbFreeBtree
 * Version: vcbmntree.h
 * Description: Call this to release memory during cleanup before
 *   calling exit in the main program.
 * Arguments: None
 * Return: None
 * Known Issues: None
 **/
void   vcbFreeBtree(VCB_mntree tree);

/** Function
 * Name: vcbQueryBtree
 * Version: vcbmntree.h
 * Description: Performs a range query on a constructed btree.
 * Arguments:
 *  float* lb: The lower bound.
 *  float* ub: The upper bound.
 *  int** record_ids: Output. record_ids is a memory segement allocated
 *   within the API. It contains the id (ie. rec_i's) of all records
 *   meeting the range requirement.<BR>
 *   Note, the user is expected to free the memory segment allocated for
 *   record_ids to avoid memory leaks. btree only performs the malloc's
 *   and not the free calls.
 * Return: int; The number of records meeting the requirement.
 * Known Issues: None
 **/
int    vcbQueryBtree(VCB_mntree tree, float * lb, float * ub, int ** record_ids);

#ifdef __cplusplus
}  /* extern C */
#endif


#endif
