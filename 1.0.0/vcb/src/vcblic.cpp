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

#include "stdlib.h"
#include "vcblic.h"
#include "vcbutils.h"
#include "slic.h"
#include "slic_aux.h"

#define CONV_LENGTH 20

void vcbGenLicTex3D (float * vectors, unsigned char * lictex, int xsize, int ysize, int zsize)
{
	SLIC *lic; // steady lic object

	lic = new SLIC(xsize, ysize, zsize, vectors, Ripple, 20, 0,0,0,1);
	printf("out of constructor\n");

	lic->set_time(0); 
	lic->execute(); 
	
	//lic->slice_to_file(0, "lic0"); 
    //lic->slice_to_file(1, "lic1"); 
    //lic->slice_to_file(2, "lic2"); 

	memcpy(lictex,lic->get_image(), xsize*ysize*zsize*sizeof(unsigned char));
}

void vcbGenLicTex2D (float * vectors, unsigned char * lictex, int xsize, int ysize)
{
	float* pvectors; 
	int i, zsize = 3;
	int nbytes_perslice;
 
 	SLIC *lic; // steady lic object

	nbytes_perslice = xsize * ysize * sizeof(float) * 3;
	
	/* make two padding slices, one above and one below */
	pvectors = (float *) malloc(nbytes_perslice * zsize);	
	for (i = 0; i < xsize*ysize; i ++) {
		pvectors[i*3]   = vectors[i*2];
		pvectors[i*3+1] = vectors[i*2+1];
		pvectors[i*3+2] = 0.f;
    }
	
	memcpy(&pvectors[xsize*ysize*3], pvectors, nbytes_perslice);
	memcpy(&pvectors[2*xsize*ysize*3], pvectors, nbytes_perslice);

	lic = new SLIC(xsize, ysize, zsize, pvectors, Ripple, 20, 0,0,0,1);

	lic->set_time(0); 
	lic->execute(); 
	memcpy(lictex,
		   lic->get_image()+xsize*ysize, 
		   xsize*ysize*sizeof(unsigned char));

	free(pvectors);
}
