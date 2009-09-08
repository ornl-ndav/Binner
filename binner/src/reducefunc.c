#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include "jrb.h"
#include "dmem.h"
#include "rebinapp.h"
#include "reducefunc.h"

#define ITEMSIZE (sizeof(int)*4 + sizeof(double)*2)

static JRB b;
static Dmlist dm;
static double volumescale, spacing[3];
static int orig[3];

static void calc_bounds(int argc, char ** argv)
{
	int    c, j, xyzsize[3];
	double cellsize, askbounds[6]; 

	c = argc - 10;
	askbounds[0] = atof(argv[c+1]);
	askbounds[1] = atof(argv[c+2]);
	spacing[0]   = atof(argv[c+3]);
	askbounds[2] = atof(argv[c+4]);
	askbounds[3] = atof(argv[c+5]);
	spacing[1]   = atof(argv[c+6]);
	askbounds[4] = atof(argv[c+7]);
	askbounds[5] = atof(argv[c+8]);
	spacing[2]   = atof(argv[c+9]);

	for (j = 0; j < 3; j++)
		xyzsize[j] = (int)ceil((askbounds[j*2+1] - askbounds[j*2])/spacing[j]);

	for (j = 0; j < 3; j++)
		spacing[j] = (askbounds[j*2+1] - askbounds[j*2])/xyzsize[j];

	cellsize = fv_bounds(askbounds, spacing, orig, xyzsize);

	volumescale = (cellsize/spacing[0]) * (cellsize/spacing[1]) * (cellsize/spacing[2]);
	volumescale =  1.0/volumescale;

	output_askinginfo(askbounds, xyzsize, spacing);
	output_prerebininfo(orig, xyzsize, spacing, cellsize);
}

int reduce_init(int argc, char ** argv)
{
	b = make_jrb();
	dm = new_dmlist();
#if REBINDEBUG			
	fprintf(stderr, "reduce_init, argc = %d\n", argc);
#endif
	calc_bounds(argc, argv);

	/* return unitsize */
	return ITEMSIZE;
}

static int cmp (Jval a, Jval b)
{
	int * i1, * i2, k;
	
	i1 = jval_v(a);
	i2 = jval_v(b);
	
	/* i1[0], i2[0]: sliceid, should be the same */
	for (k = 1; k < 4; k ++)
		if (i1[k] != i2[k])
			return (i1[k] - i2[k]);
	
	return 0;
}

static void accumulate_counts(void * h, void * v)
{
	char * hc, * vc;
	double * d1, * d2;

	hc = h;
	vc = v;
	
	hc += sizeof(int)*4;
	vc += sizeof(int)*4;
	
	d1 = (double *) hc;
	d2 = (double *) vc;
	
	d1[0] += d2[0]; /* rebinned counts */
	d1[1] += d2[1]; /* rebinned error */
}

int reduce_func(void * v, int k)
{
	void * h;
	Jval key;
	JRB bn;
	int i;

	/*
	 * the input contains k items
	 * starting at the address pointed to by v
	 */
#if 0
	write(1, v, k*ITEMSIZE); 
	return 0;
#endif

	for (i = 0; i < k; v += ITEMSIZE, i ++)
	{
		key.v = v;
		bn = jrb_find_gen(b, key, cmp); 

		if (bn == NULL) {
			h = dml_append(dm, v, ITEMSIZE);
			key.v = h;
			jrb_insert_gen(b, key, key, cmp);
		}
		else
		{
			h = jval_v(bn->val);
			accumulate_counts(h, v);
		}
	}

	return 0;
}

int reduce_done()
{
	JRB bn;
	void * v;
	char * c;
	int  * ip;
	double * dp;
	double totalvolume = 0.;
	int nvox = 0;

	jrb_traverse(bn, b) {
		v = jval_v(bn->key);
		ip = v;
		c = v;
		dp = (double *)(c+16);
		
		if (dp[0] > 1e-16)
		{
			dp[0] *= volumescale;
			dp[1] *= volumescale;
			totalvolume += dp[0];
			gmesh_singlebin_output(dp, ip[0], ip[1], ip[2], ip[3], orig, spacing);
			nvox ++;
		}
		/*write(1, v, ITEMSIZE);*/
	}

	fprintf(stderr, "rebinned n_voxels   : %d\n", nvox);
	fprintf(stderr, "rebinned sum_energy : %lf\n", totalvolume);

	free_dmlist(dm);
	jrb_free_tree(b);
	
	return 0;
}