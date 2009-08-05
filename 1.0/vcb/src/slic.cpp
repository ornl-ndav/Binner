////////////////////////////////////////////////////////////
//
//   Line Integral Convolution (LIC) class
//   Written by: Han-Wei Shen 
//   Feb. 1, 1996
//   Nov. 6, 1996 modified for unsteady flow field
//
//   Copyright (c) 1996, 1997 SCI group 
//

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

#include "slic.h"
//#include "slic_aux.cc"

/////////////////////////////////////////////////////////
//
//    Two kind of filter functions for the use of LIC
//
//

double Box(SLIC *lic, double a, double b, int speed) {
   return (b-a);
}

//////////////////////////////////////////

int check_phase(float phase, int idx, SLIC*lic){

int start = (int) (phase * LIC_INTEGRAL_LEN); 
int end = (int)((phase+0.8) * LIC_INTEGRAL_LEN); 

int limit = LIC_INTEGRAL_LEN; 

if (end >=limit)
  end = end - limit; 

if (start <=end) {
  if (idx >=start && idx <=end) return (1); 
  else return(0); 
}
else if (idx >=start || idx <=end) return(1);
else return(0); 

}

////////////////////////////////////////////////
//
int check_range(float phase, int idx, SLIC*lic){

int start = (int) (phase * LIC_INTEGRAL_LEN); 
int end = (int)((phase+0.8) * LIC_INTEGRAL_LEN); 
//int end = (int)((phase+0.3) * LIC_INTEGRAL_LEN); 

//int limit = LIC_INTEGRAL_LEN * (lic->Dye_Length/lic->Length); 
int limit = LIC_INTEGRAL_LEN; 

if (end >=limit)
  end = end - limit; 

if (start <=end) {
  if (idx >=start && idx <=end) return (1); 
  else return(0); 
}
else if (idx >=start || idx <=end) return(1);
else return(0); 

}




///////////////////////////////////////////////////////
//
//    Constructor
//
SLIC::SLIC (int xsize, int ysize, int zsize, 
          float *vecs, 
	  double         (*filter)(SLIC *, double, double, int),
	  float         len, 
	  float         freq, 
	  int            variableLength,
          int            variableSpeed, 
	  float           max_iter)
{
  printf("SLIC:SLIC\n");
  xdim = xsize; ydim = ysize;  zdim=zsize; 
  printf(" **** LIC dims = ***** %d %d %d\n", xdim, ydim, zdim);    

  noise = make_noise(); 

  /*
  printf("write into a binary file.\n"); 
  FILE *out = fopen("noise.bin", "w"); 
  fwrite(&xdim, sizeof(int), 1, out); 
  fwrite(&ydim, sizeof(int), 1, out); 
  fwrite(&zdim, sizeof(int), 1, out); 
  fwrite(noise, sizeof(unsigned char), xdim*ydim*zdim, out); 
  fclose(out); 
   */

  vectors = vecs; 

  /*
  printf("write into a binary file.\n"); 
  FILE *out = fopen("ravi.vec", "w"); 
  fwrite(&xdim, sizeof(int), 1, out); 
  fwrite(&ydim, sizeof(int), 1, out); 
  fwrite(&zdim, sizeof(int), 1, out); 
  fwrite(vectors, sizeof(float), xdim*ydim*zdim*3, out); 
  fclose(out); 
  */

  norm_vectors = vectors; 

  MAX_ITER = max_iter; 

  intg_mthd = RK2; interp_mthd = LINEAR; 

  doNormalize = 1; 
  scaleFactor = 1; 

  normalize_vecs(norm_vectors); 

  Image = new unsigned char[xdim*ydim*zdim]; 

  memptr = new float*[1]; 
  memptr[0] = vectors; 

  Filter = filter;
  Length = (len < 0.0) ? 0.0 : len; 
  VariableLength     = variableLength;
  VariableSpeed     = variableSpeed;
   
  if (VariableLength || VariableSpeed) {
    MaxLength = find_max_vec(); 
    if (MaxLength == 0.0)
      MaxLength = 1.0;
  }
  else 
    MaxLength = 0.0;

  equalPower = 5;
  makeEqualTable();

  pseudo_time = 0; 
}
//****************************************************//
////////////////////////////////////////////////////////

