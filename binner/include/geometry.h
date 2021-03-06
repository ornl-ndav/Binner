/**
   \ingroup rebinner_core
   
   \file include/geometry.h

   \brief CURRENT core API -- functions for geometric operations.

   $Id$
*/

#ifndef _BINNER_GEOMETRY_
#define _BINNER_GEOMETRY_

#include <math.h>
#include "macros.h"


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
