/****************************************************************************
 *   Copyright (C) 2004  by Jian Huang                                      *
 *   seelab@cs.utk.edu                                                      *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "vcbutils.h"
#include "vcbvolren.h"
#include "vcblinalg.h"
#include "intersect.h"
#include "vcbimage.h"

static void * fbuffer = NULL;
static float fsumred, fsumgreen, fsumblue, fsumalpha;
static int  fbuf_w, fbuf_h;
static void * sample = NULL;
static int  offsets[8];

void init_ray_o (void)
{
	fsumalpha = 0.f;
}

void init_ray_a (void)
{
	fsumalpha = 0.f;
	fsumred   = 0.f;
	fsumgreen = 0.f;
	fsumblue  = 0.f;
}

void recons_sample3ub(void * sample, float * coord, void * data, int nattribs, int * dsizes)
{
	int   i,x, y, z, id0, id1, id2, id3, off;
	float r, s, t, u, v, w, fac;
	unsigned char * voxels = (unsigned char *) data;
	char *          grads  = (char *) data;
	unsigned char * sam = (unsigned char *) sample;
	static float fsam0, fsam1, fsam2, fsam3;
	static float factors[8];

#ifndef rcast_RECONMACRO
#define rcast_RECONMACRO                        \
	x = (int) coord[0];                         \
	y = (int) coord[1];                         \
	z = (int) coord[2];                         \
                                                \
	r = coord[0] - x;                           \
	s = coord[1] - y;                           \
	t = coord[2] - z;                           \
                                                \
	id0 = vcbid3(x,y,z,dsizes,0,nattribs);       \
	u = 1.f - r;                                \
	v = 1.f - s;                                \
	w = 1.f - t;                                \
                                                \
	fsam0 = fsam1 = fsam2 = fsam3 = 0.f;        \
	id1 = id0 + 1; id2 = id0 + 2; id3 = id0+3;  \
	i = 0;                                      \
                                                \
	factors[0] = u*v*w;                         \
	factors[1] = u*v*t;                         \
	factors[2] = u*s*w;                         \
	factors[3] = u*s*t;                         \
	factors[4] = r*v*w;                         \
	factors[5] = r*v*t;                         \
	factors[6] = r*s*w;                         \
	factors[7] = r*s*t;                         \
                                                \
	while (i < 8) {                             \
		fac = factors[i];                       \
		off = offsets[i];                       \
		i ++;                                   \
		fsam0 += fac*voxels[id0 + off];         \
		fsam1 += fac*grads[id1 + off];          \
		fsam2 += fac*grads[id2 + off];          \
		fsam3 += fac*grads[id3 + off];          \
	}
#endif

	rcast_RECONMACRO;

	sam[0] = (unsigned char)(fsam0 + 0.5f);
	sam[1] = (unsigned char)((char)(fsam1 + 0.5f));
	sam[2] = (unsigned char)((char)(fsam2 + 0.5f));
	sam[3] = (unsigned char)((char)(fsam3 + 0.5f));
}

void recons_sample3s(void * sample, float * coord, void * data, int nattribs, int * dsizes)
{
	int   i,x, y, z, id0, id1, id2, id3, off;
	float r, s, t, u, v, w, fac;
	short * voxels = (short *) data;
	short * grads  = (short *) data;
	short * sam = (short *) sample;
	static float fsam0, fsam1, fsam2, fsam3;
	static float factors[8];

	rcast_RECONMACRO;

	sam[0] = (short)(fsam0 + 0.5f);
	sam[1] = (short)(fsam1 + 0.5f);
	sam[2] = (short)(fsam2 + 0.5f);
	sam[3] = (short)(fsam3 + 0.5f);
}

void recons_sample3i(void * sample, float * coord, void * data, int nattribs, int * dsizes)
{
	int   i,x, y, z, id0, id1, id2, id3, off;
	float r, s, t, u, v, w, fac;
	int * voxels = (int *) data;
	int * grads  = (int *) data;
	int * sam = (int *) sample;
	static float fsam0, fsam1, fsam2, fsam3;
	static float factors[8];

	rcast_RECONMACRO;

	sam[0] = (int)(fsam0 + 0.5f);
	sam[1] = (int)(fsam1 + 0.5f);
	sam[2] = (int)(fsam2 + 0.5f);
	sam[3] = (int)(fsam3 + 0.5f);
}

void recons_sample3f(void * sample, float * coord, void * data, int nattribs, int * dsizes)
{
	int   i,x, y, z, id0, id1, id2, id3, off;
	float r, s, t, u, v, w, fac;
	float * voxels = (float *) data;
	float * grads  = (float *) data;
	float * sam = (float *) sample;
	static float fsam0, fsam1, fsam2, fsam3;
	static float factors[8];

	rcast_RECONMACRO;

	sam[0] = fsam0;
	sam[1] = fsam1;
	sam[2] = fsam2;
	sam[3] = fsam3;
}

float ipow(float x, int n)
{
	int i;
	float result = 1.f, factor = x;
	
	/* loop through bits of n to give O(log(n)) time bound */
	for(i=1; i<= n; i<<= 1) {
		if( i & n) result *= factor;   /* i is part of n's base 2 representation */
		factor *= factor;              /* factor is now X^( log(i)+1) */
    }

	return result;
}

