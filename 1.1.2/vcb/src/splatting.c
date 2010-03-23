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
#include "vcbutils.h"
#include "vcbvolren.h"
#include "vcblinalg.h"

#include "glew.h"
#include <glut.h>
#include "vcbglutils.h"
#include "kernel.h"

/* splatting data structure */
static float varray[16], t1array[4];
static float t2array[8] = {0.f, 0.f, 1.f, 0.f, 0.f, 1.f, 1.f, 1.f};
static float t_0[2], t_1[2], t_2[2], t_3[2]; /* texture coordinates */
static float splat_0[4], splat_1[4], splat_2[4], splat_3[4]; /* splat corners */
static float splat_sz = 1.56f*2.f;
static float * b_depth = NULL;
static int   * b_order = NULL;
static int   fpTex, tfTex;

#ifndef FTB
#define FTB 0
#endif
#ifndef FPRESOLUTION
#define FPRESOLUTION 64
#endif
void vcbVolrInitS(VCB_TRANSFUNC transf)
{
	float tlut[256*4], rgba[4];
	float fprint[FPRESOLUTION * FPRESOLUTION];
#if FTB
	float tab[FPRESOLUTION * FPRESOLUTION * 2];
#endif
	int   k;
	unsigned char vval;

	for (k = 0; k < 256; k ++) {
		vval = (unsigned char) k;
		transf(rgba, &vval);
		rgba[3] *= (splat_sz/2.f);
#if FTB
		rgba[0] *= rgba[3];
		rgba[1] *= rgba[3];
		rgba[2] *= rgba[3];
#endif
		tlut[k*4+0] = rgba[0];
		tlut[k*4+1] = rgba[1];
		tlut[k*4+2] = rgba[2];
		tlut[k*4+3] = rgba[3];
	}

	tfTex = vcbBindOGLTexture1D(GL_TEXTURE0, GL_NEAREST, GL_RGBA, GL_RGBA, GL_FLOAT, 
		                     256, tlut, GL_MODULATE, NULL);

	fptTableC2d(FPRESOLUTION, fprint, -2.f, 2.f);
#if FTB
	for (k = 0; k < FPRESOLUTION*FPRESOLUTION; k ++) {
		tab[k*2+0] = fprint[k];
		tab[k*2+1] = fprint[k];
	}
	fpTex = vcbBindOGLTexture2D(GL_TEXTURE1, GL_NEAREST, GL_LUMINANCE_ALPHA, GL_LUMINANCE_ALPHA, GL_FLOAT, 
		                     FPRESOLUTION, FPRESOLUTION, tab, GL_MODULATE, NULL);
#else
	fpTex = vcbBindOGLTexture2D(GL_TEXTURE1, GL_NEAREST, GL_ALPHA, GL_ALPHA, GL_FLOAT, 
		                     FPRESOLUTION, FPRESOLUTION, fprint, GL_MODULATE, NULL);
#endif

	vcbSetOGLMaterial(GL_FRONT_AND_BACK, MATERIAL_DEFAULT, 1.0);
	glShadeModel(GL_FLAT);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(0);

#if FTB
	/* 
	 * front-to-back compositing:
	 * because opengl only allows one multiply on each fragment. wht FTB volume rendering
	 * need is: dst = src * src_alpha * (1 - dst_alpha).
	 * 
	 * Note, pre-multiplying src_alpha to RGB channels in the transfer function look-up
	 * table does not work in splatting, because the footprint takes effect via alpha
	 * channel being multiplied as src alpha. therefore the following won't work
	 *      glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
	 *
	 * In this case, the footprint must be set as below
	 */
	glBlendFunc(GL_ONE_MINUS_DST_ALPHA, GL_ONE);
#else
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); /* back-to-front compositing */
#endif

	/*glBlendFunc(GL_SRC_ALPHA, GL_ONE);*/ /* X-Ray compositing */
	glEnable(GL_BLEND);
	glDisable(GL_NORMALIZE);

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

	glAlphaFunc(GL_GREATER, 0.005);
	glEnable(GL_ALPHA_TEST);
	glDisable(GL_DITHER);
}

void buck_toss(float * depth, int nvox, int nbuckets, int * out)
{
	int * bukcnt, * buckets;
	int i, id, pcnt, cnt;
	float min, max, unit;

	buckets = (int *) calloc(nbuckets * 2, sizeof(int));
	if (buckets == NULL) {
		perror("buck_toss");
		return;
	}
	bukcnt  = buckets + nbuckets;

	for (i = 0, min = 1e10, max = -1e10; i < nvox; i ++) {
		if (min > depth[i]) min = depth[i];
		if (max < depth[i]) max = depth[i];
	}
	min -= 1.f; max += 1.f;
	unit = (max - min)/nbuckets;

	for (i = 0; i < nvox; i ++) {
		id = (int)((depth[i]-min)/unit);
		buckets[id] ++;
	}

	for (i = 1, pcnt = buckets[0], buckets[0] = 0; i < nbuckets; i ++) {
		cnt = buckets[i];
		buckets[i] = buckets[i-1] + pcnt;
		pcnt = cnt;
	}

	for (i = 0; i < nvox; i ++) {
		id = (int)((depth[i]-min)/unit);
		out[buckets[id]+bukcnt[id]] = i;
		bukcnt[id] ++;
	}

	free(buckets);
}

