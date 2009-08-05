#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "math.h"
#include "dmem.h"
#include "vcbmntree.h"
#include "vcbdebug.h"

static VCB_mntree qsorthelp = NULL;

#ifndef VCB_MNT_COPYKEY
#define VCB_MNT_COPYKEY(a, i, sz, k) memcpy((a) + (i)*sz, k, sz)
#endif
#ifndef VCB_MNT_ZEROKEY
#define VCB_MNT_ZEROKEY(a, i, sz) memset((a)+(i)*sz, 0, sz)
#endif

static int compare(const void *arg1, const void *arg2 )
/* internal function for use with qsort */
{
	float * item1, * item2;
	item1 = (float *) arg1;
	item2 = (float *) arg2;

	return qsorthelp->mntcfunc(item1, item2);
}


static int  outbound_bracket(int numvals, float * c, float * b1, float *b2)
{ /* return >1 -> c is out of the bounds of b1-b2, 0 -> otherwise */

	int i, ret;

	for (i = 0, ret = 0; i < numvals; i ++)
		ret += ((c[i] < b1[i]) | (c[i] > b2[i]));

	return ret;
}

static int  inboundincr_bracket(int numvals, float *c, float * b1, float * b2)
{ /* return :
   *    -1 -> after incr, c is still outbound
   *     0 -> c is inbound of b1-b2
   *     1 -> c is outbound, after incr, it's inbound
   */
	int i, j, carry;

	if (outbound_bracket(numvals, c, b1, b2)) {
		for (i = numvals - 1, carry = 0; i >= 0; i --)  {

			c[i] = c[i] + carry*1e-4f;
			carry = 0;

			if (c[i] < b1[i]) {
				c[i] = b1[i];
				for (j = i; j < numvals; j ++)
					c[j] = b1[j];
			}

			if (c[i] > b2[i]) {
				c[i] = b1[i];
				carry = 1;
			}
		}
		if (carry == 0)
			return 1;
		else
			return -1;
	}
	return 0;
}

static int  lessequal_bracket(int numvals, int * b1, int * b2)
{ /* b1 < b2 --> return 1, otherwise --> return 0 */
	int i;
	for (i = 0; i < numvals; i ++) {
		if (b1[i] < b2[i]) return 1;
		if (b1[i] > b2[i]) return 0;
	}

	return 1;
}

static int  less_bracket(int numvals, int * b1, int * b2)
{ /* b1 < b2 --> return 1, otherwise --> return 0 */
	int i;
	for (i = 0; i < numvals; i ++) {
		if (b1[i] < b2[i]) return 1;
		if (b1[i] > b2[i]) return 0;
	}

	return 0;
}

static int _incr_indx(VCB_mntree tree, int * clbounds, int ndims, int * lbounds, int * ubounds)
/* internal function
 * return value: 0 for normal incr
 *               1 for overflow
 */
{
	int i;

	if (lessequal_bracket(ndims, clbounds, ubounds)) {

		i = ndims - 1;
		clbounds[i] ++;
		while ((clbounds[i] > (tree->degree-1)) && (i >= 0)){
			clbounds[i] = 0;
			i --;
			if (i >= 0)
				clbounds[i] ++;
			else
				break;
		}
		return (i >=0) ? (1-lessequal_bracket(ndims, clbounds, ubounds)) : 1;
	}
	else
		return 1;
}


static void  dup_bracket(int numvals, int * b1, int * b2)
{ /* duplicate values in b2 to b1 */
	int i;
	for (i = 0; i < numvals; b1[i] = b2[i], i ++);
}


static int _binsearch(VCB_mntree tree, int numvals, unsigned char * kptr, float * newkey)
{
	int l, r;
	unsigned char * vals;

	vals = kptr-tree->keysz;
	for (l = 0, r = numvals; (r-l)>1; ) {
		if (tree->mntcfunc((float*)(vals + tree->keysz*((l+r)/2)), newkey) < 0)
			l = (l+r)/2;
		else
			r = (l+r)/2;
	}

	return (r-1);
}


static void descend_mntree(VCB_mntree tree, float * newkey, int * bracket)
{
	int i, loc;
	VCB_mntd c = tree->root;

	for (i = 0; i < tree->numlevels - 1; i ++) {
		loc = _binsearch(tree, tree->degree, c->keys, newkey);
		bracket[i] = loc;
		c = c->lower[loc];
	}
	loc = _binsearch(tree, tree->degree, c->keys, newkey);
	bracket[i] = loc;
}


