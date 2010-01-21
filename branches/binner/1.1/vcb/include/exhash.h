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

#ifndef _EXHASH_H_
#define _EXHASH_H_

/** Type
 * Name: exhHashFunc
 * Version: 0.9
 * Description:
 * typedefined&#58; int (*exhHashFunc) (void * data, void * params) <br>
 * This is used for user-specified hash functions used in the exhash API.
 **/
typedef int (*exhHashFunc) (void * data, void * params);

/** Type
 * Name: exhTHESAME
 * Version: 0.9
 * Description:
 * typedefined&#58; int (*exhTHESAME) (void * src, void * dst) <br>
 * This is used for user-specified comparison functions used in the exhash API.
 * <p>
 * It should compare any two items to be hashed to determine whether they are the same. If 
 *    they are the same, compare must return 1, otherwise -1.
 **/
typedef int (*exhTHESAME) (void * src, void * dst);


/** Struct
 * Name: exhEntry
 * Version: 0.9
 * Description:
 *  typedefined&#58; * exhEntry <br>
 *  exhEntry is a structure that contains the information for an entry in a exhTable.
 * Elements:
 *  int key:
 *  char* dptr: Linear byte sequence to store data
 *  int nitems: Number of items in this hash table entry
 *  int itemsize: Size of entry.
 *  int ilevel: Which level is this entry.
 *  int startid: This is an integer id generated for the first item in the entry 
 *   this id is unique throughout the whole dataset, the id's of 
 *   all other items in the entry can be obtained by incr'ing startid 
 **/
typedef struct _exh_entry {

	int key;
	char * dptr;         /* Linear byte sequence to store data */
	int nitems;          /* Number of items in this hash table entry */
	int itemsize;
	int ilevel;          /* Which level is this entry */

	/*
	 * This is an integer id generated for the first item in the entry 
     * this id is unique throughout the whole dataset, the id's of 
     * all other items in the entry can be obtained by incr'ing startid 
	 */
	int startid;
} * exhEntry;

/** Struct
 * Name: exhTable
 * Version: 0.91
 * Description:
 *  typedefined&#58; * exhTable <br>
 *  exhTable is the structure that contains the extendible hash table.
 * Elements:
 *  int bmask: A mask with the lowest nbits bits set to 1, 0 everywhere else
 *  int max_nitems: Maximum number of items in each hash table entry
 *  int itemsize: Size of items.
 *  int nentries: Current number of entries in the whole table
 *  int idepth: How deep is this extendible hash, i.e. # bits in use by hash index
 *  exhEntry* hptr: Head ptr to the begining of table 
 *  int* moved: An internal array used by splitEntry
 *  exhHashFunc  hfunc: Hash function to use
 *  void*  hfparams: Arguments for hash function.
 *  exhTHESAME hsame: a function pointer to a compare function that compares
 *    any two items to be hashed to  determine whether they are the same. If 
 *    they are the same, compare must return 1, otherwise -1.
 **/
typedef struct _exh_table {
	
	int bmask;           /* A mask with the lowest nbits bits set to 1, 0 everywhere else */
	int max_nitems;      /* Maximum number of items in each hash table entry */
	int itemsize;
	int nentries;        /* Current number of entries in the whole table */
	int idepth;          /* How deep is this extendible hash, i.e. # bits in use by hash index */
	exhEntry * hptr;  /* Head ptr to the begining of table */
	int * moved;         /* An internal array used by splitEntry */

	exhHashFunc  hfunc;
	void *      hfparams;
	exhTHESAME   hsame;
} * exhTable;


/** Struct
 * Name: exhStat
 * Version: 0.9
 * Description:
 *  typedefined&#58; * exhStat <br>
 *  exhStat is a structure that contains statistics for a exhTable.
 * Elements:
 *  int idepth:          Depth of extendible hash table 
 *  int nentries:        Number of entries in the table 
 *  int n_empty:         Number of empty entries 
 *  int n_nonem:         Number of non-emtry entries 
 *  int n_nonem_shallow: Number of non-emtry entries on a shallower level than idepth
 *  int min_ilevel:      Lowest value of ilevel among non-empty entries 
 *  int n_shadows:       Number of non-empty shadows 
 *  int max_nitems:      Maximum number of items per entry 
 *  float avg_items:     Average number of items in each non-empty entries 
 *  int totalbytes:      Total number of payload bytes of the extendible hashing 
 *  int totaloverhead:   Total number of overhead bytes of the extendible hashing 
 **/
typedef struct _exh_stat {
	int idepth;          /* Depth of extendible hash table */
	int nentries;        /* Number of entries in the table */
	int n_empty;         /* Number of empty entries */
	int n_nonem;         /* Number of non-emtry entries */
	int n_nonem_shallow; /* Number of non-emtry entries on a shallower level than idepth*/
	int min_ilevel;      /* Lowest value of ilevel among non-empty entries */
	int n_shadows;       /* Number of non-empty shadows */
	int max_nitems;      /* Maximum number of items per entry */
	float avg_items;     /* Average number of items in each non-empty entries */
	int totalbytes;      /* Total number of payload bytes of the extendible hashing */
	int totaloverhead;   /* Total number of overhead bytes of the extendible hashing */
} exhStat;

