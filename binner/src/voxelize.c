#include <stdio.h>
#include "vcblinalg.h"
#include "binner.h"
#include "geometry.h"
#include "voxelize.h"


void voxelize_poly(	int n, double * v, /* the polygon */
					unsigned int * wvol, /* working volume, contain tags when done */
					int * orig,
					int * xyzsize,
					double ccs,
					int base, /* base is the max val of polyid */
					int polyi) /* cell size */
{

	//double  fimin,fjmin,fkmin,fimax,fjmax,fkmax;
	int		imin,imax,jmin,jmax,kmin,kmax;
	double  poly_eq[4];
	double  edge[10*3], edget[10], *ep; /* direction vector and length of each edge */
	double  crossedge[10*3], * cp;

	double  cs;  /* half of cell size */
	double  a, b, c, d, limitf, sqrt3, limit, limitf_2, t, tmax;
	int i, j, k, f, id, xdim, ydim, zdim, flag, fill;
	double  xyz[3], ori[3], dist, v1[3], bounds[6], radi;

	double maxcos, temp;

	unsigned int polyid = (unsigned int) polyi;
	
	cs     = ccs/2.0;
	sqrt3  = sqrt(3.0);
	limitf = sqrt3*cs;

	ori[0] = orig[0] * ccs + cs;
	ori[1] = orig[1] * ccs + cs;
	ori[2] = orig[2] * ccs + cs;

	xdim = xyzsize[0];
	ydim = xyzsize[1];
	zdim = xyzsize[2];

	bounding_box(n, bounds, v);
/*
	fimin = fjmin = fkmin = 1e6;
	fimax = fjmax = fkmax = -1e6;

	for (k = 0, vp = v; k < n; vp += 3, k++)
	{
		if (vp[0]<fimin)  fimin=vp[0];
		if (vp[0]>fimax)  fimax=vp[0];
		if (vp[1]<fjmin)  fjmin=vp[1];
		if (vp[1]>fjmax)  fjmax=vp[1];
		if (vp[2]<fkmin)  fkmin=vp[2];
		if (vp[2]>fkmax)  fkmax=vp[2];
	}
*/
	/* make a bounding box of the facet */
	imin = (int)floor(bounds[0]/ccs);
	jmin = (int)floor(bounds[2]/ccs);
	kmin = (int)floor(bounds[4]/ccs);

	imax = (int)ceil(bounds[1]/ccs);
	jmax = (int)ceil(bounds[3]/ccs);
	kmax = (int)ceil(bounds[5]/ccs);

/*
	imin=(int)(fimin/ccs);
	imax=(int)(fimax/ccs);

	jmin=(int)(fjmin/ccs);
	jmax=(int)(fjmax/ccs);

	kmin=(int)(fkmin/ccs);
	kmax=(int)(fkmax/ccs);
*/
	/* compute polygon's equation */
	cal_normal(poly_eq, v);
	vec_normalize(poly_eq);
	poly_eq[3] = -VCB_DOT(poly_eq, v); /* a,b,c,d of the equation in poly_eq */
	a = poly_eq[0];
	b = poly_eq[1];
	c = poly_eq[2];
	d = poly_eq[3];

	//printf("orig: (%d %d %d) bounds (%d %d %d) - (%d %d %d), eq:%lf %lf %lf %lf\n",orig[0], orig[1], orig[2], imin, jmin, kmin, imax, jmax, kmax, a, b, c, d);

	maxcos=0.0;
	temp=fabs(a+b+c)/sqrt3;
	if(temp>maxcos)  maxcos=temp;
	temp=fabs(a-b+c)/sqrt3;
	if(temp>maxcos)  maxcos=temp;
	temp=fabs(-a+b+c)/sqrt3;
	if(temp>maxcos)  maxcos=temp;
	temp=fabs(-a-b+c)/sqrt3;
	if(temp>maxcos)  maxcos=temp;

	/* voxel center distanced > limit are not on surface */
	limit = limitf*maxcos + BINNER_EPSILON; 
	/* for comparing against radius^2 */
	limitf_2 = (limitf+BINNER_EPSILON) * (limitf+BINNER_EPSILON); 
	//printf("ccs %lf, maxcos %lf, limitf %lf, limit %lf, limif_2 %lf\n", ccs, maxcos, limitf, limit, limitf_2);

	/*
	 *	General calculations for each polygon edge. We hold the results for 
	 *	reference during the evaluation of the potential voxels. 
	 */

	for (i=0, ep=edge, cp=crossedge; i<n; ep+=3, cp+= 3, i++) {
		j = (i+1)%n;
		ep[0] = v[j*3+0] - v[i*3+0];
		ep[1] = v[j*3+1] - v[i*3+1];
		ep[2] = v[j*3+2] - v[i*3+2];
		vec_normalize(ep);
		
		tmax=(v[j*3+0]-v[i*3+0])/ep[0];

		temp=(v[j*3+1]-v[i*3+1])/ep[1];
		if (temp > tmax) tmax=temp;		
		temp=(v[j*3+2]-v[i*3+2])/ep[2];
		if (temp > tmax) tmax=temp;

		edget[i] = tmax;

		VCB_CROSS(cp, ep, poly_eq);
		vec_normalize(cp);
	}
	
	/*
	 *	for all voxels in the bounding box, DO ...
	 */
	for (i=imin;i<=imax;i++)
		for (j=jmin;j<=jmax;j++)
			for (k=kmin;k<=kmax;k++)
			{
				id = ((i - orig[0]) * ydim + (j-orig[1])) * zdim + k - orig[2];
/*
				xyz[0] = ori[0] + i * ccs;
				xyz[1] = ori[1] + j * ccs;
				xyz[2] = ori[2] + k * ccs;
*/
				xyz[0] = i*ccs + cs;
				xyz[1] = j*ccs + cs;
				xyz[2] = k*ccs + cs;
				
				dist = fabs(VCB_DOT(xyz, poly_eq) + poly_eq[3]);			
				
				flag=0;

				if (dist<=(limitf + BINNER_EPSILON))
				{
					/*	SPHERE TEST: in the sphere at the vertex ??? */

					for (f=0;f<n;f++)
					{
						v1[0] = xyz[0] - v[f*3+0];
						v1[1] = xyz[1] - v[f*3+1];
						v1[2] = xyz[2] - v[f*3+2];
						radi = VCB_DOT(v1, v1);

						if (radi<=limitf_2)
						{
							flag=1;
							wvol[id] = wvol[id] * base + polyid;
							/*printf("updated0: %d %d %d, polyid: %d\n", i,j,k,polyid-1);*/
							break;
						}
					}
						
					/* CYLINDER TEST: Voxel in the cylinder  ??? */

					if (flag==0)
					{
						fill=1;

						for (f=0;f<n;f++)
						{
							v1[0] = xyz[0] - v[f*3+0];
							v1[1] = xyz[1] - v[f*3+1];
							v1[2] = xyz[2] - v[f*3+2];
							t = VCB_DOT((&edge[f*3]), v1);

							tmax = edget[f];

							if ((t >= 0.0)&&(t <= tmax))
							{
								v1[0] = xyz[0] - (v[f*3+0]+t*edge[f*3+0]);
								v1[1] = xyz[1] - (v[f*3+1]+t*edge[f*3+1]);
								v1[2] = xyz[2] - (v[f*3+2]+t*edge[f*3+2]);

								radi = VCB_DOT(v1, v1);
								if (radi<=limitf_2)
								{
									flag=1;
									wvol[id] = wvol[id] * base + polyid;
									/*printf("updated1: %d %d %d, polyid: %d\n", i,j,k,polyid-1);*/
									break;
								}
							}
							
							if (flag==0)
								if (inside_prism(xyz, n, crossedge, v) != 1)
									fill = 0;
						}
					}
					
					if (flag == 0)
						if ((fill==1)&&(dist <= limit))
						{
							flag=1;
							wvol[id] = wvol[id] * base + polyid;
							/*printf("updated2: %d %d %d, polyid: %d\n", i,j,k,polyid-1);*/

						}
				
				}
				
				
			}
}