static void _grow_mntree(VCB_mntd * c, int m, int n, int keysize)
{
	int i;

	if ((*c) == NULL) { /* this is the root */
		(*c)         = (VCB_mntd) malloc (sizeof(struct _mnt_d));
		(*c)->ilevel = 0;
		(*c)->parent = NULL;
	}

	(*c)->degree = m;
	(*c)->keys   = (unsigned char *)malloc((m+1)*keysize);
	VCB_MNT_ZEROKEY((*c)->keys, 0, keysize);
	(*c)->keys += keysize;

	if ((*c)->ilevel < (n-1)) {
		(*c)->leave = NULL;
		(*c)->lower = (VCB_mntd *) malloc(m*sizeof(VCB_mntd));
		for (i = 0; i < m; i ++) {
			(*c)->lower[i] = (VCB_mntd) malloc (sizeof(struct _mnt_d));
			(*c)->lower[i]->ilevel   = (*c)->ilevel + 1;
			(*c)->lower[i]->parent   = (*c);
			(*c)->lower[i]->whichkid = i;
			_grow_mntree(&((*c)->lower[i]), m, n, keysize);
		}
	}
	else {
		/* recursion ends here */
		(*c)->lower = NULL;
		(*c)->leave = (VCB_mntleaf*) calloc (m, sizeof(VCB_mntleaf));
	}
}

void _rearrange_itemlist(unsigned char * q, unsigned char * items, int nitems, int qrecsz, int recsz)
{
	int i;
	unsigned char * tmpptr = (unsigned char *) malloc(recsz);
	void * srcptr, * dstptr;

	q += qrecsz - 4;
	for (i = 0; i < nitems; i ++) {
		srcptr = items + i*recsz;
		dstptr = q + i*qrecsz;
		memcpy(tmpptr,dstptr,recsz);
		memcpy(dstptr,srcptr,recsz);
		memcpy(srcptr,tmpptr,recsz);
	}

	free(tmpptr);
}

void _mntree_build(VCB_mntree mntree, int nitems, void * items)
{
	int i, cnt, bucketid;
	char * dptr;
	float  percut;
	int m, n, keysize, itemsz, nattr;

	VCB_mntd L, root;
	VCB_mntleaf anchor;

	unsigned char * p;

	/* degree and numlevels are global static variables in vr_mntree.c */
	m       = mntree->degree;
	n       = mntree->numlevels;
	keysize = mntree->keysz;
	itemsz  = mntree->_itemsz;
	nattr   = mntree->nattr;

	/* time to build an empty mntree */
	root = NULL;
	_grow_mntree(&root, m, n, keysize);

	/* let's construct the hierarchy of indices */
	percut = (float) nitems;
	for (i = 0; i < n; percut /= m, i ++);
	/*  -- now we know how many items each cut shound contain */

	/* let's dump data into mntree */
	dptr = (char *)items;

	for (i = 0; i < n; i ++) {
		mntree->travers_i[i] = 0;
		mntree->bracket_l[i] = 0;
		mntree->bracket_r[i] = m - 1;
	}

	for (cnt = 0, p = items, bucketid = 0; ;) {
		bucketid ++;
		for (i = 0, L = root; i < n-1; L = L->lower[mntree->travers_i[i]], i++);

		/* i == numlevels - 1 */
		L->leave[mntree->travers_i[i]] = (VCB_mntleaf) malloc(sizeof(struct _mnt_leaf));
		anchor = L->leave[mntree->travers_i[i]];
		/* now, anchor points to a leaf */
		anchor->nitems = (int)(bucketid * percut - cnt);

		anchor->q = p + cnt*itemsz;
		cnt += anchor->nitems;
		VCB_MNT_COPYKEY(L->keys, mntree->travers_i[i], mntree->keysz, p + (cnt-1)*itemsz);
		/* end of i == numlevels - 1 */

		for (i = 0, L = root; i < n-1; L = L->lower[mntree->travers_i[i]], i++)
			VCB_MNT_COPYKEY(L->keys, mntree->travers_i[i], mntree->keysz, p + (cnt-1)*itemsz);

		if (_incr_indx(mntree, mntree->travers_i, mntree->numlevels, mntree->bracket_l, mntree->bracket_r))
			break;
	}

	mntree->root = root;
}


VCB_mntree vcbMntreeBuild(int m,     /* degree */
						int n,       /* depth */
						int keysize, /* size of each key */
						int nitems, int itemsz, void * items,
						int nattr,
						vcbmntKeyFunc    kfunc,
						vcbKeyCmpFunc    cfunc,
						vcbKeyDecompFunc dfunc,
						vcbBoundKeyFunc  bfunc)
{
	VCB_mntree mntree;
	mntree = (VCB_mntree)malloc(sizeof(struct _mnt_tree));

	/* degree and numlevels are global static variables in vr_mntree.c */
	mntree->degree    = m;
	mntree->numlevels = n;
	mntree->keysz     = keysize;
	mntree->mntdfunc  = dfunc;
	mntree->mntbfunc  = bfunc;
	mntree->mntcfunc  = cfunc;
	mntree->_itemsz   = itemsz;
	mntree->nattr     = nattr;

	qsorthelp = mntree;
	qsort(items,nitems,itemsz,compare);
	qsorthelp = NULL;

	_mntree_build(mntree,nitems,items);

	return mntree;
}

