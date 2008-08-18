#ifndef _BINNER_GEOMETRY_
#define _BINNER_GEOMETRY_

#include <math.h>

#ifndef BINNER_EPSILON
#define BINNER_EPSILON 1E-8
#endif

#define BINNER_NORM(n)	sqrt((n)[0]*(n)[0]+(n)[1]*(n)[1]+(n)[2]*(n)[2])

#ifdef __cplusplus
extern "C" {
#endif

void cal_normal(double * n, double * v);
double vec_normalize(double * n);
					
double dist_plane(double * normal, double * pnt, double * vert0);
double inside_prism(double * pnt, int n, double * normals, double * verts);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif