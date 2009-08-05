/***************************************************************************
 *   Copyright (C) 2004  Jian Huang                                        *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "exhash.h"

int exh_sizeof(int depth)
{
	int i, nentry;
	for (nentry = 1, i = 0; i < depth; nentry *= 2, i ++)
		;
	return nentry;
}

exhTable exhNew(int        maxnitems, 
						  int        nbits, 
						  int        itemsize,
						  exhHashFunc hfunc,
						  void *     hfparams,
						  int        hfpsize,
						  exhTHESAME  compare)
{
	int i;
	exhTable exht;

	exht = (exhTable) malloc (sizeof(struct _exh_table));
	if (exht == NULL) {
		fprintf(stderr,"error in exhNew exht memory allocation \n");
		return NULL;
	}

	exht->hfunc = hfunc;
	exht->hsame = compare;
	exht->max_nitems = maxnitems;
	exht->moved = (int *) malloc (sizeof(int)*maxnitems);
	exht->itemsize = itemsize;
	exht->idepth = nbits;

	exht->nentries = exh_sizeof(nbits);
	exht->bmask = exht->nentries - 1; /* all 1's in the lower nbits bits */

	exht->hptr = (exhEntry*) malloc (sizeof(exhEntry)*exht->nentries);

	if (exht->hptr == NULL) {
		fprintf(stderr,"error in exhNew exht->hptr memory allocation \n");
		return NULL;
	}

	for (i = 0; i < exht->nentries; exht->hptr[i] = NULL, i ++);

	exht->hfparams = malloc(hfpsize);
	memcpy(exht->hfparams, hfparams, hfpsize);

	return exht;
}

exhEntry exhNewEntry(int maxnitems, int itemsize, int key, int ilevel)
{
	exhEntry entry;

	entry = (exhEntry) malloc (sizeof(struct _exh_entry));
	entry->key = key;
	entry->nitems = 0;
	entry->itemsize = itemsize;
	entry->dptr = (char *) malloc(itemsize * maxnitems);
	entry->ilevel = ilevel;
	entry->startid = -1;

	return entry;
}


int exhSplitEntry(exhTable exth, int entryid)
/* return -1 for failure, > 0 for success */
{
	int i, key, nitems, stride, depthdiff, nshadows, shadowid;
	int localbmask;
	exhEntry src, dst;
	char * sptr;

	entryid = entryid & exth->bmask;

	src = exth->hptr[entryid];
	depthdiff = exth->idepth - src->ilevel;
	if (depthdiff < 1) return -1;

	localbmask = exh_sizeof(src->ilevel) -1;
	entryid = entryid & localbmask;

	/* figure out the stride among ids of each shadow entry */
	nshadows = exh_sizeof(depthdiff);

	for (i = 0, stride = exth->nentries; i < depthdiff; stride/=2, i ++);

	for (i = 1, shadowid = entryid + stride; i < nshadows; shadowid+= stride, i ++)
		if (exth->hptr[shadowid] != exth->hptr[entryid]) {
			return -1;
		}

	for (i = 1, shadowid = entryid + stride; i < nshadows; shadowid += stride, i ++)
		exth->hptr[shadowid] = 	exhNewEntry(exth->max_nitems,exth->itemsize, shadowid, exth->idepth);

	for (i = 0; i < src->nitems; i ++) {
		key = exth->hfunc(&src->dptr[i*src->itemsize],exth->hfparams);
		key = key & exth->bmask;
		exth->moved[i] = key;
	}

	sptr = src->dptr; 

	nitems = src->nitems;
	for (i = 0, shadowid = entryid; i < nshadows; shadowid += stride, i ++) {
		exth->hptr[shadowid]->nitems = 0;
		exth->hptr[shadowid]->ilevel = exth->idepth;
	}

	for (i = 0; i < nitems; i ++) {
		dst = exth->hptr[exth->moved[i]];
		if (&dst->dptr[dst->nitems*dst->itemsize] != sptr)
			memcpy(&dst->dptr[dst->nitems*dst->itemsize],sptr,src->itemsize);
		dst->nitems ++;
		sptr += src->itemsize;
	}

	return 1;
}