#ifdef __cplusplus
extern "C" {
#endif



/* extendible hashing API */

/** Function
 * Name: exhNew
 * Version: 0.9
 * Description:
 *  exhNew constructs an extensible hashing data structure.
 * Arguments:
 *  int maxnitems: the maximum number of items per hash bucket, common values
 *    to use are < 25.
 *  int nbits: the initial number of bits within hash index to use, 2^nbits 
 *    is the initial size of the extensible hash table.
 *  int itemsize: the number of bytes in each item to be hashed.
 *  exhHashFunc hfunc: a function pointer to a hash function.
 *  void* hfparams: a pointer to an array of constant parameters for use in 
 *    hfunc. hunc must return an integer value to be used as the hash index.
 *  int hfpsize: the number of bytes in hfparams.
 *  exhTHESAME compare: a function pointer to a compare function that compares
 *    any two items to be hashed to  determine whether they are the same. If 
 *    they are the same, compare must return 1, otherwise -1.
 * Return: exhTable; new extensible hashing data structure
 * Known Issues: None
 **/
exhTable exhNew(int maxnitems, int nbits, int itemsize, 
						  exhHashFunc hfunc, void * hfuncparams, int hfpsize, 
						  exhTHESAME compare);

/** Function
 * Name: exhFree
 * Version: 0.9
 * Description:
 *  exhFree jettisons a whole extensible hash table.
 * Arguments:
 *  exhTable exth: Extensible hash table to free.
 * Return: None
 * Known Issues: None
 **/
void        exhFree(exhTable exth);

/** Function
 * Name: exhQueryItem
 * Version: 0.91
 * Description:
 *  exhQueryItem determines whether an item already exists in the 
 *  extensible hash table.
 * Arguments:
 *  exhTable exth: The exhTable to be searched for the existance of "data".
 *  void* data: The pointer to the item to query for existance in the hash table.
 * Return: int; returns 0 if the item exists, -1 if the item does not.
 * Known Issues: None
 **/       
int         exhQueryItem(exhTable exth, void * data);

/** Function
 * Name: exhAddItem
 * Version: 0.91
 * Description:
 *  exhAddItem adds the item pointed to by "data" to the exhTable "exth".
 *  <p>
 * Important Note&#58; exhAddItem assumes that no duplicates exist in the table. 
 *  Always call exhQueryItem before exhAddItem in your program.
 * Arguments:
 *  exhTable exth: The exhTable that will be added to.
 *  void* data: The pointer to the item that will be added to the hash table.
 * Return: int; returns 1 on a successful add, -1 on an unsuccessful add.
 * Known Issues: None
 **/   
int         exhAddItem(exhTable exth, void * data);

/** Function
 * Name: exhItemID
 * Version: 0.9
 * Description:
 *  exhItemID returns a unique integer id for the item specified by "data" 
 *  in the exhTable "exth", assuming that item exits in "exth". The id 
 *  returned corresponds to the location of the item as in exhSerialize. 
 *  Important Note: This function is only valid after a exhTable has been 
 *  finalized with exhFinalize.
 * Arguments:
 *  exhTable exth: Current hash table.
 *  void* data: The table item whose ID will be returned.
 * Return: int; returns -1 if "data" is not in "exth"
 * Known Issues: None
 **/ 
int         exhItemID(exhTable exth, void * data);


/** Function
 * Name: exhFinalize
 * Version: 0.9
 * Description:
 *  exhFinalize finalizes "exth" and assignes each item bashed in "exth" a
 *  unique integer id. This id corresponds to the location of each item in 
 *  exhSerialize.  exhFinalize also generates an exhStat struct 
 *  "stat" containing statistics of the extendible hashing table "exth".  The
 *  memory storage for "stat" is assumed to be allocated.
 * Arguments:
 *  exhTable exth: Current hash table.
 *  exhStat* stat: Memory for storage of table statistics.
 * Return: int; returns the total number of unique items in "exth".
 * Known Issues: None
 **/ 
int         exhFinalize(exhTable exth, exhStat * stat);


/** Function
 * Name: exhSerialize
 * Version: 0.9
 * Description:
 *  exhSerialize builds a linear list of "exth" pointed to by "data". In 
 *  this list, unique items are stored one after another and can be indexed by 
 *  data[id * itemsize]. Here, id is what exhItemID returns.
 *  <p>
 *  exhSerialize always assumes that the memory pointed to by data is 
 *  valid and has at least "nitems" * "itemsize" bytes, where "nitems" is what 
 *  exhFinalize returns.
 *  <p>
 *  Important Note&#58 exhSerialize is only valid when called after
 *  exhFinalize.
 * Arguments:
 *  exhTable exth: Current hash table.
 *  void* data: Linear list for table items.
 * Return: None
 * Known Issues: None
 **/ 
void        exhSerialize(exhTable exth, void * data);

/** Function
 * Name: exhPrintStat
 * Version: 0.9
 * Description:
 *  exhPrintStat prints to fp (fp can be stdout, stderr or any other
 *  already opened regular file) the information contained in "stat" into a
 *  standard format.
 * Arguments:
 *  FILE* fp: File that statistics will be printed to.
 *  exhStat* stat: The statistics from an extensible hash table.
 * Return: None
 * Known Issues: None
 **/ 
void        exhPrintStat(FILE * fp, exhStat * stat);

/* extendible hashing utility functions, mostly for internal use */
int         exh_sizeof(int depth);
exhEntry exhNewEntry(int maxnitems, int itemsize, int key, int ilevel);
void        exhFreeEntry(exhEntry entry);
int         exhSplitEntry(exhTable exth, int entryid);
int         exhDblTable(exhTable exth);
exhEntry exhInsertLoc(exhTable exth, int key);

#ifdef __cplusplus
}  /* extern C */
#endif


#endif