static void    _free_mntd (VCB_mntd c, VCB_mntree tree)
{
	if (c->leave != NULL) free(c->leave);
	if (c->lower != NULL) free(c->lower);
	c->keys -= tree->keysz;
	free(c->keys);
	free(c);
}

void    _free_tree(VCB_mntd mntree, VCB_mntree tree)
{
	int i;

	if (mntree -> leave == NULL) {         /* internal level */
		for (i = mntree->degree-1; i >= 0; i --)
			_free_tree(mntree->lower[i], tree);
	}
	else {                                  /* leaf level */
		for (i = 0; i < mntree->degree; i ++)
			free(mntree->leave[i]);
		_free_mntd(mntree, tree);
	}
}

void    vcbMntreeFree(VCB_mntree mntree)
{
	_free_tree(mntree->root, mntree);
	free(mntree);
}

int    vcbMntreeQuery(VCB_mntree tree, int nattr, float * lb, float * ub, void *** result)
{
	int i, flag, totalitems, numouts, itemsz;
	int payloadsz;
	float * cb;
	float * lkey, * thiskey;
	VCB_mntd L;
	VCB_mntleaf anchor;
	Dmlist dm;
	VCB_mntd mntree = tree->root;

	cb       = (float *) malloc(nattr * sizeof(float));
	lkey     = (float *) malloc (tree->keysz);

	for (i = 0; i < nattr; cb[i] = lb[i], i ++);

	tree->mntbfunc(lkey, cb);
	descend_mntree(tree, lkey, tree->bracket_l );
	tree->mntbfunc(lkey, ub);
	descend_mntree(tree, lkey, tree->bracket_r);

	dm = new_dmlist();
	totalitems = 0;
	dup_bracket(tree->numlevels, tree->travers_i, tree->bracket_l);

	itemsz = tree->_itemsz;
	payloadsz = itemsz-tree->keysz;

	numouts = 0;
	while (1) {

		for (i = 0, L = mntree; i < tree->numlevels-1; L = L->lower[tree->travers_i[i]], i++);
		anchor = L->leave[tree->travers_i[i]]; /* i == numlevels - 1 */

		for (i = 0; i < anchor->nitems; i ++) {
			thiskey = (float*)(anchor->q+i*itemsz);
			/*mntdfunc(nattr, thiskey, cb);*/
			if (outbound_bracket(nattr, thiskey, lb, ub) == 0) {
				/*dml_append(dm, thiskey + nattr, payloadsz);*/
				dml_append(dm, &thiskey, sizeof(void *));
				totalitems += 1;
				numouts = 0;
			}
			else
				numouts ++;
		}

		if (numouts > 0){
			numouts = 0;
			if ( 0 < (flag = inboundincr_bracket(nattr, cb, lb, ub))) {
				/*mntbfunc(lkey, cb);*/
				descend_mntree(tree, cb, tree->already_i);
				if (less_bracket(tree->numlevels, tree->travers_i, tree->already_i)) {
					dup_bracket(tree->numlevels, tree->travers_i, tree->already_i);
					continue;
				}
			}
		}

		if (numouts > 0 &&  flag < 0)
			break; /* the 1st of the two normal exits */

		if (_incr_indx(tree, tree->travers_i, tree->numlevels, tree->bracket_l, tree->bracket_r))
			break; /* the 2nd of the two normal exits */
	}

	(*result) = (void **) malloc (totalitems * sizeof(void *));
	dml_cpyall(dm, (*result));

	free(lkey);
	free(cb);
	return totalitems;
}

int     vcbMntreeStat(VCB_mntree tree)
{
	int totalitems, i;
	VCB_mntd L, mntree = tree->root;
	VCB_mntleaf anchor;

	for (i = 0; i < tree->numlevels; i ++) {
		tree->travers_i[i] = 0;
		tree->bracket_l[i] = 0;
		tree->bracket_r[i] = tree->degree-1;
	}

	for (totalitems = 0; ;) {
		for (i = 0, L = mntree; i < tree->numlevels-1; L = L->lower[tree->travers_i[i]], i++);
		anchor = L->leave[tree->travers_i[i]]; /* i == numlevels - 1 */

		totalitems += anchor->nitems;
		if (_incr_indx(tree, tree->travers_i, tree->numlevels, tree->bracket_l, tree->bracket_r))
		break;
	}
	return totalitems;
}

/* start of revisions for raw data in unsigned char */

static int compare_ub(const void *arg1, const void *arg2 )
/* internal function for use with qsort */
{
	unsigned char * item1, * item2;
	item1 = (unsigned char *) arg1;
	item2 = (unsigned char *) arg2;

	return qsorthelp->ubmntcfunc(item1, item2);
}

static int _binsearch_ub(VCB_mntree tree, int numvals, unsigned char * kptr, unsigned char * newkey)
{
	int l, r;
	unsigned char * vals;

	vals = kptr - tree->keysz;
	for (l = 0, r = numvals; (r-l)>1; ) {
		if (tree->ubmntcfunc(vals + tree->keysz*((l+r)/2), newkey) < 0)
			l = (l+r)/2;
		else
			r = (l+r)/2;
	}

	return (r-1);
}