void SLIC::set_phase(double phase) 
{
  Phase = phase; 
}

/////////////////////////////////////////////////////

void SLIC::BuildIntegralTables (int i){

   int speed, length;
   double new_phase = (double) (2.0 * VCB_M_PI * (i / (float)MAX_ITER));
   set_phase(new_phase); 
   printf(" new phase %lf \n",Phase); 
   for (speed = 0; speed < LIC_INTEGRAL_SPEEDS; speed++) {
      double s;
      for (length = 0; length < LIC_INTEGRAL_LEN; length++) {
	 s = (double) (Length * length / (double)LIC_INTEGRAL_LEN);
	 NegTable[speed][length] = Filter(this, -s, 0.0, speed);
	  PosTable[speed][length] = Filter(this, 0.0, s, speed);
      }
    }
}
//////////////////////////////////////////////////////
//****************************************************//

void SLIC::new_data(float *vecs)
{
  vectors = vecs; 

  norm_vectors = vectors; 
  normalize_vecs(norm_vectors); 

  memptr = new float*[1]; 
  memptr[0] = vectors; 
}

/////////////////////////////////////////////////////

SLIC::~SLIC() {
  delete[] noise; 
  delete[] Image; 
}
/*
//////////////////////////////////////////////////////
//
//   normalize the input vector field
//
void SLIC::normalize_vecs(float* field)
{
   int i, j, k;
   int ydimxdim = ydim*xdim; 
   int alldim = xdim*ydim*zdim; 

   for (k=0; k<zdim; k++) 
      for (j = 0; j < ydim; j++)
	 for (i = 0; i < xdim; i++) {
	    register float fx, fy, mag;
            int idx = ( k*ydimxdim + j*xdim + i)*3; 
	    fx = field[idx];
	    fy = field[idx+1];
	    mag = sqrt(fx*fx + fy*fy); 
	    if (mag != 0.0) {
               field[idx] = fx/mag; 
               field[idx+1] = fy/mag; 
	    }
	  }
 }
*/
///////////////////////////////////////////////////
//
//  Find the maximum vector magnitude in the field
//
float SLIC::find_max_vec()
{
   register float max;
   max = 0.0;
   register int i, j, k;
   int ydimxdim = ydim*xdim; 

   for (k=0; k<zdim; k++)
      for (j=0; j<ydim; j++)
	      for (i = 0; i < xdim; i++) {
	        register float fx, fy, fz, mag;
			int idx = (k*ydimxdim + j*xdim + i)*3; 
	        fx = vectors[idx];
	        fy = vectors[idx+1];
	        mag = sqrt(fx*fx + fy*fy); 
	        if (mag > max) 
	        max = mag;
	      }

   return max;
}

///////////////////////////////////////////////////
//
//  Simple functions to change parameters of LIC
//

void SLIC::set_length(float len)
{
  Length  = (len < 0.0) ? 0.0 : len; 
}

void SLIC::set_time(float t) {
  if (t >=MAX_ITER) pseudo_time = MAX_ITER - 1; 
  else if (t < 0) pseudo_time = 0; 
  else pseudo_time = t; 
}

