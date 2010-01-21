/****************************************************************************
 *   Copyright (C) 2004  by Jian Huang										*
 *	 seelab@cs.utk.edu														*
 *																			*
 *   This library is free software; you can redistribute it and/or			*
 *   modify it under the terms of the GNU Lesser General Public				*
 *   License as published by the Free Software Foundation; either			*
 *   version 2.1 of the License, or (at your option) any later version.		*
 *																			*
 *   This library is distributed in the hope that it will be useful,		*
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of			*
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU		*
 *   Lesser General Public License for more details.						*
 *																			*
 *   You should have received a copy of the GNU Lesser General Public		*
 *   License along with this library; if not, write to the					*
 *   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,			*
 *   Boston, MA  02110-1301  USA											*
 ****************************************************************************/

#ifndef INTERSECT_H
#define INTERSECT_H


#ifdef __cplusplus
extern "C" {
#endif

int intersect_triangle(float orig[3], float dir[3],float vert0[3], float vert1[3], float vert2[3],float *t, float *u, float *v);
int ray_triangle_intersection(float *startpnt, float *dir, float *ver1, float *ver2, float *ver3, float * intpnt);
int ray_polygon_intersection(float *startpnt, float *dir, float * verlist[3], int vernum, float * intpnt);
float ray_block_intersection(float *startpnt, float *dir, float * bound, float * intpnt);


#ifdef __cplusplus
}  /* extern C */
#endif

#endif

