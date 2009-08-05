#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "dmem.h"

static Dmlist node;
static int    cloc;
static int    entrysz;

Dmlist new_dmlist()
{
	Dmlist ptr;

	ptr         = (Dmlist) malloc (sizeof(struct dmemstruct));
	ptr->d      = (char *) malloc (sizeof(char)*DMEMALLOCSIZE);
	ptr->nbytes = 0;
	ptr->link   = NULL;

	return ptr;
}

void   free_dmlist(Dmlist dm)
{
	Dmlist fptr, bptr;
	for (fptr = dm; fptr != NULL; bptr = fptr, fptr = fptr->link, free(bptr))
		free(fptr->d);
}

void   dml_append(Dmlist dm, void * data, int nbytes)
{
	Dmlist fptr, bptr;
	for (fptr = dm; fptr != NULL; bptr = fptr, fptr = fptr->link) {
		if (fptr->nbytes + nbytes <= DMEMALLOCSIZE)
			break;
	}

	if (fptr == NULL) { /* we are all the way to the very end now */
		bptr->link = new_dmlist();
		fptr = bptr->link;
	}

	memcpy(&fptr->d[fptr->nbytes], data, nbytes);
	fptr->nbytes += nbytes;
}

void  dml_straverse(Dmlist dm, int ensz)
{
	node = dm;
	entrysz = ensz;
	cloc = 0;
}

Dmlist dml_next(void * data)
{
	for (;(node!=NULL) && (cloc + entrysz > node->nbytes); node = node->link, cloc = 0);

	if (node == NULL)
		return NULL;
	
	memcpy(data,&node->d[cloc],entrysz);
	cloc += entrysz;

	return node;
}

void  dml_etraverse()
{
	node = NULL;
	cloc = 0;
	entrysz = 0;
}

int   dml_totalbytes(Dmlist dm)
{
	int totalbytes = 0;
	for (; dm!=NULL; totalbytes+=dm->nbytes, dm = dm->link);
	return totalbytes;
}

void   dml_cpyall(Dmlist dm, void * dout)
/* 
 * copy dml memory to contiguous memory, release all
 * dml memory just like dml_free is called 
 */
{
	Dmlist fptr, bptr;
	int nbytes;
	char * cdout = (char *) dout;

	fptr = dm;
	nbytes = 0;
	while (fptr != NULL) {
		memcpy(&cdout[nbytes], fptr->d, fptr->nbytes);
		nbytes += fptr->nbytes;
		free(fptr->d);
		bptr = fptr;
		fptr = fptr->link;
		free(bptr);
	}
}