void shadesample_b(VCB_volren volr, void * gradient, float * rgba)
{
	float dot, total, spec=0.f, fgrad[3];
	char * grad = ((char *) gradient) + 1;

	fgrad[0] = grad[0]; fgrad[1] = grad[1]; fgrad[2] = grad[2];
	la_normalize3(fgrad);

	total = volr->ambient[0];
	dot = fgrad[0] * volr->lvec[0] + fgrad[1] * volr->lvec[1] + fgrad[2] * volr->lvec[2];
	if (dot > 0)
		total += dot * volr->diff[0];

	dot = fgrad[0] * volr->half[0] + fgrad[1] * volr->half[1] + fgrad[2] * volr->half[2];
	if (dot > 0)
		spec = (dot > 0) * ipow(dot, 40) * volr->spec[0];
		
	total += spec;

	rgba[0] = rgba[0] * total; 
	rgba[1] = rgba[1] * total; 
	rgba[2] = rgba[2] * total;
}

void shadesample_s(VCB_volren volr, void * gradient, float * rgba)
{
	float dot, total, spec=0.f, fgrad[3];
	short * grad = ((short *) gradient) + 1;

	fgrad[0] = grad[0]; fgrad[1] = grad[1]; fgrad[2] = grad[2];
	la_normalize3(fgrad);

	total = volr->ambient[0];
	dot = fgrad[0] * volr->lvec[0] + fgrad[1] * volr->lvec[1] + fgrad[2] * volr->lvec[2];
	if (dot > 0)
		total += dot * volr->diff[0];

	dot = fgrad[0] * volr->half[0] + fgrad[1] * volr->half[1] + fgrad[2] * volr->half[2];
	if (dot > 0)
		spec = ipow(dot, 40) * volr->spec[0];

	total += spec;
	rgba[0] = rgba[0] * total; 
	rgba[1] = rgba[1] * total; 
	rgba[2] = rgba[2] * total;
}

void shadesample_f(VCB_volren volr, void * gradient, float * rgba)
{
	float dot, total, spec=0.f, fgrad[3];
	float * grad = ((float *) gradient) + 1;

	fgrad[0] = grad[0]; fgrad[1] = grad[1]; fgrad[2] = grad[2];
	la_normalize3(fgrad);

	dot = fgrad[0] * volr->lvec[0] + fgrad[1] * volr->lvec[1] + fgrad[2] * volr->lvec[2];
	total = volr->ambient[0];
	if (dot > 0)
		total += dot * volr->diff[0];

	dot = fgrad[0] * volr->half[0] + fgrad[1] * volr->half[1] + fgrad[2] * volr->half[2];
	if (dot > 0)
		spec = ipow(dot, 40) * volr->spec[0];

	total += spec;
	rgba[0] = rgba[0] * total; 
	rgba[1] = rgba[1] * total; 
	rgba[2] = rgba[2] * total;
}

void composite_sample_a(float * rgba)
{
    float alpha = rgba[3]*(1.0f - fsumalpha);

    fsumred   += rgba[0]*alpha;
    fsumgreen += rgba[1]*alpha;
    fsumblue  += rgba[2]*alpha;
	fsumalpha += alpha;
}

void composite_sample_o(float * rgba)
{
    fsumalpha = rgba[3]*(1.0f - fsumalpha);
}

void writefbuffer_oub(int i, int j)
{
	unsigned char * buf;

	buf = (unsigned char *) fbuffer;
	buf[i * fbuf_h + j] = (unsigned char) ((fsumalpha > 1.0) ? 255 : (fsumalpha* 255+0.5));
}