static void descend_mntree_ub(VCB_mntree tree, unsigned char * newkey, int * bracket)
{
	int i, loc;
	VCB_mntd c = tree->root;

	for (i = 0; i < tree->numlevels - 1; i ++) {
		loc = _binsearch_ub(tree, tree->degree, c->keys, newkey);
		bracket[i] = loc;
		c = c->lower[loc];
	}
	loc = _binsearch_ub(tree, tree->degree, c->keys, newkey);
	bracket[i] = loc;
}



static int  outbound_bracket_ub(int numvals, int * c, int * b1, int *b2)
{ /* return >1 -> c is out of the bounds of b1-b2, 0 -> otherwise */

	int i, ret;

	for (i = 0, ret = 0; i < numvals; i ++)
		ret += ((c[i] < b1[i]) | (c[i] > b2[i]));

	return ret;
}

static int  inboundincr_bracket_ub(int numvals, int *c, int * b1, int * b2)
{ /* return :
   *    -1 -> after incr, c is still outbound
   *     0 -> c is inbound of b1-b2
   *     1 -> c is outbound, after incr, it's inbound
   */
	int i, j, carry;

	if (outbound_bracket_ub(numvals, c, b1, b2)) {
		for (i = numvals - 1, carry = 0; i >= 0; i --)  {

			c[i] += carry;
			carry = 0;

			if (c[i] < b1[i]) {
				c[i] = b1[i];
				for (j = i; j < numvals; j ++)
					c[j] = b1[j];
			}

			if (c[i] > b2[i]) {
				c[i] = b1[i];
				carry = 1;
			}
		}
		if (carry == 0)
			return 1;
		else
			return -1;
	}
	return 0;
}


VCB_mntree vcbMntreeBuildub(int m,       /* degree */
						int n,       /* depth */
						int keysize, /* size of each key */
						int nitems, int itemsz, void * items,
						int nattr,
						vcbmntKeyUBFunc    kfunc,
						vcbKeyCmpUBFunc    cfunc,
						vcbKeyDecompUBFunc dfunc,
						vcbBoundKeyUBFunc  bfunc)
{
	VCB_mntree mntree;

	mntree = (VCB_mntree)malloc(sizeof(struct _mnt_tree));

	/* degree and numlevels are global static variables in vr_mntree.c */
	mntree->degree    = m;
	mntree->numlevels = n;
	mntree->keysz     = keysize;
	mntree->ubmntdfunc  = dfunc;
	mntree->ubmntbfunc  = bfunc;
	mntree->ubmntcfunc  = cfunc;
	mntree->_itemsz     = itemsz;
	mntree->nattr       = nattr;

	qsorthelp = mntree;
	qsort(items,nitems,itemsz,compare_ub);
	qsorthelp = NULL;

	_mntree_build(mntree,nitems,items);

	return mntree;
}

int    vcbMntreeQueryub(VCB_mntree tree, int nattr, int * lb, int * ub, void *** result)
{
	int i, flag, totalitems, numouts;
	int payloadsz, itemsz;
	int * cb;
	unsigned char * lkey, * thiskey;
	VCB_mntd L, mntree = tree->root;
	VCB_mntleaf anchor;
	Dmlist dm;

	cb       = (int *) malloc(nattr * sizeof(int));
	lkey     = (unsigned char *) malloc (tree->keysz);

	for (i = 0; i < nattr; i ++) {
		ub[i] = (ub[i] > 255)? 255 : (ub[i]);
		lb[i] = (lb[i] > 0) ? (lb[i]) : 0;
	}

	for (i = 0; i < nattr; cb[i] = lb[i], i ++);

	tree->ubmntbfunc(lkey, cb);
	descend_mntree_ub(tree, lkey, tree->bracket_l );
	tree->ubmntbfunc(lkey, ub);
	descend_mntree_ub(tree, lkey,  tree->bracket_r);

	dm = new_dmlist();
	totalitems = 0;
	dup_bracket(tree->numlevels, tree->travers_i, tree->bracket_l);

	itemsz = tree->_itemsz;
	payloadsz = itemsz - tree->keysz;

	numouts = 0;
	while (1) {

		for (i = 0, L = mntree; i < tree->numlevels-1; L = L->lower[tree->travers_i[i]], i++);
		anchor = L->leave[tree->travers_i[i]]; /* i == numlevels - 1 */

		for (i = 0, thiskey = anchor->q; i < anchor->nitems; thiskey += itemsz, i ++) {
			tree->ubmntdfunc(nattr, thiskey, cb);
			if (outbound_bracket_ub(nattr, cb, lb, ub) == 0) {
			/*if ((thiskey[0] >= lb[0]) && (thiskey[0] <= ub[0]) && (thiskey[1] >= lb[1]) && (thiskey[1] <= ub[1])){*/
				/*dml_append(dm, thiskey + tree->keysz, payloadsz);*/
				dml_append(dm,&thiskey,sizeof(void *));
				totalitems += 1;
				numouts = 0;
			}
			else
				numouts ++;
		}

		if (numouts > 3){
			numouts = 0;
			if ( 0 < (flag = inboundincr_bracket_ub(nattr, cb, lb, ub))) {
				tree->ubmntbfunc(lkey, cb);
				descend_mntree_ub(tree, lkey, tree->already_i);
				if (less_bracket(tree->numlevels, tree->travers_i, tree->already_i)) {
					dup_bracket(tree->numlevels, tree->travers_i, tree->already_i);
					continue;
				}
			}
		}

		if (numouts > 3 && flag < 0)
			break; /* the 1st of the two normal exits */

		if (_incr_indx(tree, tree->travers_i, tree->numlevels, tree->bracket_l, tree->bracket_r))
			break; /* the 2nd of the two normal exits */
	}

	(*result) = (void **) malloc (totalitems * sizeof(void *));
	dml_cpyall(dm, (*result));

	free(lkey);
	free(cb);
	return totalitems;
}

