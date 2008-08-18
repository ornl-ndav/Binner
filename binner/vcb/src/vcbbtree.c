#include <stdlib.h>
#include <string.h>
#include "vcbmntree.h"

static int btreehelper;

static void  sparseKeyFunc(float * key, void * dptr)
{
	memcpy(key, dptr, btreehelper*sizeof(float));
}


static void   boundKeyFunc(float * key, float * data)
{
	int i;
	for (i = 0; i < btreehelper; i ++)
		key[i] = data[i];
}

static void  decompKeyFunc(int nattrib, float * key, float * cl)
{
	int i;
	for (i = 0; i < btreehelper; i ++)
		cl[i] = key[i];
}

static int  compareKeyFunc(float * k1, float * k2)
{
	int i;
	for (i = 0; i < btreehelper; i ++){
		if (k1[i] < k2[i]) return -1;
		if (k1[i] > k2[i]) return  1;
	}
	return 0;
}

void * vcbBuildBtree(int nrecords, int nattribs, void * records, int m, int n)
{
	VCB_mntree tree;
	btreehelper = nattribs;

	tree = vcbMntreeBuild(m, n, nattribs*sizeof(float), nrecords, (nattribs+1)*sizeof(float), records, nattribs, sparseKeyFunc, compareKeyFunc, decompKeyFunc, boundKeyFunc);

	return tree;
}

void  vcbFreeBtree(VCB_mntree tree)
{
	vcbMntreeFree(tree);
}

int   vcbQueryBtree(VCB_mntree tree, float * lb, float * ub, int ** record_ids)
{
	int nrecords, i;
	char ** recptr;

	btreehelper = tree->nattr;
	nrecords = vcbMntreeQuery(tree, tree->nattr, lb, ub, (void ***)&recptr);

	(*record_ids) = (int *) malloc (nrecords*sizeof(int));
	for (i = 0; i < nrecords; i ++)
		memcpy((*record_ids)+i, (*(recptr + i)) + tree->keysz,sizeof(int));

	free(recptr);

	return nrecords;
}
