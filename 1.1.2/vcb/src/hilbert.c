/***************************************************************************
 *   Copyright (C) 2004  Jian Huang                                        *
 *   seelab@cs.utk.edu                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "hilbert.h"

void copy_bitmask(bitmask_t source, bitmask_t dest) {
	int i;
	for (i=0; i<MASK_NUM; i++)
		dest[i] = source[i];
}

void bit_move_right_one(bitmask_t oribits) {
	long temp;
	bitmask_t result;
	int  i;

	for (i=0; i<(MASK_NUM-1); i++) {
		temp = oribits[i+1];
		temp = temp << 31;
		result[i] = oribits[i] >> 1;
		result[i] = result[i] | temp;
	}
	i = MASK_NUM-1; /* by wenxiang huang to be sure. */
    result[i] = oribits[i] >> 1;

	copy_bitmask(result, oribits);
}

void bit_move_right_ones(bitmask_t oribits, unsigned int shiftnum) {
	unsigned  int i;

	for (i=0; i<shiftnum; i++)
		bit_move_right_one(oribits);	
}

void bit_move_left_one(bitmask_t oribits) {
	long temp;
	bitmask_t result;
	int  i;

	result[MASK_NUM-1] = oribits[MASK_NUM-1] << 1;
	for (i=(MASK_NUM-2); i>=0; i--) {
		temp = oribits[i];
		temp = temp >> 31;
		result[i+1] = result[i+1] | temp;
		result[i] = oribits[i] << 1;		
	}
	copy_bitmask(result, oribits);	
}

void bit_move_left_ones(bitmask_t oribits, unsigned int shiftnum) {
	unsigned int i;

	for (i=0; i<shiftnum; i++)
		bit_move_left_one(oribits);
}

int minus_op(bitmask_t temp1,int count)
{
	int i;
	long temp = 1;
	long judge = 1;
	int carry = 0;
	
	/* int size = sizeof(temp1[count])*8;*/
	for(i=0;i<SIZE;i++)
	{	
		temp1[count] ^=temp;
		if((temp1[count]&judge) != 0)
		{
		  temp = temp << 1;
		  judge = judge << 1;
		}
		else
			return 0;
	}
	return 1;
}

void minus_one(bitmask_t oribits) {
	int i;

	for (i=0; i<MASK_NUM; i++) {
		if(!minus_op(oribits,i))return;
		}

/*	for (i=0; i<MASK_NUM; i++) {
		if (oribits[i] > 1) {
			oribits[i] = oribits[i]-1;
			return;
		} else
		  oribits[i] = oribits[i]-1;
	}*/
}

int add_op(bitmask_t temp1,int count)
{
	long temp = 1;
	long judge = 1;
	int carry = 0, i;
	
	/*int size = sizeof(temp1[count])*8;*/
	for(i=0;i<SIZE;i++)
	{	
		temp1[count] ^=temp;
		if((temp1[count]&judge) == 0)
		{
		  temp = temp << 1;
		  judge = judge << 1;
		}
		else
			return 0;
	}
	return 1;
}

void add_one(bitmask_t oribits) {
	int i;
/*
	for (i=0; i<MASK_NUM; i++) {
		oribits[i] = oribits[i] + 1;
		if (oribits[i] > 0) {
			return;
		}
	}*/

	for (i=0; i<MASK_NUM; i++) {
		if(!add_op(oribits,i))return;
		}
}


void ones(unsigned int k, bitmask_t result) {
	int i;

	for (i=0; i<MASK_NUM; i++)
		result[i] = 0;
	result[0] = 2;

	bit_move_left_ones(result, k-1);
	minus_one(result);
}

void bit_and(bitmask_t w1, bitmask_t w2, bitmask_t result) {
	int i;

	for (i=0; i<MASK_NUM; i++)
		result[i] = w1[i] & w2[i];
}

void bit_or(bitmask_t w1, bitmask_t w2, bitmask_t result) {
	int i;

	for (i=0; i<MASK_NUM; i++)
		result[i] = w1[i] | w2[i];

}

void bit_xor(bitmask_t w1, bitmask_t w2, bitmask_t result) {
	int i;

	for (i=0; i<MASK_NUM; i++)
		result[i] = w1[i] ^ w2[i];
}


void rdbit(bitmask_t w,unsigned k, bitmask_t result) {
	bitmask_t temp;
	int i;

	/* result = w;  written by gao commented by wenxiang huang */
	copy_bitmask(w,result);
	for (i=0; i<MASK_NUM; i++)
		temp[i] = 0;
	temp[0] = 1;

	bit_move_right_ones(result, k);
	bit_and(result, temp, result);
}


/*((((arg) >> (nRots)) | ((arg) << ((nDims)-(nRots)))) & ones(bitmask_t,nDims))*/
void rotateRight(bitmask_t arg, unsigned nRots, unsigned int nDims, bitmask_t result) { 
	bitmask_t temp1;
	bitmask_t temp2;
	bitmask_t temp3;

	copy_bitmask(arg, temp1);
	copy_bitmask(arg, temp2);
	ones(nDims, temp3);

	bit_move_right_ones(temp1, nRots);
	bit_move_left_ones(temp2, nDims-nRots);
	bit_or(temp1, temp2, result);
  bit_and(result, temp3, result);
}

