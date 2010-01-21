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

#ifndef _LIBLIC_H_
#define _LIBLIC_H_


/*
 * ANSI C and POSIX includes.
 */
#include <stdlib.h>
/*#include <unistd.h>*/
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include <math.h>
/*
 * Amount of bias used to insure that we step over pixel/cell boundaries
 * when performing vector field stream line advection.
 */
#ifndef ROUND_OFF
#define ROUND_OFF	0.00001
#endif


/*
 * Default parallel edge/vector tolerance.  If an advected stream line and
 * an edge meet at an angle less than or equal to PARALLEL_EDGE, treat it
 * as an intersection of parallel lines (sic) and extend the advected stream
 * line out till it meets the next edge.
 */
#ifndef SIN_PARALLEL
#define SIN_PARALLEL	0.0523359562429	/* sin(three degrees) */
#endif
#ifndef VCB_M_PI
#   define VCB_M_PI		3.141592653589793116
#endif

#ifndef TWO_PI
#   define TWO_PI		2*VCB_M_PI
#endif


/*
 * define REGISTER to register on some machines for speed ...
 */
#ifndef REGISTER
#define REGISTER  register
#endif

/*
 * Handy macros.
 */
#ifndef SQUARE
#define SQUARE(x)	((x)*(x))
#endif
/*#ifndef SIGN
#define SIGN(x)		(((x) > 0) ? 1 : (((x) < 0) ? -1 : 0))
#endif */
#ifndef CLAMP2
#define CLAMP2(x)	(((x) > 255) ? 255 : (((x) < 0) ?  0 : (x)))
#endif


/*
 * Macro to wrap pixel coordinates in a torus like topology: 0 .. width-1.
 */
#ifndef WRAP
#define WRAP(i, width) \
{ \
    if ((i) < 0) \
    { \
	/* we'll never be off more than one width negatively */ \
	(i) += (width); \
    } \
    else if ((i) >= (width)) \
    { \
	/* we may be very far off positively ... */ \
	(i) %= (width); \
    } \
}
#endif


/*
 * Macros to take the floor and ceil of real numbers representing coordinates
 * within a vector lattice.  Since we're dealing with lattice coordinates,
 * we know that we're dealing with positive numbers, so we get to use
 * trucation to get floor.  Moreover, since we definitely won't be working
 * with vector fields with indexes larger than an int, we get to do the
 * truncation by simply casting into an int.  Finally, since we're dealing
 * with lattice coordinates, we get to use truncation + 1 to get ceil.
 */
#ifndef FLOOR
#define FLOOR(x)  ((double)(int)(x))
#endif
#ifndef CEIL
#define CEIL(x)   ((double)((int)(x) + 1))
#endif
#ifndef IFLOOR
#define IFLOOR(x) ((int)(x))
#endif
#ifndef ICEIL
#define ICEIL(x)  ((int)(x) + 1)
#endif


#endif /* _LIBLIC_H_ */