void init_splatstructs(float * mat)  
{
	float v[4];

	t_0[0] = 0.f; t_0[1] = 0.f;
	t_1[0] = 1.f; t_1[1] = 0.f;
	t_2[0] = 0.f; t_2[1] = 1.f;
	t_3[0] = 1.f; t_3[1] = 1.f;

	v[0] = -splat_sz; v[1] = -splat_sz; v[2] = 0.f; v[3] = 1.f;
	vcbMatMult4fv(splat_0, mat,v);

	v[0] = splat_sz; v[1] = -splat_sz; v[2] = 0.f; v[3] = 1.f;
	vcbMatMult4fv(splat_1, mat,v);

	v[0] = -splat_sz; v[1] = splat_sz; v[2] = 0.f; v[3] = 1.f;
	vcbMatMult4fv(splat_2, mat,v);

	v[0] = splat_sz; v[1] = splat_sz; v[2] = 0.f; v[3] = 1.f;
	vcbMatMult4fv(splat_3, mat,v);
}

/* void vcbVolrSsplats(VCB_volren volr, vcbdatatype vcbtype, void * voxels, int nval, int * dsizes) */

#ifndef offvert
#define offvert(corner)    \
		(*fptr++) = x + corner[0]; \
		(*fptr++) = y + corner[1]; \
		(*fptr++) = z + corner[2]; \
		fptr++
#endif

void vcbVolrSsplats(unsigned char * voxels, int nvox, int * dsizes)
{
	int i, xdim, id;
	float pos[4], modelview[16], invmodelview[16], rotmat[16], view_vec[4];
	float * fptr, x, y, z, invf = 1.f/255.f;
	unsigned short * cvox;

	if (nvox < 1) return; /* there is nothing to do */

	if (b_depth != NULL) {
		free(b_depth);
		free(b_order);
	}
	b_depth = (float *) malloc(nvox * sizeof(float));
	b_order = (int *) malloc(nvox * sizeof(int));

	xdim = dsizes[0];
	if (xdim < dsizes[1]) xdim = dsizes[1];
	if (xdim < dsizes[2]) xdim = dsizes[2];

	glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
	memcpy(rotmat, modelview, 16*sizeof(float));
	rotmat[12] = rotmat[13] = rotmat[14] = 0.f; rotmat[15] = 1.f;
	init_splatstructs(rotmat);

	vcbTransposeMatr(modelview);
	vcbInvMat(invmodelview, modelview);
	vcbMatMult4f(view_vec, invmodelview, 0.f, 0.f, 1.f, 1.f);

	/* compute depth */
	for (i = 0; i < nvox; i ++) {
		cvox = (unsigned short *)&voxels[i*10+4];
		pos[0] = (float)(cvox[0]);	
		pos[1] = (float)(cvox[1]);
		pos[2] = (float)(cvox[2]);
#if FTB
		b_depth[i] = -(pos[0]*view_vec[0]+pos[1]*view_vec[1]+pos[2]*view_vec[2]);
#else
		b_depth[i] = (pos[0]*view_vec[0]+pos[1]*view_vec[1]+pos[2]*view_vec[2]);
#endif
	}

	buck_toss(b_depth, nvox, xdim*3, b_order);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  	glVertexPointer(3, GL_FLOAT, 16, varray);
	glClientActiveTexture(GL_TEXTURE0);
	glTexCoordPointer(1, GL_FLOAT, 0, t1array);
	glClientActiveTexture(GL_TEXTURE1);
	glTexCoordPointer(2, GL_FLOAT, 0, t2array);	

	for (i = 0; i < nvox; i ++) {
		id = b_order[i];

		fptr = varray;
		cvox = (unsigned short *)&voxels[id*10+4];
		x = (float)(cvox[0]);	
		y = (float)(cvox[1]);
		z = (float)(cvox[2]);

		offvert(splat_0);
		offvert(splat_1);
		offvert(splat_2);
		offvert(splat_3);

		t1array[0] = voxels[id*10] * invf;
		t1array[1] = t1array[0];
		t1array[2] = t1array[0];
		t1array[3] = t1array[0];

		glNormal3bv((GLbyte*)&voxels[id*10+1]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		/*glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indices);*/
	}

}



void vcbVolrCloseS()
/*
 * after done using vr volume renderer, must call the
 * corresponding closeVolRen function for a clean exit
 * there are: vcbVolrCloseRC/S/T for raycasting, splatting
 * and 3D texture mapping, respectively.
 */
{
	free(b_depth);
	free(b_order);
	b_depth = NULL;
	b_order = NULL;
}