///////////////////////////////////////////////////////
//
// The LIC driver routine. This routine will be called 
// several times to produce a sequence of images with 
// different LIC phase. Use for animation.
//
//
void SLIC::execute()
{
   REGISTER int      i, j, k; 
   float valFore, valBack; 
   float ForewardKernelArea, BackwardKernelArea;
   float results; 
   
char fn[6];


   BuildIntegralTables(pseudo_time); 

   for (k=0; k<zdim; k++) {
   
     for (j=0; j<ydim; j++) { 
      for (i=0; i<xdim; i++) {
		convolve(i, j, k, LIC_FOREWARD, valFore, 
			ForewardKernelArea, pseudo_time); 

		convolve(i, j, k, LIC_BACKWARD, valBack, 
			BackwardKernelArea, pseudo_time); 

		results = (valFore + valBack)
			/ (ForewardKernelArea + BackwardKernelArea);

		int idx = k*xdim*ydim+j*xdim+i; 
		Image[idx] = CLAMP2(results); 
	  }
	 }

	 //sprintf(fn, "lic%d", k);
	 //slice_to_file(k, fn);
	 
	}
	   
   doEqualFilter(Image);

  //writing out LIC volume
  printf("IN SLIC execute: write into a binary file.\n"); 
  FILE *out = fopen("noise.bin", "wb"); 
  fwrite(&xdim, sizeof(int), 1, out); 
  fwrite(&ydim, sizeof(int), 1, out); 
  fwrite(&zdim, sizeof(int), 1, out); 
  fwrite(Image, sizeof(unsigned char), xdim*ydim*zdim, out); 
  fclose(out); 

   printf("Computing Image %d Successfully!!! \n", pseudo_time);
}

///////////////////////////////////////////////////////
//
//   LIC convolution for point (i,j,k)
//
//
//      Establish length, L, of convolution and which ``speed''
//      version of the
//      filter kernel to use.  The default L is the value
//      set by the user.
//      For VariableLength convolutions, L is scaled by 
//      the magnitude of the
//      vector.  The default speed is to use the maximum 
//      ``speed'' version of
//      the filter kernel.  For VariableSpeed convolutions, 
//      speed is scaled
//      by the magnitude of the vector.
//
//      Establish filter kernel integral table to use based 
//      on direction and
//      speed.  Also determine index scaling for values of
//      s, 0 <= s <= L,
//      into the table.  Note that the scaling is performed
//      relative to L,
//      Scaling relative to L means that the filter is effectively
//      dilated to
//      match the convolution length even when performing
//      VariableLength convolution.
//


///////////////////////////////////////////////////////////////
//
//  A routine to get the vector data, given a point location
//  Perform linear interpolation if necessary. 
//
int SLIC::get_vector(float x, float y, float z,
                     int  fi, int fj, int fk,
                     float& fx, float& fy, float & fz)
{
  float u0, v0, w0, u1, v1, w1; 

  if (x >= xdim-1 || y >= ydim -1 || z >= zdim -1)
    return(0); 

  if (fi <0 || fi >=xdim ||
      fj <0 || fj >=ydim || 
      fk <0 || fk >=zdim)
    return(0); 

  if (interp_mthd == LINEAR) {
    int r1 = lerp_vector(x,y,z, 0, u0, v0, w0);  // steady flow: t = 0
    if (r1 == 0) return(0); 
    fx = u0; 
    fy = v0; 
    fz = w0;
    return(1); 
  }
  // *** Zero-Order interpolation
  else if (interp_mthd == ZERO) {
    int offset =  fk*xdim*ydim+fj*xdim + fi; 
    int idx  = offset * 3; 
    fx = vectors[idx]; 
    fy = vectors[idx+1]; 
    fz = vectors[idx+2];
    return(1); 
  }

  return(0);
}

/////////////////////////////////////////////////

#ifndef _max
#define _max(a,b) a>b?a:b 
#endif