int exhDblTable(exhTable exth)
/* double the extendible hash table */
/* return -1 for failure, >0 for success */
{
	int i, nentry;
	exhEntry * tmphptr;

	nentry = exh_sizeof(exth->idepth+1);
	assert(nentry == exth->nentries * 2);

	tmphptr = (exhEntry*) malloc (sizeof(exhEntry)*nentry);
	if (tmphptr == NULL) {
		fprintf(stderr,"exhDblTable cannot allocate memory\n");
		return -1;
	}

	exth->idepth += 1;
	exth->bmask = nentry - 1; /* all 1's in the lower nbits bits */

	for (i = 0; i < exth->nentries; tmphptr[i] = exth->hptr[i], i ++);
	free(exth->hptr);
	exth->hptr = tmphptr;

	for (i = 0; i < exth->nentries; i ++)
		exth->hptr[i+exth->nentries]= exth->hptr[i];

	exth->nentries = nentry;
	return 1;
}

exhEntry exhInsertLoc(exhTable exth, int key)
/* return NULL for failure, otherwise the entry this new data should be added to */
{
	exhEntry  * entry;
	entry = & exth->hptr[key & exth->bmask];

	/* entry has not been created -> new_ExhEntry */
	if ((*entry) == NULL) {
		(*entry) = exhNewEntry(exth->max_nitems,exth->itemsize, key, exth->idepth);
		return (*entry);
	}

	/* entry exist and is not full -> just add */
	if ((*entry)->nitems < exth->max_nitems)
		return (*entry);

	if ((*entry)->ilevel < exth->idepth) {
	    /* entry is full, but has shadows -> split with all its shadows */
		if (exhSplitEntry(exth, key) < 0)
			return NULL;

		entry = &exth->hptr[key & exth->bmask];
		if ((*entry)->nitems < exth->max_nitems)
			return (*entry);
		else
			return exhInsertLoc(exth, key);
	}
	else {
		/* entry is full and no shadow -> double dictionary */
		if (exhDblTable(exth) < 0)
			return NULL;
		else
			return exhInsertLoc(exth, key);
	}
}

int exhAddItem(exhTable exth, void * data)
{
	int key;
	exhEntry entry;
	key = exth->hfunc(data, exth->hfparams);

	if ((entry = exhInsertLoc(exth, key))==NULL)
		return -1;
	
	/* 
	 * entry is not full -> add 
	 * exhInsertLoc should guarantee that this is ture,
	 * just doing a sanity check here anyway
	 */
	if (entry->nitems >= exth->max_nitems)
		return -1;

	memcpy(&entry->dptr[entry->nitems*entry->itemsize],data,entry->itemsize);
	entry->nitems ++;
	return 1;
}

int exhItemID(exhTable exth, void * data)
{
	int i, key;
	exhEntry entry;
	key = exth->hfunc(data, exth->hfparams);
	entry = exth->hptr[key & exth->bmask];

	for (i = 0; i < entry->nitems; i ++)
		if ((exth->hsame(&entry->dptr[i*entry->itemsize],data)) > 0)
			return (entry->startid + i);

	return -1;
}


int exhQueryItem(exhTable exth, void * data)
{
	int i, key;
	exhEntry entry;
	key = exth->hfunc(data, exth->hfparams);
	entry = exth->hptr[key & exth->bmask];

	if (entry == NULL) return -1;

	for (i = 0; i < entry->nitems; i ++)
		if ((exth->hsame(&entry->dptr[i*entry->itemsize],data)) > 0)
			return 1;

	return -1;
}


