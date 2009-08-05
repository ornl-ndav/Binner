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

void zcurve_incr(unsigned int nDims, unsigned int nBits, unsigned char bits[], int coord[]) 
{
	int i;
	int j;
	int pow2 = 1;
	
	for (i=nBits-1; i>=0; i--) {
		if (bits[i] == 0) {
			bits[i] = 1;
			break;
		}
    
		bits[i] = 0;
	}

	for (i=0; i<(int)nDims; i++)
		coord[i] = 0;

	i = nBits-1;
	while (i>0) {
		for (j=nDims-1; j>=0; j--) {
			coord[j] += bits[i]*pow2;
			i--;
		}
		pow2 *= 2;
	}
}

