#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "vcblinalg.h"
#include "binner.h"
#include "clip.h"

#define INTERP( t, x1, x2 )   ( (x1) + (t) * ((x2) - (x1)) )

void vertex_interp(double t, double * result, double * v1, double * v2)
{
	result[0] = INTERP(t, v1[0], v2[0]);
	result[1] = INTERP(t, v1[1], v2[1]);
	result[2] = INTERP(t, v1[2], v2[2]);
}

int clip(int nv, double * verts, double * vbuf, int * onbuf, double * cplane)
{
	/* 
	 * vbuf is intended to be something like: double vbuf[3*10];
	 *      the vertex buffer: caller should allocate for a safe number of vertices 
	 *      say, more than 10 vertices in the resulting polygon?
	 * 
	 * onbuf is of a similar intention. onbuf tells which vertices are on the cplane
	 */
	double *vp; /* iterator pointer into vbuf */
	double *v;  /* iterator pointer into verts */
	double *s;
	int i, ninside;
	double ddist, sdist, t;
	
	s  = verts + (nv - 1) * 3; /* s is the starting point */
	/* vp is the current point */
	for (i = 0, ninside = 0, vp = vbuf, v = verts; i < nv; v+= 3, i ++) {

		ddist = VCB_DOT(v, cplane) + cplane[3];
		
		if (ddist < 0.0) /* d is inside */
		{
			if ((sdist = (VCB_DOT(s, cplane) + cplane[3])) < 0.0) /* s is also inside */
			  ;
			else /* s is outside */
			{
				/* between s and d, there is an interaction point */
				ddist = fabs(ddist);
				sdist = fabs(sdist);
				t = sdist/(sdist + ddist);
				vertex_interp(t, vp, s, v);
				vp += 3;
				onbuf[ninside] = 1;
				ninside ++;					
			}

			vp[0] = v[0];
			vp[1] = v[1];
			vp[2] = v[2];

			vp += 3;
			onbuf[ninside] = 0;
			ninside ++;
		}
		else /* d is outside */
		{
			if ((sdist = (VCB_DOT(s, cplane) + cplane[3])) < 0.0) /* s is inside */ 
			{
				/* between s and d, there is an interaction point */
				ddist = fabs(ddist);
				sdist = fabs(sdist);
				t = sdist/(sdist + ddist);
				vertex_interp(t, vp, s, v);
				vp += 3;
				onbuf[ninside] = 1;
				ninside ++;		
			}
			else /* if s is also outside, nothing to do */
			  ;
		
		}
		
		s = v;
	}

	return ninside;
}

void swap_v1v2(double * c1, double * c2)
{
	double tm[3];
	tm[0] = c1[0];
	tm[1] = c1[1];
	tm[2] = c1[2];
	c1[0] = c2[0];
	c1[1] = c2[1];
	c1[2] = c2[2];
	c2[0] = tm[0];
	c2[1] = tm[1];
	c2[2] = tm[2];
}


double clockwise_cmp(double * c1, double * c2, double * center, double * cplane)
{
	double v1[3], v2[3], n[3];
	
	v1[0] = c1[0] - center[0];
	v1[1] = c1[1] - center[1];
	v1[2] = c1[2] - center[2];
	
	v2[0] = c2[0] - center[0];
	v2[1] = c2[1] - center[1];
	v2[2] = c2[2] - center[2];

	VCB_CROSS(n,v1,v2);

	return VCB_DOT(n, cplane);
}

int close_enough(double * c1, double * c2)
{
	double v1[3], dist;

	v1[0] = c1[0] - c2[0];
	v1[1] = c1[1] - c2[1];
	v1[2] = c1[2] - c2[2];
	dist = BINNER_NORM(v1);

	if (dist <= BINNER_EPSILON) 
		return 1;
	else
		return 0;
}

int build_polygon(int ne, double * inbuf, double * cplane)
{
	int i;
	double center[3], *cbuf, *cp, *np;
	
	if (ne <=2) return 0; /* cannot build a meaningful polygon */

	cbuf = inbuf;

	/* can we do a sort? */
	center[0] = center[1] = center[2] = 0.0;
	for (i = 0, cp = cbuf; i < ne*2; cp += 3, i ++) {
		center[0] += cp[0];
		center[1] += cp[1];
		center[2] += cp[2];
	}

	center[0] /= (ne * 2);
	center[1] /= (ne * 2);
	center[2] /= (ne * 2);
	
	/* now let's turn all edges to clockwise direction */
	
	for (i = 0, cp = cbuf; i < ne; cp += 6, i ++)
		if (clockwise_cmp(cp, cp+3, center, cplane) < 0.0)
			swap_v1v2(cp, cp+3); /* need to flip order */

	for (cp = cbuf+3; cp < cbuf + (ne*6); cp += 6) {
		for ( np = cp + 3; np < cbuf + (ne*6); np +=6)
			if (close_enough(cp, np) > 0) {
				swap_v1v2(cp+3, np);
				swap_v1v2(cp+6, np+3);
			}
	}
	
	np = inbuf;
	cbuf = malloc(ne * 3 * sizeof(double));
	cp = cbuf;
	memcpy(cp, np, 3 * sizeof(double));
	cp += 3;
	np += 3;
	for (i = 1; i < ne; cp += 3, np+=6, i ++)
		memcpy(cp, np, 3 * sizeof(double));

	memcpy(inbuf, cbuf, ne*3*sizeof(double));

	free(cbuf);
	return ne;
}