int  exhFinalize(exhTable exth, exhStat * stat)
{
	int i, nitems;
	exhEntry entry;

	stat->idepth = exth->idepth;
	stat->nentries = exth->nentries;
	stat->max_nitems = exth->max_nitems;
	stat->n_empty = 0;
	stat->n_nonem = 0;
	stat->n_nonem_shallow = 0;
	stat->min_ilevel = 32768;
	stat->n_shadows = 0;
	stat->avg_items = 0.f;
	stat->totalbytes = 0;
	stat->totaloverhead = 0;

	for (i = 0, nitems = 0; i < exth->nentries; i ++) {
		entry = exth->hptr[i];
		if (entry == NULL) {
			stat->n_empty++;
			continue;
		}
		if (entry->startid >= 0) {
			assert(entry->startid <= nitems);
			stat->n_shadows ++;
			continue;
		}

		stat->n_nonem ++;
		if (entry->ilevel < exth->idepth) stat->n_nonem_shallow ++;
		if (entry->ilevel < stat->min_ilevel) stat->min_ilevel = entry->ilevel;

		stat->totalbytes += entry->nitems * entry->itemsize;
		stat->totaloverhead += sizeof(struct _exh_entry);
		entry->startid = nitems;
		nitems += entry->nitems;
	}

	stat->totaloverhead += sizeof(exhEntry) * stat->nentries;
	stat->totaloverhead += sizeof(struct _exh_table);
	stat->totaloverhead += sizeof(int) * stat->max_nitems;
	stat->avg_items = ((float)nitems)/stat->n_nonem;

	return nitems;
}

void exhPrintStat(FILE * fp, exhStat * stat)
{
	fprintf(fp, "vrexhstat: VR extendible hashing statistics \n");
	fprintf(fp, "vrexhstat: table depth = %d, with %d entries locations\n",stat->idepth,stat->nentries);
	fprintf(fp, "vrexhstat: among these entries, %d void, %d non-empty, %d shadow\n",
		stat->n_empty, stat->n_nonem, stat->n_shadows);
	fprintf(fp, "vrexhstat: minimal ilevel for non-empty entries = %d\n",stat->min_ilevel);
	fprintf(fp, "vrexhstat: %d of the %d non-empty entries have ilevel < table depth\n",stat->n_nonem_shallow,stat->n_nonem);
	fprintf(fp, "vrexhstat: in the non-empty entries, avg number of items is %f\n", stat->avg_items);
	fprintf(fp, "vrexhstat: the maximum number of items per entry is %d\n",stat->max_nitems);
	fprintf(fp, "vrexhstat: total number of payload bytes: %d\n",stat->totalbytes);
	fprintf(fp, "vrexhstat: total number of overhead bytes: %d\n",stat->totaloverhead);
}

void exhSerialize(exhTable exth, void * data)
{
	int i, nbytes, cid;
	exhEntry entry;
	char * bstream = (char *) data;

	for (i = 0, cid = 0, nbytes = 0; i < exth->nentries; i ++) {
		entry = exth->hptr[i];
		if (entry == NULL) continue;
		if (entry->startid < cid) continue;
		cid = entry->startid;
		memcpy(&bstream[nbytes],entry->dptr,entry->nitems*entry->itemsize);
		nbytes += entry->nitems*entry->itemsize;
	}
}


void exhFreeEntry(exhEntry entry)
{
	free(entry->dptr);
	free(entry);
}

void exhFree(exhTable exth)
{
	int i, j, stride, depthdiff, nshadows, shadowid;
	exhEntry entry;

	for (i = 0; i < exth->nentries; i ++) {
		/* figure out the shadows */
		entry = exth->hptr[i];
		if (entry == NULL) continue;

		depthdiff = exth->idepth - entry->ilevel;
		nshadows = exh_sizeof(depthdiff);
		if (nshadows > 1) {
			for (j = 0, stride = exth->nentries; j < depthdiff; stride/=2, j ++);
			for (j = 1, shadowid = i + stride; j < nshadows; shadowid += stride, j ++)
				exth->hptr[shadowid] = NULL;
		}

		exhFreeEntry(entry);
		exth->hptr[i] = NULL;
	}

	free(exth->moved);
	free(exth->hptr);
	free(exth);
}