/*((((arg) << (nRots)) | ((arg) >> ((nDims)-(nRots)))) & ones(bitmask_t,nDims)) */
void rotateLeft(bitmask_t arg, unsigned nRots, unsigned int nDims, bitmask_t result) {                                 
	bitmask_t temp1;
	bitmask_t temp2;
	bitmask_t temp3;

	copy_bitmask(arg, temp1);
	copy_bitmask(arg, temp2);
	ones(nDims, temp3);

	bit_move_left_ones(temp1, nRots);
	bit_move_right_ones(temp2, nDims-nRots);
	bit_or(temp1, temp2, result);
	bit_and(result, temp3, result);
}

int empty(bitmask_t arg) {
	int flag = 1, i;
	for (i=0; i<MASK_NUM; i++) {
		if (arg[i]!=0)
			return 0;
	}
	return 1;
}

/* -arg */
void bit_reverse(bitmask_t arg) {
	int i;
	long temp;

#ifdef EIGHT
		temp = 0xff;
#endif

#ifdef SIXTEEN
		temp = 0xffff;
#endif

#ifdef THIRTYTWO
		temp = 0xffffffff;
#endif

	for (i=0; i<MASK_NUM; i++) {
		arg[i] ^= temp;
	}
	add_one(arg);
}

void adjust_rotation(unsigned *rotation, unsigned int nDims, bitmask_t bits, bitmask_t nd1Ones) {
	bitmask_t temp;
	do {
		/* bits &= -bits & nd1Ones; */
		copy_bitmask(bits, temp);
		bit_reverse(temp);
		bit_and(temp, nd1Ones, temp);
		bit_and(temp, bits, bits);
    
		while (!empty(bits)) {
			bit_move_right_one(bits); /* bits >>= 1; */
			*rotation = *rotation + 1;                                      
		}
    if ( ++(*rotation) >= nDims )                                        
        (*rotation) -= nDims;                                              
	} while (0);
}

void init_one(bitmask_t temp) {
	int i;

	for (i=0; i<MASK_NUM; i++)
		temp[i] = 0;
	temp[0] = 1;
}

void init_zero(bitmask_t temp) {
	int i;

	for (i=0; i<MASK_NUM; i++)
		temp[i] = 0;
}

int not_equal(bitmask_t w1, bitmask_t w2)  {
	int i;
	for (i=0; i<MASK_NUM; i++) {
		if (w1[i] != w2[i])
			return 1;
	}
	return 0;
}


/*****************************************************************
 * hilbert_incr
 * 
 * Advance from one point to its successor on a Hilbert curve
 * Inputs:
 *  nDims:      Number of coordinates.
 *  nBits:      Number of bits/coordinate.
 *  coord:      Array of nDims nBits-bit coordinates.
 * Output:
 *  coord:      Next point on Hilbert curve
 * Assumptions:
 *      nBits <= (sizeof bitmask_t) * (bits_per_byte)
 */

void hilbert_incr(unsigned int nDims, unsigned int nBits, bitmask_t coord[])
{
  bitmask_t one;
  bitmask_t ndOnes;
  bitmask_t nd1Ones;
  unsigned b, d;
  unsigned rotation = 0;
  bitmask_t reflection;
  bitmask_t index;
  unsigned rb = nBits-1;
  bitmask_t rd;
	bitmask_t temp;
	bitmask_t temp_hwx;
	bitmask_t bits;
	int       i;

  init_one(one);
	ones(nDims, ndOnes);
	copy_bitmask(ndOnes, nd1Ones);
	bit_move_right_one(nd1Ones);
	init_zero(reflection);
	init_zero(index);
	copy_bitmask(ndOnes, rd);


  for (b = nBits; b--;)
    {
      copy_bitmask(reflection, bits);
      init_zero(reflection);
      for (d = 0; d < nDims; ++d) {
				copy_bitmask(coord[d], temp_hwx);
				rdbit(temp_hwx, b, temp);
				bit_move_left_ones(temp, d);
				bit_or(reflection, temp, reflection); /*reflection |= rdbit(coord[d], b) << d;*/
			}

      bit_xor(bits, reflection, bits);
      rotateRight(bits, rotation, nDims, bits);
      bit_xor(index, bits, index);
      for (d = 1; d < nDims; d *= 2) {
				copy_bitmask(index, temp);
				bit_move_right_ones(temp, d);
        bit_xor(index, temp, index); /*index ^= index >> d;*/
			}

			if (not_equal(index, ndOnes)) 
      /*if (index++ != ndOnes)*/
      {
				  add_one(index);
					rb = b;
					copy_bitmask(index, temp);
					bit_reverse(temp);
          bit_and(index, temp, rd);  /*rd = index & -index;*/
          rotateLeft(rd, rotation, nDims, rd);
          
      } else
					add_one(index);

      bit_and(index, one, index);  /*index &= 1;*/
      bit_move_left_ones(index, nDims-1); /*index <<= nDims-1;*/

			copy_bitmask(one, temp);
			bit_move_left_ones(temp, rotation);
      bit_xor(reflection, temp, reflection); /*reflection ^= one << rotation;*/
      adjust_rotation(&rotation,nDims,bits, nd1Ones);
    }

    d = 0;
	rdbit(rd, d, temp);
  for (d = 0; empty(temp) /*!rdbit(rd, d)*/; ++d) 
	{
		rdbit(rd, d, temp);
	}
    if(d>0)
		d--;

	for (i=0; i<MASK_NUM; i++)
		temp[i] = 0;
	temp[0] = 2;

	bit_move_left_ones(temp, rb);
	minus_one(temp);
	bit_xor(coord[d], temp, coord[d]);
  /*coord[d] ^= (2 << rb) - 1;*/
}

