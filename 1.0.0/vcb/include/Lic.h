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

///////////////////////////////////////////////////////
//
//   A base class for steady and unsteady flow LIC
//   Written by:  Han-Wei Shen 
//                hwshen@nas.nasa.gov
//
//

#ifndef LIC_H
#define LIC_H 

#include <stdio.h>
#include <stdlib.h> 
#include <math.h>

//////////////////////////////////////////////////
//  Constant for LIC execution 

#ifndef LIC_INTEGRAL_LEN
#define LIC_INTEGRAL_LEN     2048 // size of integral table, 
#endif
                                  // this is only for the use of Hanning filter

#ifndef LIC_FOREWARD
#define LIC_FOREWARD         0    // convole forward or backward
#endif
#ifndef LIC_BACKWARD
#define LIC_BACKWARD         1
#endif

#ifndef EULER
#define EULER                0    // integration methods 
#endif
#ifndef RK2
#define RK2                  1
#endif

#ifndef ZERO
#define ZERO                 0    // interpolation methods
#endif
#ifndef LINEAR
#define LINEAR               1
#endif



/////////////////////////////////////////////////
//
//   A base class for steady and unsteady LIC
//

class Lic{

protected: 

  float          *vectors;        // actual data (vector) array
  float**        memptr;  	  // for unsteady flows 

  unsigned char  *noise;          // white noise for LIC 
  unsigned char  *make_noise();   // routine that generate white noise 

  // linear interpolation in space and time 
  int lerp_vector(float  x, float  y, float z, int time, 
		  float& u, float& v, float & w); 

  // linear interpolation in space only 
  float lerp(float r, float y0, float y1); 

  float next_t(float, float, float, float, float, float); 

  int interp_mthd;                // interpolation method: zero or first order
  int intg_mthd;                  // integration method  : Euler or RK2

  int   doNormalize; 
  float scaleFactor; 

  int     equalPower;             // power that shapes equalization filter
  unsigned char   *EqualArr;	  // lookup table for equalization filter
  void makeEqualTable();
  void doEqualFilter(unsigned char* texture); 

public: 

  int        xdim, ydim, zdim; 	

  virtual unsigned char* get_slice(int) = 0; 
  virtual void execute() = 0; 
  virtual void set_time(float) = 0; 

  void normalize_vecs(float* field); 

  void set_Normalize(int doit, float scale) {
    doNormalize = doit;
    scaleFactor = scale; 
  }

  // specifing the data interpolation method 
  //
  void set_interp(int type) {
     if (type!= ZERO && type != LINEAR) 
     printf(" Error: interpolation method: %d currently is not supported.",type); 
     interp_mthd = type; 
  }

  // specifing the vector integration method
  // 
  void set_intg(int type) {
     if (type != RK2 && type != EULER)
     printf(" Error: integration method: %d currently is not supported.", type); 
     intg_mthd = type; 
  }

  Lic(); 

  ~Lic(){}; 

}; 

#endif