unsigned char *_make_list_rec(VCB_mntree tree, VCB_mntd mntree, VCB_mntd *nodes, VCB_mntleaf *leaves)
{
	int i,j;
	static int pnode, pleave;
	static unsigned char *items;
	unsigned char *t;

	if(mntree -> parent == NULL) pnode = pleave = 0, items = NULL;

	nodes[pnode++] = mntree;

	if (mntree -> leave == NULL) {         /* internal level */
		for (i = 0; i < mntree->degree; i ++) {
			_make_list_rec(tree, mntree->lower[i], nodes, leaves);
		}
	}
	else {                                  /* leaf level */
		for (i = 0; i < mntree->degree; i ++) {
			leaves[pleave++] = mntree->leave[i];
			for (j = 0; j < mntree->leave[i]->nitems; j++) {
				memcpy(&t, &(mntree->leave[i]->q[j*tree->qrecsz+tree->keysz]), sizeof(unsigned char *));
				if ((items==NULL) || (items > t)) items = t;
			}
		}
	}

	return items;
}

unsigned char *_make_list(VCB_mntree tree, VCB_mntd *nodes, VCB_mntleaf *leaves)
{
	return _make_list_rec(tree, tree->root, nodes, leaves);
}

int vcbMntreeStore(VCB_mntree tree, int *dims, char *filename)
{
	int i,j, nitems, nnodes, nleaves, nwrote;
	VCB_mntd *nodes, mntree = tree->root;
	VCB_mntleaf *leaves;
	unsigned char *items;
	unsigned char bool;
	char errormsg[300];
	FILE *dataout;

	/* make list of nodes and leaves */
	nnodes  = (int)(pow(tree->degree,tree->numlevels)-1)/(tree->degree-1);
	nleaves = (int)pow(tree->degree,tree->numlevels);
	nodes   = (VCB_mntd    *)malloc(sizeof(VCB_mntd   )*nnodes );
	leaves  = (VCB_mntleaf *)malloc(sizeof(VCB_mntleaf)*nleaves);

	items  = _make_list(tree, nodes, leaves);
	nitems = vcbMntreeStat(tree);


	dataout = fopen(filename,"wb");
	if (!dataout) {
		sprintf(errormsg,"cannot open %s to write\n", filename);
		perror(errormsg);
		return 0;
	}

	/* write static variables first */
	fwrite(&tree->numlevels, sizeof(int), 1, dataout);
	fwrite(&tree->degree, sizeof(int), 1, dataout);
	fwrite(&tree->keysz, sizeof(int), 1, dataout);
	fwrite(&tree->qrecsz, sizeof(int), 1, dataout);
	fwrite(&tree->_itemsz, sizeof(int), 1, dataout);
	fwrite(&tree->nattr, sizeof(int), 1, dataout);
	VCB_DEBUG("numlevels: %d\n", tree->numlevels);
	VCB_DEBUG("degree: %d\n", tree->degree);
	VCB_DEBUG("keysz: %d\n", tree->keysz);
	VCB_DEBUG("qrecsz: %d\n", tree->qrecsz);
	VCB_DEBUG("nattr: %d\n", tree->nattr);

	fwrite(tree->bracket_l, sizeof(int), VCB_MAXLEVELS, dataout);
	fwrite(tree->bracket_r, sizeof(int), VCB_MAXLEVELS, dataout);
	fwrite(tree->travers_i, sizeof(int), VCB_MAXLEVELS, dataout);
	fwrite(tree->already_i, sizeof(int), VCB_MAXLEVELS, dataout);

	/* write meta data for tree and items */
	fwrite(dims, sizeof(int), 3, dataout);
	fwrite(&items, sizeof(unsigned char *), 1, dataout); //meta data for resetting pointers when loading back into memory
	fwrite(&nnodes, sizeof(int), 1, dataout);
	fwrite(&nleaves, sizeof(int), 1, dataout);
	fwrite(&nitems, sizeof(int), 1, dataout);
	VCB_DEBUG ("dims: %d %d %d\n", dims[0], dims[1], dims[2]);
	VCB_DEBUG ("items: %u\n", items);
	VCB_DEBUG ("nnodes: %d\n", nnodes);
	VCB_DEBUG ("nleaves: %d\n", nleaves);
	VCB_DEBUG ("nitems: %d\n", nitems);

	/* write tree nodes */
	for (i = 0; i < nnodes; i++) {
		fwrite(&nodes[i], sizeof(VCB_mntd), 1, dataout);
		fwrite(&nodes[i]->ilevel, sizeof(int), 1, dataout);
		fwrite(&nodes[i]->degree, sizeof(int), 1, dataout);
		fwrite(&nodes[i]->parent, sizeof(VCB_mntd), 1, dataout);
		fwrite(&nodes[i]->whichkid, sizeof(int), 1, dataout);
		VCB_DEBUG ("nodes[%d]: %u\n", i, nodes[i]);
		VCB_DEBUG ("nodes[%d]->ilevel: %d\n", i, nodes[i]->ilevel);
		VCB_DEBUG ("nodes[%d]->degree: %d\n", i, nodes[i]->degree);
		VCB_DEBUG ("nodes[%d]->parent: %u\n", i, nodes[i]->parent);
		VCB_DEBUG ("nodes[%d]->whichkid: %d\n", i, nodes[i]->whichkid);

		fwrite(nodes[i]->keys-tree->keysz, tree->keysz, tree->degree+1, dataout);
		if (nodes[i]->leave == NULL) {
			bool = 0;
			fwrite(&bool, sizeof(unsigned char), 1, dataout);
			fwrite(nodes[i]->lower, sizeof(VCB_mntd), tree->degree, dataout);
			VCB_DEBUG_CODE {VCB_DEBUG("nodes[%d]->leave: NULL\n"); for(j=0;j<tree->degree;j++) VCB_DEBUG("nodes[%d]->lower[%d]: %u\n", i, j, nodes[i]->lower[j]);}
		} else {
			bool = 1;
			fwrite(&bool, sizeof(unsigned char), 1, dataout);
			fwrite(nodes[i]->leave, sizeof(VCB_mntleaf), tree->degree, dataout);
			VCB_DEBUG_CODE {for(j=0;j<tree->degree;j++) VCB_DEBUG("nodes[%d]->leave[%d]: %u\n", i, j, nodes[i]->leave[j]); VCB_DEBUG("nodes[%d]->lower: NULL\n"); }
		}
	}

	/* write leave nodes */
	for (i = 0; i < nleaves; i++) {
		fwrite(&leaves[i], sizeof(VCB_mntleaf), 1, dataout);
		fwrite(&leaves[i]->nitems, sizeof(int), 1, dataout);
		fwrite(leaves[i]->q, tree->qrecsz, leaves[i]->nitems, dataout);
		VCB_DEBUG ("leaves[%d]: %u\n", i, leaves[i]);
		VCB_DEBUG ("leaves[%d]->nitems: %d\n", i, leaves[i]->nitems);
	}

	/* write items */
	nwrote = (int) fwrite(items, tree->_itemsz, nitems, dataout);
	VCB_DEBUG ("items: %d items at %u\n", nitems, items);
	if ( nwrote < nitems) {
		sprintf(errormsg,"In %s, can only write %d items out of %d\n", filename, nwrote, nitems);
		fprintf(stderr,"%s",errormsg);
		fclose(dataout);
		return 0;
	}

	fclose(dataout);
	return 1;
}

