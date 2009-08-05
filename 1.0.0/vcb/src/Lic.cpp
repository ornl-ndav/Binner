////////////////////////////////////////////////////////////
//
//   Line Integral Convolution (Lic) class
// 
//   Written by: Han-Wei Shen       hwshen@nas.nasa.gov 
//  
//   Copyright (c) 1996, 1997  MRJ/NASA Ames 
#pragma warning(disable:4244)   // double to float conversion--loss of data
#pragma warning(disable:4101)   // unreference local variable
#pragma warning(disable:4305)   // "+=" : truncation from double to float


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <memory.h>

#include "Lic.h"
#include "misc.h"

//////////////////////////////////////////
//
//
Lic::Lic() 
{
  // set default filter parameters.
  equalPower = 5;
  makeEqualTable();
}

//////////////////////////////////////////
//
//  A linear interpolation inline function
//
inline float Lic::lerp(float r, float y0, float y1)
{
return(y0 + r * (y1-y0)); 
}

//////////////////////////////////////////
//
//  Use trilinear interpolation to compute
//  the vector value at the current position
//
//
int Lic::lerp_vector(float x, float y, float z, int tidx, 
                float& u, float& v, float &w)
{
  int x1, y1, z1, i; 
  int yoffset, zoffset; 
  int off[8]; 
  float tx, ty, tz; 
  float top, bot, front, back, left, right; 

  x1 = IFLOOR(x); y1 = ydim - ICEIL(y); z1 = IFLOOR(z);
  //x1 = IFLOOR(x); y1 = IFLOOR(y); z1 = IFLOOR(z);

  // note that the ICEIL is ((int)x+1)

  if (x1 <0   || x1 >=xdim ||
      y1 <= 0 || y1 >=ydim ||
      z1 <= 0 || z1 >=zdim)
    return(0); 

  tx = x - x1; ty = y - IFLOOR(y); tz = z-z1;
  //tx = x - x1; ty = y - y1; tz = z-z1;

  float *toffset = memptr[tidx]; // get the memory pointer 

  yoffset = xdim * y1; 
  zoffset = xdim * ydim * z1;

  off[0] = zoffset + yoffset + x1; 
  off[1] = off[0] + 1; 
  off[2] = off[1] - xdim; // here is really wierd. I want to get rid of it
  off[3] = off[0] - xdim; // once we start writing the new system 

  zoffset = xdim * ydim;
  off[4] = off[0] + zoffset; 
  off[5] = off[1] + zoffset; 
  off[6] = off[2] + zoffset; 

  for (i=0; i<8; i++) off[i] *=3; 

  for (i=0; i<3; i++) {
    bot = lerp(tx, *(toffset+off[0]+i), *(toffset+off[1]+i)); 
    top = lerp(tx, *(toffset+off[3]+i), *(toffset+off[2]+i)); 
    front = lerp(ty, bot, top); 

    //bot = lerp(tx, *(toffset+off[4]+i), *(toffset+off[5]+i)); 
    //top = lerp(tx, *(toffset+off[7]+i), *(toffset+off[6]+i)); 
    //back = lerp(ty, bot, top);


    if (i==0)      u = front;//lerp (tz, front, back); 
    else if (i==1) v = front;//lerp (tz, front, back); 
    else if (i==2) w = front;//lerp (tz, front, back); 
  }
  u = -1*u; v= -1*v; w = -1*w;
  return(1); 
}



//////////////////////////////////////////////////////
//
//   normalize the input vector field
//
void Lic::normalize_vecs(float* field)
{
   int i, j, k;
   int ydimxdim = ydim*xdim; 
   int alldim = xdim*ydim*zdim;

   for (k=0; k<zdim; k++) 
     for (j = 0; j < ydim; j++) {
	 for (i = 0; i < xdim; i++) {
	    float fx, fy, fz, mag;
            int idx = ( k*ydimxdim + j*xdim + i)*3; 
	    fx = field[idx];
	    fy = field[idx+1];
      fz = field[idx+2];
	    mag = sqrt(fx*fx + fy*fy + fz*fz); 
	    if (mag != 0.0) {
	      field[idx] = (fx/mag)*scaleFactor; 
	      field[idx+1] = (fy/mag)*scaleFactor;
        field[idx+2] = (fz/mag)*scaleFactor;
	      // field[idx] = (fx/mag); 
	      // field[idx+1] = (fy/mag); 
	    }
	  }
     }
 }


double LARGE = pow(2,15)-1; 

//////////////////////////////////////////
//
//  Create a gray-level white noise image 
//  for LIC 
//
//  The sizes of noise image is 
//  xdim*ydim*zdim*pixelsize
// 
unsigned char* Lic::make_noise()
{
  double large = pow(2,31)-1; 
  unsigned char *tmp = new unsigned char[xdim*ydim*zdim]; 
  //printf("IN makenoise: %d %d %d %d\n", xdim, ydim, zdim, RAND_MAX);
  //rand(98639); 
  int l=0; 

  for (int k=0; k<zdim; k++)
    for (int j=0; j<ydim; j++) 
      for (int i=0; i<xdim; i++) {
	tmp[l++] = (int)(rand()*255./RAND_MAX); 
      }
  return(tmp); 
}

//////////////////////////////////////////////////////////
//
// An auxilary routine to find the forwarding distance t
//
float Lic::next_t(float fx, float fy, float fz,
                     float x, float y, float z)
{
float t; 

if (fx < -SIN_PARALLEL){
   t = (FLOOR(x)-x) / fx;	 // left edge 
   if (t ==0) t = -1/fx; 
} 
else if (fx > SIN_PARALLEL){
   t = (CEIL(x)-x) / fx;       // right edge
   if (t==0) t = 1/fx; 
}
else
   t = HUGE_VAL;

float ty; 	    
if (fy < -SIN_PARALLEL) {
   ty = (FLOOR(y) - y)/fy;   // bottom edge 
   if (ty ==0) ty = -1/fy; 
}
else if (fy > SIN_PARALLEL) {
   ty = (CEIL(y) - y)/fy; 	 // top edge 
   if (ty ==0) ty = 1/fy; 
}  
else ty = HUGE_VAL; 
if (ty <t ) t = ty; 

float tz; 	    
if (fz < -SIN_PARALLEL) {
   tz = (FLOOR(z) - z)/fz;   // bottom edge 
   if (tz ==0) tz = -1/fz; 
}
else if (fz > SIN_PARALLEL) {
   tz = (CEIL(z) - z)/fz; 	 // top edge 
   if (tz ==0) tz = 1/fz; 
}  
else tz = HUGE_VAL; 
if (tz <t ) t = tz; 

return(t); 
}

//////////////////////////////////////////////////////////////////////////
//
//  makeEqualTable() - creates lookup table for equalization filter
//

void Lic::makeEqualTable( )
{
  float   percent, power;
  EqualArr = new unsigned char[256];
  power     = equalPower;
  for( int i = 0; i < 128; i++ ) {
    percent = float( i ) / 127.0;
    percent = powf( percent, power );
    EqualArr[i] = ( unsigned char ) ( percent * 127.0 + 0.5 );
    EqualArr[255-i] = 255 - EqualArr[i];
  }
}

/////////////////////////////////////////////////////
//
//   Apply histogram equalization to the texture
//
void Lic::doEqualFilter( unsigned char *texture )
{	
	int index;
	for( int i = 0; i <xdim*ydim*zdim; i++ ) {
	    index = texture[i];
	    if (index > 255) {
	      printf(" continue\n"); 
	      continue; 
	    }
	    texture[i] = EqualArr[ index ];
	}
}
