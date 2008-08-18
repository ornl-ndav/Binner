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

#ifndef HILBERT_H
#define HILBERT_H


#ifndef MASK_NUM
#define MASK_NUM 1
#endif
#ifndef SIZE
#define SIZE 32
#endif
#ifndef THIRTYTWO
#define THIRTYTWO
#endif
typedef int bitmask_t [MASK_NUM];

#ifdef __cplusplus
extern "C" {
#endif

extern void init_zero(bitmask_t);
extern void hilbert_incr(unsigned int nDims, unsigned int nBits, bitmask_t coord[]);


#ifdef __cplusplus
}  /* extern C */
#endif


#endif