void writefbuffer_aub(int i, int j)
{
	unsigned char * buf;

	buf = (unsigned char *) fbuffer;

	buf[(i * fbuf_h + j)*4 + 0] = (unsigned char) ((fsumred > 1.0)   ? 255 : (fsumred * 255+0.5));
	buf[(i * fbuf_h + j)*4 + 1] = (unsigned char) ((fsumgreen > 1.0) ? 255 : (fsumgreen * 255+0.5));
	buf[(i * fbuf_h + j)*4 + 2] = (unsigned char) ((fsumblue > 1.0)  ? 255 : (fsumblue  * 255+0.5));
	buf[(i * fbuf_h + j)*4 + 3] = (unsigned char) ((fsumalpha > 1.0) ? 255 : (fsumalpha * 255+0.5));
}

void writefbuffer_ous(int i, int j)
{
	unsigned short * buf;

	buf = (unsigned short *) fbuffer;
	buf[i * fbuf_h + j] = (unsigned short) ((fsumalpha > 1.0) ? 65536 : (fsumalpha* 65536));
}

void writefbuffer_aus(int i, int j)
{
	unsigned short * buf;

	buf = (unsigned short *) fbuffer;
	buf[(i * fbuf_h + j)*4 + 0] = (unsigned short) ((fsumred > 1.0)   ? 65536 : (fsumred   * 65536+0.5));
	buf[(i * fbuf_h + j)*4 + 1] = (unsigned short) ((fsumgreen > 1.0) ? 65536 : (fsumgreen * 65536+0.5));
	buf[(i * fbuf_h + j)*4 + 2] = (unsigned short) ((fsumblue > 1.0)  ? 65536 : (fsumblue  * 65536+0.5));
	buf[(i * fbuf_h + j)*4 + 3] = (unsigned short) ((fsumalpha > 1.0) ? 65536 : (fsumalpha * 65536+0.5));
}

void writefbuffer_of(int i, int j)
{
	float * buf;

	buf = (float *) fbuffer;
	buf[i * fbuf_h + j] = (fsumalpha > 1.0) ? 1.f : fsumalpha;
}

void writefbuffer_af(int i, int j)
{
	float * buf;

	buf = (float *) fbuffer;
	buf[(i * fbuf_h + j)*4 + 0] = (fsumred > 1.0)   ? 1.f : fsumred;
	buf[(i * fbuf_h + j)*4 + 1] = (fsumgreen > 1.0) ? 1.f : fsumgreen;
	buf[(i * fbuf_h + j)*4 + 2] = (fsumblue > 1.0)  ? 1.f : fsumblue;
	buf[(i * fbuf_h + j)*4 + 3] = (fsumalpha > 1.0) ? 1.f : fsumalpha;
}

static void (* init_ray) (void);
static void (* recons_sample3) (void * sample, float * coord, void * data, int nattribs, int * dsizes);
static void (* shadesample) (VCB_volren volr, void * gradient, float * rgba);
static void (* composite_sample)(float * rgba);
static void (* writefbuffer)(int i, int j);

void setup_funcptrs(vcbdatatype datatype, vcbdatatype fbtype, vcbOpticalModel opticalmodel)
{
	switch (datatype) {
		case VCB_UNSIGNEDBYTE: 
		case VCB_BYTE :
			recons_sample3 = recons_sample3ub;
			shadesample    = shadesample_b;
			break;
		case VCB_UNSIGNEDSHORT:
		case VCB_SHORT:
			recons_sample3 = recons_sample3s;
			shadesample    = shadesample_s;
			break;
		case VCB_UNSIGNEDINT:
		case VCB_INT:
		case VCB_BITFIELD:
			recons_sample3 = recons_sample3i;
			shadesample    = shadesample_f;
			break;
		case VCB_FLOAT:
		case VCB_DOUBLE:
			recons_sample3 = recons_sample3f;
			shadesample    = shadesample_f;
			break;
	}

	init_ray = (opticalmodel == VCB_ABSEMI) ? init_ray_a : init_ray_o;
	composite_sample = (opticalmodel == VCB_ABSEMI) ? composite_sample_a : composite_sample_o;

	switch (fbtype) {
		case VCB_UNSIGNEDBYTE: 
		case VCB_BYTE :
			writefbuffer = (opticalmodel == VCB_ABSEMI) ? writefbuffer_aub : writefbuffer_oub;
			break;
		case VCB_UNSIGNEDSHORT:
		case VCB_SHORT:
			writefbuffer = (opticalmodel == VCB_ABSEMI) ? writefbuffer_aus : writefbuffer_ous;
			break;
		case VCB_UNSIGNEDINT:
		case VCB_INT:
		case VCB_BITFIELD:
		case VCB_FLOAT:
		case VCB_DOUBLE:
			writefbuffer = (opticalmodel == VCB_ABSEMI) ? writefbuffer_af : writefbuffer_of;
			break;
	}

}