void SLIC::convolve (int i, int j, int k,
		     int direction, float&  aIntegral,
		     float&  KernelArea, float iter)
{
   register float L;		// pos/neg length of kernel 
   register float s, sp;	// parametric distance along kernel 
   register float fx, fy, fz;	// vector field values 
   register float fx1, fy1, fz1;     // vector field values 
   float          wp, owt;      // integral corresponding to sp 
   float          aSum, aV;
   int            speed;	// integral table speed index 
   float          is;	        // integral table and index scale 
   int            LoopCount = 0;	// main loop iteration count 
   double *itab; 

   aSum = 0.0;    sp = 0.0;
   L = Length;
   speed = LIC_INTEGRAL_SPEEDS - 1;

   if (VariableLength || VariableSpeed) {
      register float fx, fy, norm;
      int idx = (k*ydim*xdim + j*xdim + i)*3; 

      fx = vectors[idx]; fy = vectors[idx+1]; fz = vectors[idx+2];
      norm = sqrt(fx*fx + fy*fy + fz*fz) / MaxLength;

      if (VariableLength){
         float factor; 
         if (norm <0.25) factor = 0.5; 
         else if (norm <0.5) factor = 1.0;
         else if (norm <0.75) factor = 2.0;
         else factor = 4.0;
	 L *= factor; 
      }
      if (VariableSpeed)
	 speed *= norm;
    }

   //---------------------------------------
   // for ripple filter 

   if (direction == LIC_FOREWARD) 
     itab = &PosTable[speed][0]; 
   else 
     itab = &NegTable[speed][0]; 

   //---------------------------------------

   is = (float)(LIC_INTEGRAL_LEN - 1) / L;

   LoopCount = 0;

// Only perform LIC if L is greater than zero.  Zero lengths
// can result from bad user input or magnitude based lengths.

   if (L > 0) {
      register int fi, fj, fk;		// vector field index 
      register float x, y, z; 		

      // current cartesian location in the vector field 

      fi = i; fj = j; fk = k;
      x =  i + 0.5; y =  ydim - (j + 0.5); z = k + 0.5; //TRICKY
      //x =  i + 0.5; y = j + 0.5; z = k + 0.5;
      wp = 0.0; owt = 0.0;

      do {
	 register float t;	
         get_vector(x,y,z, fi, fj, fk, fx, fy, fz); 
 
         if (direction == LIC_BACKWARD) {
	   fx = -fx; fy = -fy, fz = -fz;
         }
	 if (fx == 0.0 && fy == 0.0 && fz == 0.0)
	    break;

         // find the stepsize for going to the next cell
         t= next_t(fx, fy, fz, x, y, z); 

         // ***** RK-2 Integration ********
         if (intg_mthd == RK2) {
	   float _x, _y, _z; 

	   _x = x + fx*t; _y = y + fy*t; _z = z + fz * t;

           int _fi, _fj, _fk; 
	   
	   _fi = IFLOOR(_x); _fj = ydim - ICEIL(_y); _fk = IFLOOR(_z); 
     //_fi = IFLOOR(_x); _fj = IFLOOR(_y); _fk = IFLOOR(_z);

	   get_vector(_x,_y,_z,_fi, _fj,_fk, fx1, fy1,fz1); 

	   if (direction == LIC_BACKWARD) {
	     fx1 = -fx1; fy1 = -fy1; fz1 = -fz1;
	   }
	   if (fx1 == 0.0 && fy1 == 0.0 && fz1 == 0.0)
	     break;

	 //Compute the new location using RK-2
	   float dx = (fx+fx1)/2.0 * t; 
	   float dy = (fy+fy1)/2.0 * t; 
     float dz = (fz+fz1)/2.0 * t;

	   x= x + dx;  y= y + dy; z= z + dz;

	   //	   float sp_incr = _max(dx,_max(dy, dz)); 
	   float sp_incr = t; 

	   // s and sp represent a monotonically moving convolution window 
	   s  = sp;  sp = sp + sp_incr;
	 }
         // ***** Euler Integration ********
	 else {

	   t += ROUND_OFF;
	   float dx = fx * t; 
	   float dy = fy * t; 
     float dz = fz * t;

	   // Compute the cell we just stepped into 
	   x = x + dx; y = y + dy; z = z + dz;

	   float sp_incr = t; 

	   // s and sp represent a monotonically moving convolution window 
	   s  = sp;  sp = sp + sp_incr;
	 }
         // Make sure we don't exceed the kernel width
	 if (sp > L) {
	    sp = L;
	    t  =  sp - s;
	 }
         // Grab the input pixel corresponding to the current cell and
         // integrate its va lue over the convolution kernel from s to sp.
	 {
	    REGISTER int ii, ij, ik; 
	    
	    /* toriodally wrap input image coordinates */
	    ii = fi; ij = fj; ik = fk;
	    WRAP(ii, xdim); WRAP(ij, ydim); WRAP(ik, zdim);
	    
	    /* Get the input image value for this cell */
            int idx = ik*xdim*ydim+ij*xdim+ii; 
	    aV = noise[idx];
	 }
	 
	 // integrate over the convolution kernel between s and sp
	 float          wt;
	 REGISTER float dw;

	 /* ----------------------------------
            using ripple filter              
	 */
	 wt = (float)itab[(int)(sp * is)];
	 dw = wt - owt; 
         owt = wt; 
         wp += dw; 
         aSum  += (aV * dw); 

	 /* ---------------------------------  
         //   my old stuff, using box stuff         
	 wt = sp; 
	 dw = wt - owt;
	 owt= wt; 
	 
	 if (check_range(iter/MAX_ITER, 
			 ((int)(sp*is)),this)) {   
	   wp += dw;
	   aSum += (aV * dw);
	 }
	 ------------------------------------*/
	 LoopCount++;
	 
	 // break out of the loop if we step out of the field 
	 if (x < 0.0 || y < 0.0)
	    break;

	 fi = IFLOOR(x); fj = ydim - ICEIL(y); fk = IFLOOR(z);
   //fi = IFLOOR(x); fj = IFLOOR(y); fk = IFLOOR(z);
	 
	 if (fi >= xdim || fi<0)
	   break;

	 if (fj < 0 || fj >=ydim )
	   break; 

   if (fk < 0 || fk >=zdim )
	   break; 

    } while (sp < L && LoopCount <= 3*L);

    if (LoopCount > 0 && L > 0)
    {
      aIntegral  = aSum; 
      KernelArea = wp; 
    }
    else
      {
	aIntegral = 0.0; 
	KernelArea = 1;
      } 
   }
}

