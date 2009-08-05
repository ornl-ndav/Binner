#include <math.h>
#include "intersect.h"
#include "vcblinalg.h"

#ifndef VCB_EPSILON
#define VCB_EPSILON 1E-8
#endif

int intersect_triangle(float orig[3], float dir[3],
					   float vert0[3], float vert1[3], float vert2[3],						
					   float *t, float *u, float *v)
/* 
 * an optimized ray triangle intersection routine, originally
 * developed by Tomas Moller, Ben Trumbore in May 2000.
 *
 * orig: starting point of the ray
 * dir:  forward going directino of the ray
 * vert0-3: vertices describing the triangle
 * t, u, v: location, as in parameterized formula of the ray, of the intersection
 *
 * return: 0 for failure (e.g. ray parallel to the triangle)
 *         1 for success
 */
{
   float edge1[3], edge2[3], tvec[3], pvec[3], qvec[3];
   float det,inv_det;

   /* find vectors for two edges sharing vert0 */
   VCB_SUB(edge1, vert1, vert0);
   VCB_SUB(edge2, vert2, vert0);

   /* begin calculating determinant - also used to calculate U parameter */
   VCB_CROSS(pvec, dir, edge2);

   /* if determinant is near zero, ray lies in plane of triangle */
   det = VCB_DOT(edge1, pvec);

   /* calculate distance from vert0 to ray origin */
   VCB_SUB(tvec, orig, vert0);
   inv_det = 1.f / det;
   
   VCB_CROSS(qvec, tvec, edge1);
      
   if (det > VCB_EPSILON) {
      *u = VCB_DOT(tvec, pvec);
      if (*u < 0.0 || *u > det)
		  return 0;

      /* calculate V parameter and test bounds */
      *v = VCB_DOT(dir, qvec);
      if (*v < 0.0 || *u + *v > det)
		  return 0;
   }
   else if(det < -VCB_EPSILON) {
      /* calculate U parameter and test bounds */
      *u = VCB_DOT(tvec, pvec);
      if (*u > 0.0 || *u < det)
		  return 0;
      
      /* calculate V parameter and test bounds */
      *v = VCB_DOT(dir, qvec) ;
      if (*v > 0.0 || *u + *v < det)
		  return 0;
   }
   else return 0;  /* ray is parallell to the plane of the triangle */

   *t = VCB_DOT(edge2, qvec) * inv_det;
   (*u) *= inv_det;
   (*v) *= inv_det;

   return 1;
}

int ray_triangle_intersection(float *startpnt, float *dir, float *ver1, float *ver2, float *ver3, float * intpnt)
/*
 * a utility funcion for finding ray-triangle intersection by calling intersect triangle
 * it's merely a wrapper function
 * 
 * inputs:
 *   startpnt and dir: defines the ray
 *   ver1-3:           3 vertices of the triangle
 * output:
 *   intpnt:           the 3D coordinate of the intersection point 
 * return:
 *   0 - failure, 1 - success
 *
 * note: all memory locations are assumed to be valid
 */
{
	int     ret;
	float   t, u, v;

	ret = intersect_triangle(startpnt, dir, ver1, ver2, ver3, &t, &u, &v);

	if (ret > 0) {
		intpnt[0] = startpnt[0] + t*dir[0];
		intpnt[1] = startpnt[1] + t*dir[1];
		intpnt[2] = startpnt[2] + t*dir[2];
	}
  
	return ret;
}

int ray_polygon_intersection(float *startpnt, float *dir, float * verlist[3], int vernum, float * intpnt)
/*
 * a utility funcion for finding ray-polygon intersection by calling ray_triangle_intersection
 * 
 * inputs:
 *   startpnt and dir: defines the ray
 *   verlist:          all vertices of the polygon
 *   vernum:           number of vertices in the polygon
 *
 * output:
 *   intpnt:           the intersection point 
 * return:
 *   0 - failure, 1 - success
 *
 * note: all memory locations are assumed to be valid
 */
{
	float * ver[3];
	int j;

	ver[0] = verlist[0];
	for (j=1; j<vernum-1; j++) {
		ver[1] = verlist[j];
		ver[2] = verlist[j+1];
    
		if (ray_triangle_intersection(startpnt, dir, ver[0], ver[1], ver[2], intpnt)>0) 
			return 1;
	}
  
	return 0;
}

float ray_block_intersection(float *startpnt, float *dir, float * bound, float * intpnt)
/*
 * the main interface function for finding ray-block intersection
 * 
 * inputs:
 *   startpnt and dir: defines the ray
 *   bound:            the coordinats of the bounding box, there must be 8 verices, i.e. 24 floats
 *                     the order of the vertices must be:
 *                                (xmin, ymin, zmin)
 *                                (xmax, ymin, zmin)
 *                                (xmax, ymax, zmin)
 *                                (xmin, ymax, zmin)
 *                                (xmin, ymin, zmax)
 *                                (xmax, ymin, zmax)
 *                                (xmax, ymax, zmax)
 *                                (xmin, ymax, zmax)
 *
 * output:
 *   intpnt:           the intersection points, always two intersection points. must have
 *                     space for 6 floats. in case where only one intersection point is
 *                     found with the block, since no sample can be guaranteed on the
 *                     ray, treat it the same as having no intersections
 *                     the two intersection points are sorted in the order of the
 *                     first intersection and the second intersection.
 *
 * return:
 *   0 - no intersection, otherwise - the distance between the two intersection points
 *
 * note: all memory locations are assumed to be valid
 */
{
	int     count = 0, i, id0, id1, id2, id3, id;
	static  int polygon[6][4] = {{0, 1, 2, 3}, {2, 1, 5, 6}, {3, 2, 6, 7}, 
	                        {0, 3, 7, 4}, {4, 7, 6, 5}, {0, 4, 5, 1}};

	float * poly[4];
	float   dist = 0.f, xdist, ydist, zdist, tmpv;

	for (i=0; i<6; i++) {
		id0 = polygon[i][0]; poly[0] = & bound[id0*3];
		id1 = polygon[i][1]; poly[1] = & bound[id1*3];
		id2 = polygon[i][2]; poly[2] = & bound[id2*3];
		id3 = polygon[i][3]; poly[3] = & bound[id3*3];

		if (ray_polygon_intersection(startpnt, dir, poly, 4, &intpnt[count*3]) > 0) 
			count++;
		if (count == 2) break;
	}

  
	if (count == 2) {
		/* 
		 * if distance is too small, we cannot garantee sample points in volume render
         * So we treat this situation as no sample points
		 *  step 1: pick an axis that is not the same
	     */
		id = 0;
		while (fabs(startpnt[id] - intpnt[id]) < VCB_EPSILON) {
			id ++;
			if (id > 2) return 0.f;
		}

		/*  step 2: intpnt 0 should be between startpnt and intpnt 1 */
		if ((startpnt[id] - intpnt[id])*(intpnt[id] - intpnt[id+3]) < 0) {
			tmpv = intpnt[0]; intpnt[0] = intpnt[3]; intpnt[3] = tmpv;
			tmpv = intpnt[1]; intpnt[1] = intpnt[4]; intpnt[4] = tmpv;
			tmpv = intpnt[2]; intpnt[2] = intpnt[5]; intpnt[5] = tmpv;
		}

		xdist = intpnt[0] - intpnt[3];
		ydist = intpnt[1] - intpnt[4];
		zdist = intpnt[2] - intpnt[5];
		dist = (float) sqrt(xdist*xdist+ydist*ydist+zdist*zdist);
		return dist;
	}
	
	return 0.f;
}

