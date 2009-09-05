/****************************************************************************
 *   Copyright (C) 2004  by Jian Huang										*
 *	 huangj@cs.utk.edu														*
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

#ifndef _VCB_DMEM_H_
#define _VCB_DMEM_H_

/** Define
 * Name: DMEMALLOCSIZE
 * Version: vcbmntree.h
 * Description: This constant is the number of bytes in each
 *  Dmlist node. It is currently set to&#58;<br>
 *  1024 * 1024 * 4 = 4Mb
 **/
#ifndef DMEMALLOCSIZE
#define DMEMALLOCSIZE 1024*1024*4
#endif

/** Struct
 * Name: Dmlist 
 * Version: vcbmntree.h
 * Description:
 *  This is a dynamic array library implemented as a linked list
 * Elements:
 *  char* d: head addr of malloc'ed data
 *  int nbytes: number of currently used bytes
 *  struct dmemstruct* link: pointer to the next dmemstruct
 **/
typedef struct dmemstruct {
	char * d;                 /* head addr of malloc'ed data */
	int    nbytes;            /* number of currently used bytes */
	struct dmemstruct * link; /* pointer to the next dmemstruct */
} * Dmlist;

/** Define
 * Name: dm_traverse
 * Version: vcbmntree.h
 * Description:
 *  This macro can be used in place of a for loop to traverse a Dmlist.
 *  Pass it a temporary Dmlist pointer and a Dmlist.
 **/
#ifndef dm_traverse
#define dm_traverse(ptr, list) for (ptr = list; ptr != NULL; ptr = ptr->link)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Directions for dmem
 * <h2>Description:</h2>
 * dmem is a dynamic memory allocation library intended for
 * a specific use.
 *  <p>
 * For instance, if we have an algorithm that incrementally generates a
 *   previously unknown number of primitives of the same
 *   size, it would be desirable to have an API to hide the details
 *   of managing this dynamic set of memory.
 *
 * <h2>Technique background:</h2>
 *   dmem is basically a singly linked list with each
 *   node referencing DMEMALLOCSIZE bytes of memory
 *   allocation. DMEMALLOCSIZE is currently set to 4MB.
 *  
 *  <h2>Expected usage:</h2><pre>
 *	Dmlist dm;
 *	int    ntotalbytes;
 *	dm = new_dmlist();
 *
 *	while(generating primitives one by one){
 *		dml_append(dm, pointer to the new primitive, sizeof(primitive));
 *	}</pre>
 *
 *	The simplest way to proceed is to first call:<pre>
 *	ntotalbytes = dml_totalbytes(dm); </pre>
 *  to obtain the total number of bytes in this dynamically managed memory.
 *  <p>
 *  After the user allocates a contiguous amount of memory in "dout": <pre>
 *	dml_cpyall(dm, dout);</pre>
 *  This will copy all data contained in dm to dout; dm is deallocated
 *  during the process.
 *	<p>
 *  free_dmlist(dm) will also deallocate dm. However, dml_cpyall essentially
 *  does everything free_dmlist performs, so do not call
 *  free_dmlist after calling dml_cpyall.
 *  <p>
 *  This library is more efficient than, and replaces, a previous Darray
 *   c++ library in VR.
 **/

/** Function
 * Name: new_dmlist
 * Version: vcbmntree.h
 * Description:
 *  new_dmlist creates and returns a new Dmlist struct.
 * Arguments: None
 * Return: Dmlist; new Dmlist struct
 * Known Issues: None
 **/
Dmlist new_dmlist();

/** Function
 * Name: free_dmlist
 * Version: vcbmntree.h
 * Description:
 *  free_dmlist removes the Dmlist "dm" from memory.<br>
 *  NOTE&#58; Do not call free_dmlist after dml_cpyall as it will
 *  have already been freed by dml_cpyall.
 * Arguments:
 *  Dmlist dm: The Dmlist to be freed from memory.
 * Return: None
 * Known Issues: None
 **/
void   free_dmlist(Dmlist dm);

/** Function
 * Name: dml_append
 * Version: vcbmntree.h
 * Description:
 *  dml_append adds a new node "data" of size "nbytes" onto the Dmlist "dm".
 * Arguments:
 *  Dmlist dm: The Dmlist to add to.
 *  void* data: The new node to add to "dm"
 *  int nbytes: The size of data in bytes.
 * Return: dm now has a copy of "data", returned is the head address of that copy
 * Known Issues: None
 **/
void *   dml_append(Dmlist dm, void * data, int nbytes);

/** Function
 * Name: dml_totalbytes
 * Version: vcbmntree.h
 * Description:
 *  dml_totalbytes returns the total number of bytes of memory currently being used by
 *  the Dmlist "dm".
 * Arguments:
 *  Dmlist dm: The Dmlist to currently being used.
 * Return: int; Number of bytes
 * Known Issues: None
 **/
int    dml_totalbytes(Dmlist dm);

/** Function
 * Name: dml_cpyall
 * Version: vcbmntree.h
 * Description:
 *  dml_cpyall copies the entire contents of the Dmlist "dm" into the static memory
 *  location "dout". The correct number of bytes for "dout" is assumed to have
 *  previously been allocated.<br>
 *  NOTE&#58; dml_cpyall frees "dm" using free_dmlist
 * Arguments:
 *  Dmlist dm: The Dmlist to currently being used.
 *  void* dout: Pointer to static memory location that will contain all data
 *   from "dm". Memory must be allocated in advance.
 * Return: None
 * Known Issues: None
 **/
void   dml_cpyall(Dmlist dm, void * dout);

/* internal API. don't expose */
void   dml_straverse(Dmlist dm, int ensz);
Dmlist dml_next(void * data);
void   dml_etraverse();

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
