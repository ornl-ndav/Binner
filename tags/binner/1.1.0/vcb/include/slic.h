/****************************************************************************
 *   Copyright (C) 2004 by Han-Wei Shen                                     *
 *   hwshen@nas.nasa.gov													*
 *                                                                          *
 *   This library is free software; you can redistribute it and/or          *
 *   modify it under the terms of the GNU Lesser General Public             *
 *   License as published by the Free Software Foundation; either           *
 *   version 2.1 of the License, or (at your option) any later version.     *
 *                                                                          *
 *   This library is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with this library; if not, write to the                  *
 *   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,           *
 *   Boston, MA  02110-1301  USA                                            *
 ****************************************************************************/

//
//   Line Integral Convolution (LIC) class
//   Written by: Han-Wei Shen 
//
//   MRJ/NASA Ames Research Center 
//
//
#ifndef SLIC_H 
#define SLIC_H

#include <math.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <memory.h>

#include "Lic.h"
#include "misc.h"

#ifndef LIC_INTEGRAL_SPEEDS
#define LIC_INTEGRAL_SPEEDS  20   // the max speed scale for 
#endif
                                  // implementation of variable 
                                  // speed lic


//////////////////////////////////////////////
//
//           LIC Class declaration
//
class SLIC: public Lic {

private:

 double  NegTable[LIC_INTEGRAL_SPEEDS][LIC_INTEGRAL_LEN];
 double  PosTable[LIC_INTEGRAL_SPEEDS][LIC_INTEGRAL_LEN];

 void    BuildIntegralTables(int iter); 

 void    set_phase(double); 
 
 float   MaxLength; 
 int     VariableLength;
 int     VariableSpeed; 

 double  (*Filter) (SLIC*, double, double, int);

 float    *norm_vectors;     // normalized vector, if necessary 

 float  find_max_vec(); 

 void convolve (int i, int j, int k, int dir,
		    float&  aIntegral,
		    float&  KernelArea,
		    float iter); 

int get_vector(float x, float y, float z,
                int fi, int fj, int fk,
		float& fx, float& fy, float & fz); 


int pseudo_time; 

float MAX_ITER; 

public:

//
// If you just want to the simplest LIC, 
// Frequency, VariableLength, VariableSpeed can be just set to zero
// and max_iter can be set to be 1
//
 SLIC (int xdim, int ydim, int zdim, 
      float* vecs, 
      double (*Filter)(SLIC *, double, double, int),
      float Length, float Frenquency,
      int  VariableLength, int VariableSpeed, float max_iter); 

 float   Length;            // Convolution kernel size 

 unsigned char  *Image; 
 unsigned char * getImage() { return Image;}

 virtual void execute();    // compute the lic for a 
                            // particular animation iteration

 virtual unsigned char* get_slice(int slice); 
 void slice_to_file(int slice, char* fname); 

 virtual void set_time(float t);  

 void new_data(float *vec); 

 void set_length(float Length); 
 unsigned char * get_image() { return Image; }

 double Phase;   // I don't like the idea of its being public 

 ~SLIC();

}; 


#endif