void comp_imgspc_bbox(float * obj2scr, int * origin, int * dsizes, float *objbbox, float * rcast_bounds)
{
	float tv[4];
	int i, j;

	rcast_bounds[0] = rcast_bounds[2] = rcast_bounds[4] = 1e8;
	rcast_bounds[1] = rcast_bounds[3] = rcast_bounds[5] = -1e8;

	objbbox[0] = origin[0]*1.f;
	objbbox[1] = origin[1]*1.f;
	objbbox[2] = origin[2]*1.f;

	objbbox[3] = objbbox[0]+dsizes[0] - 1.f;
	objbbox[4] = objbbox[1];
	objbbox[5] = objbbox[2];

	objbbox[6] = objbbox[0]+dsizes[0] - 1.f;
	objbbox[7] = objbbox[1]+dsizes[1] - 1.f;
	objbbox[8] = objbbox[2];

	objbbox[9]  = objbbox[0];
	objbbox[10] = objbbox[1]+dsizes[1] - 1.f;
	objbbox[11] = objbbox[2];

	objbbox[12] = objbbox[0];
	objbbox[13] = objbbox[1];
	objbbox[14] = objbbox[2]+dsizes[2] - 1.f;

	objbbox[15] = objbbox[0]+dsizes[0] - 1.f;
	objbbox[16] = objbbox[1];
	objbbox[17] = objbbox[2]+dsizes[2] - 1.f;

	objbbox[18] = objbbox[0]+dsizes[0] - 1.f;
	objbbox[19] = objbbox[1]+dsizes[1] - 1.f;
	objbbox[20] = objbbox[2]+dsizes[2] - 1.f;

	objbbox[21] = objbbox[0];
	objbbox[22] = objbbox[1]+dsizes[1] - 1.f;
	objbbox[23] = objbbox[2]+dsizes[2] - 1.f;

	for (i = 0; i < 8; i ++) {
		vcbMatMult4f(tv, obj2scr,objbbox[i*3+0],objbbox[i*3+1],objbbox[i*3+2],1.f);

		for (j = 0; j < 3; j ++) {
			rcast_bounds[j*2+0] = VCB_MINVAL(tv[j],rcast_bounds[j*2+0]);
			rcast_bounds[j*2+1] = VCB_MAXVAL(tv[j],rcast_bounds[j*2+1]);
		}
	}
}

void vcbVolrRC(VCB_volren volr, 
				vcbdatatype vcbtype, 
				void * voxels, 
				int nval, 
				int * origin,
				int * dsizes, 
				float * scales, 
				float stepsz, 
				int * imgfootprint)
