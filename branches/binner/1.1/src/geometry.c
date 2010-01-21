#include "geometry.h"
#include "vcblinalg.h"

double dist_plane(double * normal, double * pnt, double * vert0)
{
	double d;
	d = -(VCB_DOT(normal, vert0));
	return (VCB_DOT(normal, pnt) + d);
}

double inside_prism(double * pnt, int n, double * normals, double * verts)
{
	int i;
	
	for (i = 0; i < n; i ++)
		if (dist_plane(&normals[i*3], pnt, &verts[i*3]) > 1e-6)
			return 0;

	return 1;
}

void cal_normal(double * n, double * v)
{
	double v1[3], v2[3];

	v1[0] = v[3] - v[0];
	v1[1] = v[4] - v[1];
	v1[2] = v[5] - v[2];
	
	v2[0] = v[6] - v[0];
	v2[1] = v[7] - v[1];
	v2[2] = v[8] - v[2];

	VCB_CROSS(n,v1,v2);
}

double vec_normalize(double * n)
{
	double norm;
	/*
	norm = BINNER_NORM(v);
	
	v[0] /= norm;
	v[1] /= norm;
	v[2] /= norm;
	*/

	norm = BINNER_NORM(n);
	if (norm < BINNER_EPSILON) {
		n[0] = n[1] = n[2] = 0.0;
		return 0.0;
	}
	else {
		n[0] /= norm;
		n[1] /= norm;
		n[2] /= norm;
		return norm;
	}
	
}