int clean_polyhedron(int nfacets, int ** nverts, double **v)
{
	/* 
	 * after a sequence of clipping operations
	 * a facet may become a single point, while an edge maybe
	 * have its two end points being the same 
	 * 
	 * this function produces a cleaned polyhedron
	 */
	int nv[10];
	double vbuf[10*3];   /* each facet has fewer than 10 vertices */
	double pbuf[10*3*20]; /* each polyhedron has fewer han 20 facets */
	double * vp, *vpbuf;

	int i, j, n, nf;

	nf = 0;
	vp = (*v);
	vpbuf = pbuf;

	for (i = 0; i < nfacets; vp += (*nverts)[i]*3, i ++)
	{
		n = 0;
		for (j = 0; j < (*nverts)[i]-1; j ++)
			if (close_enough(&vp[j*3], &vp[j*3+3]) == 0)
			{
				vbuf[n*3+0] = vp[j*3+0];
				vbuf[n*3+1] = vp[j*3+1];
				vbuf[n*3+2] = vp[j*3+2];
				n ++;
			}
			
		if (close_enough(&vp[j*3], &vp[0]) == 0)
		{
			vbuf[n*3+0] = vp[j*3+0];
			vbuf[n*3+1] = vp[j*3+1];
			vbuf[n*3+2] = vp[j*3+2];
			n ++;
		}
		
		if (n > 2)
		{
			nv[nf] = n;
			memcpy(vpbuf, vbuf, n*3*sizeof(double));
			vpbuf += n*3;
			nf++;
		}
	}
	
	free(*nverts);
	free(*v);
	(*nverts) = malloc(nf * sizeof(int));
	memcpy((*nverts), nv, nf * sizeof(int));
	(*v) = malloc((vpbuf - pbuf)*sizeof(double));
	memcpy((*v), pbuf, (vpbuf - pbuf)*sizeof(double));

	return nf;
}

/* return val: number of resulting faces. >= nfacets */
int clip_polyhedral(int nfaces, int ** nverts, double ** verts, double * cplane)
{
	/* 
	 * a polyhedral cell is described by:
	 *    nfaces:         number of faces
	 *    *nverts[nfaces]: the number of vertices on each face
	 *    *verts:          a linear array of vertices
	 *
	 * note: faces are treated as independent of each other
	 *  i.e. no mesh type of topology is stored, although such information
	 *       can be computed from the available data
	 */
	double vbuf[10 * 3], *vp;
	int nv, onbuf[10];
	int noutfaces, * noutverts, totalvertices;
	double * outverts, *op;
	
	double cbuf[20*3], *cp; /* vertex buffer for polygon on clipping plane */
	int    nc;         /* number of polygon vertices on clipping plane */
	int    ne;         /* number of polygon edges on clipping plane */

	int i, j;

	/* 
	 * at maximal, the new polyhedra have one more face
	 * still assume 10 vertices per face
	 * 3 cooridnates (xyz) on each vertex
	 * everything in double precision
	 */ 
	outverts = malloc((nfaces+1)*10*3*sizeof(double));
	noutverts = malloc((nfaces+1)*sizeof(int));

	noutfaces = 0;
	op = outverts;
	totalvertices = 0;
	ne = 0;
	cp = cbuf;

	for (i = 0, vp = (*verts); i < nfaces; vp += ((*nverts)[i]*3), i ++) {
		nv = clip((*nverts)[i], vp, vbuf, onbuf, cplane);
		if (nv > 0) /* not all vertices are clipped out */
		{
			noutverts[noutfaces] = nv;
			memcpy(op, vbuf, nv*3*sizeof(double));
			op += nv * 3;
			noutfaces ++;
			totalvertices += nv;
			
			/*
			 * now scan for all edges on the clipping plane
			 * if there are > 1 edge on the clipping plane
			 * the clipping plane should make up a new face
			 *
			 * actually, there should always be a new face
			 * formed by the clipping plane, if there is truly
			 * a cross-section/intersection
			 */
			for (j = 0; j < nv; j ++)
				if (onbuf[j] > 0)
					if (onbuf[(j + 1)%nv] > 0)
						if (close_enough(&vbuf[j*3],&vbuf[((j+1)%nv)*3]) == 0)
						{
							memcpy(cp, &vbuf[j*3], 3*sizeof(double));
							cp += 3;
							memcpy(cp, &vbuf[((j+1)%nv)*3], 3*sizeof(double));
							cp += 3;
							ne ++;
						}

		}
	}

	if (ne > 0)
	{
		nc = build_polygon(ne, cbuf, cplane);

		if (nc > 0) /* succeeded in building a polygon on clipping plane */
		{
			memcpy(op, cbuf, nc*3*sizeof(double));
			noutverts[noutfaces] = nc;
			noutfaces ++;
			totalvertices += nc;
		}
	}
	
	free(*nverts);
	free(*verts);

	(*nverts) = malloc(noutfaces * sizeof(int));
	memcpy((*nverts), noutverts, noutfaces * sizeof(int));
	
	(*verts)  = malloc(totalvertices * 3 * sizeof(double));
	memcpy((*verts), outverts, totalvertices * 3 * sizeof(double));

	free(outverts);
	free(noutverts);

	noutfaces = clean_polyhedron(noutfaces, nverts, verts);
	
	return noutfaces;
}