/*
 * ray-casting volume renderer
 *
 * inputs:
 *   volr:    the volume rendering context
 *   vcbtype:  data type of the volume
 *   origin:  origin of the volume block given
 *   dsizes:  dsizes[0] through dsizes[3] describes number of voxels in the 3 dimensions of the volume block
 *   nval:    number of variables on each voxel
 *   scales:  relative scale of the volume, for instance 1:1:3 for a non-uniform grid with 
 *            a bigger spacing in Z direction
 *   voxels:  pointer to the 3D volume, coordinate (i,j,k) is at location
 *                           ((i * dsize[1] + j) * dsize[2] + k) * nval
 * 
 *
 * outputs:
 *   imgfootprint: this is a pointer to 4 integer array. if imgfootprint is NULL,
 *            then no output via this pointer is given. But if it's not NULL and
 *            (assuming it has a space for at least 4 integers), the actually
 *            bounds of the screen projection of the bounding box of this volume
 *            being rendered is given on the image plane.
 *
 *   if the destination mode is VCB_MEMORY, an output image is gererated and
 *       stored in the memory space given in vcbVolrFramebuffer
 *   else, an image is still rendered but stored in an internal static
 *       memory allocation not visible outside the volume renderer API
 *       in this case, the internal static memory allocation is released
 *       when vcbVolrCloseRC is called.
 *
 * usage:
 *   a brief example usage of the vcb volume renderer is given. except a
 *   few functions, vcb volume renderer follows opengl conventions very
 *   closely.
 *
 *   VCB_volren volren;
 *   unsigned char * framebuffer;
 *   int imgfootprint[4];
 *   int origin = {32, 32, 32};
 *	 int w = h = 512;
 *
 *   &#47;* set up *\/
 *	 framebuffer = (unsigned char *) malloc (w*h*4*sizeof(unsigned char));
 *   volren = vcbNewVolRenderer(VCB_ABSEMI);
 *   vcbVolrPerspective(volren, 30.f, 1.f, .01f, 10000.f); 
 *   vcbVolrLookAt(volren, 32.f, 32.f, 32.f, 160.f, 32.f, 160.f, 1.f, 0.f, 0.f);
 *   vcbVolrLight(volren, 160.f, 32.f, 160.f); &#47;*light position*\/
 *   vcbVolrViewPort(volren, w, h);
 *   vcbVolrTransFunc(volren, voxtransf);  &#47;* specify the transfer function*\/
 *   vcbVolrFramebuffer(volren, VCB_MEMORY, VCB_UNSIGNEDBYTE, framebuffer);
 *
 *   &#47;* volume render*\/
 *   vcbVolrRC(volren, VCB_UNSIGNEDBYTE, voxels, 4, origin, sz, scales, 0.5f, imgfootprint);
 *
 *   &#47;* done rendering, save image to disk*\/
 *   vcbImgWriteBMP("rcast.bmp",framebuffer, 4, w, h);
 *
 *   &#47;* clear up *\/
 *   vcbVolrCloseRC(volren);
 *   vcbFreeVolRenderer(volren);
 *   free(framebuffer);
 */
{
	int   i, j, k, nsteps, umin, umax, vmin, vmax;
	float boundbox[24], rcast_bounds[6]; 
	float zrange, rgba[4], xmax,ymax,zmax;
	float p1[4], inc[4], startpnt[4], intpnt[6], realpnt[4];

	int nchannels = (volr->opticalmodel == VCB_AOREONLY) ? 1 : 4;
	int nfbytes = volr->imgw * volr->imgh * nchannels * vcbSizeOf(volr->fbtype);
	
	float residualz;

	if (fbuffer == NULL)
	  fbuffer = (volr->dst == VCB_OPENGL)? (malloc(nfbytes)) : volr->fbuffer;

	if (fbuffer == NULL)
	  fprintf(stderr,"vcbVolrRC failing due to NULL fbuffer\n");

	if (sample == NULL)
		sample = malloc (nval * vcbSizeOf(vcbtype));

	if (sample == NULL)
	  fprintf(stderr,"vcbVolrRC failing due to NULL sample ptr\n");

	fbuf_w = volr->imgw;
	fbuf_h = volr->imgh;

	vcbVolrUpdateLight(volr);

	setup_funcptrs(vcbtype, volr->fbtype, volr->opticalmodel);

	/* compute the bounding box in image space */
	comp_imgspc_bbox(volr->obj2scr, origin, dsizes, boundbox, rcast_bounds);

	umin = (int) floor(VCB_MAXVAL(rcast_bounds[0], 0.f));
	vmin = (int) floor(VCB_MAXVAL(rcast_bounds[2], 0.f));
	umax = (int) ceil(VCB_MINVAL(rcast_bounds[1], volr->imgw-1));
	vmax = (int) ceil(VCB_MINVAL(rcast_bounds[3], volr->imgh-1));

	if (imgfootprint != NULL) {
		imgfootprint[0] = umin;
		imgfootprint[1] = vmin;
		imgfootprint[2] = umax;
		imgfootprint[3] = vmax;
	}

	/* clear framebuffer */
	memset(fbuffer, '\0', nfbytes);

	offsets[0] = 0;
	offsets[1] = 1;
	offsets[2] = dsizes[2];
	offsets[3] = dsizes[2]+1;
	offsets[4] = dsizes[1]*dsizes[2];
	offsets[5] = offsets[4] + 1;
	offsets[6] = offsets[4] + dsizes[2];
	offsets[7] = offsets[4] + dsizes[2] + 1;
	for (i = 0; i < 8; offsets[i]*= nval, i ++);

	xmax = dsizes[0] - 1.f;
	ymax = dsizes[1] - 1.f;
	zmax = dsizes[2] - 1.f;

	/* loop over each pixel */
	p1[3] = 1.f;
	for (i = umin; i <= umax; i ++) {
	
		p1[0] = i + 0.5f;
		for (j = vmin; j <= vmax; j ++) {

			p1[2] = rcast_bounds[4] - 0.05f;
			p1[1] = j + 0.5f;

		    /* Determine basepoint of ray in data space */
			vcbMatMult4fv(startpnt, volr->scr2obj, p1);

			p1[2] += 0.01f;
			vcbMatMult4fv(inc, volr->scr2obj, p1);
			inc[0] -= startpnt[0]; inc[1] -= startpnt[1]; inc[2] -= startpnt[2];
			la_normalize3(inc);
      
			init_ray(); 
			if ((zrange = ray_block_intersection(startpnt, inc, boundbox, intpnt)) < stepsz/10.f) continue;

			/* Step through the ray */

			nsteps = (int)(zrange/stepsz);
			residualz = zrange - nsteps*stepsz;

			intpnt[0] += inc[0]*stepsz/2.f;
			intpnt[1] += inc[1]*stepsz/2.f;
			intpnt[2] += inc[2]*stepsz/2.f;
			for (k = 0; k < nsteps; intpnt[0] += inc[0]*stepsz, intpnt[1] +=inc[1]*stepsz, intpnt[2] += inc[2]*stepsz, k++) {

				realpnt[0] = intpnt[0] - origin[0];
				realpnt[1] = intpnt[1] - origin[1];
				realpnt[2] = intpnt[2] - origin[2];

				if ((realpnt[0] < 0.f) || (realpnt[0] >= xmax)
					|| (realpnt[1] < 0.f) || (realpnt[1] >= ymax)
					|| (realpnt[2] < 0.f) || (realpnt[2] >= zmax))
					continue;
 
				recons_sample3(sample, realpnt, voxels, nval, dsizes);
				volr->transf(rgba,sample); rgba[3] *= stepsz;

				if (rgba[3] < 0.00001f) continue;

				if (volr->opticalmodel != VCB_AOREONLY)
					shadesample(volr, sample, rgba);

				/* composite rgba into fsumalpha/red/green/blue */
				composite_sample(rgba);

				if (fsumalpha > 0.97) break;
			}
			
		    if ((fsumalpha <= 0.97) && (residualz > stepsz/10.f)) { /* in case we may miss something in the back */
		  
				/* take half a residualz step backward */
				intpnt[0] = intpnt[3] - inc[0]*(residualz/2.f);
				intpnt[1] = intpnt[4] - inc[1]*(residualz/2.f);
				intpnt[2] = intpnt[5] - inc[2]*(residualz/2.f);

				realpnt[0] = intpnt[0] - origin[0];
				realpnt[1] = intpnt[1] - origin[1];
				realpnt[2] = intpnt[2] - origin[2];

				if ((realpnt[0] >= 0.f) && (realpnt[0] < xmax)
					&& (realpnt[1] >= 0.f) && (realpnt[1] < ymax)
					&& (realpnt[2] >= 0.f) && (realpnt[2] < zmax)) {
 
					recons_sample3(sample, realpnt, voxels, nval, dsizes);
					volr->transf(rgba,sample); rgba[3] *= residualz;

					if (volr->opticalmodel != VCB_AOREONLY)
						shadesample(volr, sample, rgba);

					composite_sample(rgba);
				}
			}
		  
			writefbuffer(i, j);
		}
	}
}

void vcbVolrCloseRC(VCB_volren volr)
/*
 * after done using vcb volume renderer, must call the
 * corresponding closeVolRen function for a clean exit
 * there are: vcbVolrCloseRC/S/T for raycasting, splatting
 * and 3D texture mapping, respectively.
 */
{
  if(volr == NULL)
          return;
  if (volr->dst == VCB_OPENGL)
  {
    if (fbuffer != NULL)
    {
      free(fbuffer);
      /* fbuffer = NULL; */
    }
  }

  /* CHANGED BY CHRIS ON 2006/2/14 */
  /* Reset fbuffer no matter what so that a new volume renderer will get
   * its framebuffer properly pointed to.  If fbuffer is not NULL when a
   * a volume renderer is created, it will use the previous value, which
   * may be wrong. */
  fbuffer = NULL;

  if (sample != NULL)
  {
    free(sample);
    sample = NULL;
  }
}