VCB_mntree _load_mntree(VCB_mntree tree, char *filename, unsigned char **items, int *dims)
{
	char errormsg[300];
	int i,j,k, nitems, nnodes, nleaves, nread, breaker;
	VCB_mntd *nodes, root=NULL, *oldnodes;
	VCB_mntleaf *leaves, *oldleaves;
	unsigned char *newitems, *olditems, *t, *tmp;
	unsigned char bool;
	FILE *datain;

	datain = fopen(filename,"rb");
	if (datain == NULL) {
		sprintf(errormsg,"cannot open %s to read\n", filename);
		perror(errormsg);
		return NULL;
	}

	/* read static variables first */
	fread(&tree->numlevels, sizeof(int), 1, datain);
	fread(&tree->degree, sizeof(int), 1, datain);
	fread(&tree->keysz, sizeof(int), 1, datain);
	fread(&tree->qrecsz, sizeof(int), 1, datain);
	fread(&tree->_itemsz, sizeof(int), 1, datain);
	fread(&tree->nattr, sizeof(int), 1, datain);
	VCB_DEBUG ("numlevels: %d\n", tree->numlevels);
	VCB_DEBUG ("degree: %d\n", tree->degree);
	VCB_DEBUG ("keysz: %d\n", tree->keysz);
	VCB_DEBUG ("qrecsz: %d\n", tree->qrecsz);
	VCB_DEBUG ("_itemsz: %d\n", tree->_itemsz);
	VCB_DEBUG ("nattr: %d\n", tree->nattr);

	fread(tree->bracket_l, sizeof(int), VCB_MAXLEVELS, datain);
	fread(tree->bracket_r, sizeof(int), VCB_MAXLEVELS, datain);
	fread(tree->travers_i, sizeof(int), VCB_MAXLEVELS, datain);
	fread(tree->already_i, sizeof(int), VCB_MAXLEVELS, datain);

	/* read meta data for tree and items */
	fread(dims, sizeof(int), 3, datain);
	fread(&olditems, sizeof(unsigned char *), 1, datain); //meta data for resetting pointers when loading back into memory
	fread(&nnodes, sizeof(int), 1, datain);
	fread(&nleaves, sizeof(int), 1, datain);
	fread(&nitems, sizeof(int), 1, datain);
	VCB_DEBUG ("dims: %d %d %d\n", dims[0], dims[1], dims[2]);
	VCB_DEBUG ("items: %u\n", olditems);
	VCB_DEBUG ("nnodes: %d\n", nnodes);
	VCB_DEBUG ("nleaves: %d\n", nleaves);
	VCB_DEBUG ("nitems: %d\n", nitems);

	/* create empty list for nodes and leaves, and oldnodes */
	nodes     = (VCB_mntd    *)malloc(sizeof(VCB_mntd   )*nnodes );
	leaves    = (VCB_mntleaf *)malloc(sizeof(VCB_mntleaf)*nleaves);
	oldnodes  = (VCB_mntd    *)malloc(sizeof(VCB_mntd   )*nnodes );
	oldleaves = (VCB_mntleaf *)malloc(sizeof(VCB_mntleaf)*nleaves);
	if(!nodes||!leaves||!oldnodes||!oldleaves) {
		VCB_DEBUG("%d nodes, %d leaves: ", nnodes, nleaves);
		perror("malloc main memory");
		return NULL;
	}

	/* read tree nodes */
	for (i = 0; i < nnodes; i++) {
		fread(&oldnodes[i], sizeof(VCB_mntd), 1, datain);
		nodes[i] = (VCB_mntd)malloc(sizeof(struct _mnt_d));
		if(!nodes[i]) {
			perror("malloc nodes");
			return NULL;
		}
		fread(&nodes[i]->ilevel, sizeof(int), 1, datain);
		fread(&nodes[i]->degree, sizeof(int), 1, datain);
		fread(&nodes[i]->parent, sizeof(VCB_mntd), 1, datain);
		fread(&nodes[i]->whichkid, sizeof(int), 1, datain);
		VCB_DEBUG ("nodes[%d]: %u\n", i, oldnodes[i]);
		VCB_DEBUG ("nodes[%d]->ilevel: %d\n", i, nodes[i]->ilevel);
		VCB_DEBUG ("nodes[%d]->degree: %d\n", i, nodes[i]->degree);
		VCB_DEBUG ("nodes[%d]->parent: %u\n", i, nodes[i]->parent);
		VCB_DEBUG ("nodes[%d]->whichkid: %d\n", i, nodes[i]->whichkid);

		nodes[i]->keys = (unsigned char *)malloc(tree->keysz*(tree->degree+1));
		if(!nodes[i]->keys) {
			perror("malloc nodes->keys");
			return NULL;
		}
		fread(nodes[i]->keys, tree->keysz, tree->degree+1, datain);
		nodes[i]->keys+=tree->keysz;
		fread(&bool, sizeof(unsigned char), 1, datain);
		if(!bool) {
			nodes[i]->leave= NULL;
			nodes[i]->lower = (VCB_mntd *)malloc(sizeof(VCB_mntd)* tree->degree);
			if(!nodes[i]->lower) {
				perror("malloc nodes->lower");
				return NULL;
			}
			fread(nodes[i]->lower, sizeof(VCB_mntd), tree->degree, datain);
			VCB_DEBUG_CODE {VCB_DEBUG("nodes[%d]->leave: NULL\n"); for(j=0;j<tree->degree;j++) VCB_DEBUG("nodes[%d]->lower[%d]: %u\n", i, j, nodes[i]->lower[j]);}
		} else {
			nodes[i]->lower= NULL;
			nodes[i]->leave = (VCB_mntleaf *)malloc(sizeof(VCB_mntleaf)* tree->degree);
			if(!nodes[i]->leave) {
				perror("malloc nodes->leave");
				return NULL;
			}
			fread(nodes[i]->leave, sizeof(VCB_mntleaf), tree->degree, datain);
			VCB_DEBUG_CODE {for(j=0;j<tree->degree;j++) VCB_DEBUG("nodes[%d]->leave[%d]: %u\n", i, j, nodes[i]->leave[j]); VCB_DEBUG("nodes[%d]->lower: NULL\n"); }
		}
	}

	/* read leave nodes */
	for (i = 0; i < nleaves; i++) {
		fread(&oldleaves[i], sizeof(VCB_mntleaf), 1, datain);
		leaves[i] = (VCB_mntleaf)malloc(sizeof(struct _mnt_leaf));
		if(!leaves[i]) {
			perror("malloc leaves");
			return NULL;
		}
		fread(&leaves[i]->nitems, sizeof(int), 1, datain);
		leaves[i]->q = (unsigned char *)malloc(tree->qrecsz*leaves[i]->nitems);
		if(!leaves[i]->q) {
			perror("malloc leaves->q");
			return NULL;
		}
		fread(leaves[i]->q, tree->qrecsz, leaves[i]->nitems, datain);
		VCB_DEBUG ("leaves[%d]: %u\n", i, oldleaves[i]);
		VCB_DEBUG ("leaves[%d]->nitems: %d\n", i, leaves[i]->nitems);
	}

	/* allocate memory for items */
	newitems = (unsigned char*)malloc(tree->_itemsz*nitems);
	if (!newitems) {
		sprintf(errormsg,"cannot allocate space for items array, need %d bytes\n", tree->_itemsz*nitems);
		perror(errormsg);
		fclose(datain);
		return NULL;
	}

	/* read items */
	nread = (int) fread(newitems, tree->_itemsz, nitems, datain);
	VCB_DEBUG ("items: %d items at %u\n", nitems, olditems);
	if ( nread < nitems) {
		sprintf(errormsg,"In %s, can only read %d vals out of %d\n", filename, nread, nitems);
		fprintf(stderr,"%s",errormsg);
		free(newitems);
		fclose(datain);
		return NULL;
	}

	fclose(datain);

	/* set items main pointer, set root */
	*items = newitems;
	root = nodes[0];
	tree->root = root;

	/* reinitialize item pointers in leaves */
	for (i = 0; i < nleaves; i++) {
		for (j = 0; j < leaves[i]->nitems; j++) {
			memcpy(&t, &(leaves[i]->q[j*tree->qrecsz+tree->keysz]), sizeof(unsigned char *));
			tmp = t;
			if (newitems > olditems) {
				t += (newitems - olditems);
			} else {
				t -= (olditems - newitems);
			}
			memcpy(&(leaves[i]->q[j*tree->qrecsz+tree->keysz]), &t, sizeof(unsigned char *));
		}
	}

	/* reinitialize node and leaf pointers in nodes */
	for (i = 0; i < nnodes; i++) {
		if (nodes[i]->lower == NULL) {
			/* substitute leaf pointers */
			for (j = 0; j < tree->degree; j++) {
				breaker = 0;
				for(k = 0; !breaker && k < nleaves; k++) {
					if (oldleaves[k] == nodes[i]->leave[j]) {
						nodes[i]->leave[j] = leaves[k];
						breaker=1;
					}
				}
				if (!breaker) VCB_DEBUG("no break in nodes[%d]->leave[%d]!\n", i, j);
			}
		} else {
			/* substitute child nodes pointers */
			for (j = 0; j < tree->degree; j++) {
				breaker = 0;
				for(k = 0; !breaker && k < nnodes; k++) {
					if (oldnodes[k] == nodes[i]->lower[j]) {
						nodes[i]->lower[j] = nodes[k];
						breaker=1;
					}
				}
				if (!breaker) VCB_DEBUG("no break in nodes[%d]->lower[%d]!\n", i, j);
			}
		}
		if (nodes[i]->parent != NULL) {
			/* substitute parent pointer */
			breaker = 0;
			for(k = 0; !breaker && k < nnodes; k++) {
				if (oldnodes[k] == nodes[i]->parent) {
					nodes[i]->parent = nodes[k];
					breaker=1;
				}
			}
			if (!breaker) VCB_DEBUG("no break in nodes[%d]->parent!\n", i);
		}
	}

	/* return tree */
	return tree;
}

VCB_mntree vcbMntreeLoad(char *filename, unsigned char **items, int *dims, vcbKeyCmpFunc cfunc, vcbKeyDecompFunc dfunc, vcbBoundKeyFunc bfunc)
{
	VCB_mntree tree = (VCB_mntree) malloc(sizeof(struct _mnt_tree));

	tree->mntdfunc  = dfunc;
	tree->mntbfunc  = bfunc;
	tree->mntcfunc  = cfunc;

	return _load_mntree(tree, filename, items, dims);
}

VCB_mntree vcbMntreeLoadub(char *filename, unsigned char **items, int *dims, vcbKeyCmpUBFunc cfunc, vcbKeyDecompUBFunc dfunc, vcbBoundKeyUBFunc bfunc)
{
	VCB_mntree tree = (VCB_mntree) malloc(sizeof(struct _mnt_tree));

	tree->ubmntdfunc  = dfunc;
	tree->ubmntbfunc  = bfunc;
	tree->ubmntcfunc  = cfunc;

	return _load_mntree(tree, filename, items, dims);
}