////////////////////////////////////////////////////
//
//    out a slice of the LIC result at the current 
//    pseudo time
//
unsigned char* SLIC::get_slice(int slice)
{
   int size = xdim*ydim*zdim; 
   int bufsize = xdim*ydim*1; 
   unsigned char* buf;

   buf = new unsigned char[bufsize]; 
   for (int i=0; i<xdim; i++) 
     for (int j =0; j<ydim; j++) {
       int idx = j*xdim+i; 
       int slice_idx = slice*xdim*ydim + idx; 
       buf[idx] = Image[slice_idx]; 
   }
   return(buf); 
}

void WriteImage(unsigned short * frameBuffImg, 
		char * fileNameRoot, 
		int w, int h)
{
  FILE *fp;
  int i,j, k;
  char tmpStr[300];

  sprintf(tmpStr,"%s.ppm",fileNameRoot);

  if(!(fp=fopen(tmpStr,"w"))) 
    {
      printf("Cannot write %s\n",tmpStr);
      return;
    }

  fprintf(fp,"P6\n%d %d\n255\n",w,h);

  for (j = 0; j < h; j ++)
    for (i = 0; i < w; i ++)   
      {
	unsigned char tmp = (unsigned char)(frameBuffImg[j*w+i]);
	for (k = 0; k < 3; k ++)
	  fwrite(&tmp,1,1,fp);
      }
  
  fclose(fp);
}

void SLIC::slice_to_file(int slice, char* fname)
{
   int size = xdim*ydim*zdim; 
   int bufsize = xdim*ydim*1; 
   unsigned short* buf;

   buf = new unsigned short[bufsize]; 
   for (int i=0; i<xdim; i++) 
     for (int j =0; j<ydim; j++) {
       int idx = j*xdim+i; 
       int slice_idx = slice*xdim*ydim + idx; 
       buf[idx] = (unsigned short) Image[slice_idx]; 
   }

     //printf("IN slice to file: %d %d %d\n", xdim,ydim,zdim);
   WriteImage(buf,fname,xdim,ydim);

   // write to a rgb image file
   /*
   IMAGE *iris = iopen(fname, "w", RLE(1), 3, xdim, ydim, 3); 
   for (int j=0; j<ydim; j++){
     putrow(iris, buf, j, 0); 
     putrow(iris, buf, j, 1); 
     putrow(iris, buf, j, 2); 
     buf+= xdim; 
   }
   iclose(iris); 
   */
}

















