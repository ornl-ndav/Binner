/***************************************************************************
 *   Copyright (C) 2004 by Jian Huang                                      *
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
#include <math.h>
#include "vcbglutils.h"
#include <glu.h>
#include <glut.h>

void vcbGetOGLMaterial(vcbMaterial matid, GLfloat * matparam)
{
	GLfloat material[260] = 
	{/*MATERIAL_DEFAULT*/ 0.2f, 0.2f, 0.2f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 
	/*MATERIAL_BRASS*/   0.329412f, 0.223529f, 0.027451f, 1.0f,0.780392f, 0.568627f, 0.113725f, 1.0f,0.992157f, 0.941176f, 0.807843f, 1.0f,27.8974f, 
	/*MATERIAL_BRONZE*/  0.2125f, 0.1275f, 0.054f, 1.0f,0.714f, 0.4284f, 0.18144f, 1.0f,0.393548f, 0.271906f, 0.166721f, 1.0f,25.6f,
	/*MATERIAL_POLISHED_BRONZE*/ 0.25f, 0.148f, 0.06475f, 1.0f,0.4f, 0.2368f, 0.1036f, 1.0f,0.774597f, 0.458561f, 0.200621f, 1.0f,76.8f,
	/*MATERIAL_CHROME*/ 0.25f, 0.25f, 0.25f, 1.0f,0.4f, 0.4f, 0.4f, 1.0f,0.774597f, 0.774597f, 0.774597f, 1.0f,76.8f,
	/*MATERIAL_COPPER*/ 0.19125f, 0.0735f, 0.0225f, 1.0f,0.7038f, 0.27048f, 0.0828f, 1.0f,0.256777f, 0.137622f, 0.086014f, 1.0f,12.8f,
	/*MATERIAL_POLISHED_COPPER*/ 0.2295f, 0.08825f, 0.0275f, 1.0f,0.5508f, 0.2118f, 0.066f, 1.0f,0.580594f, 0.223257f, 0.0695701f, 1.0f, 51.2f,
	/*MATERIAL_GOLD*/ 0.24725f, 0.1995f, 0.0745f, 1.0f, 0.75164f, 0.60648f, 0.22648f, 1.0f, 0.628281f, 0.555802f, 0.366065f, 1.0f, 51.2f,
	/*MATERIAL_POLISHED_GOLD*/ 0.24725f, 0.2245f, 0.0645f, 1.0f,0.34615f, 0.3143f, 0.0903f, 1.0f,0.797357f, 0.723991f, 0.208006f, 1.0f,83.2f,
	/*MATERIAL_PEWTER*/ 0.105882f, 0.058824f, 0.113725f, 1.0f,0.427451f, 0.470588f, 0.541176f, 1.0f,0.333333f, 0.333333f, 0.521569f, 1.0f,9.84615f,
	/*MATERIAL_SILVER*/ 0.19225f, 0.19225f, 0.19225f, 1.0f,0.50754f, 0.50754f, 0.50754f, 1.0f,0.508273f, 0.508273f, 0.508273f, 1.0f,51.2f,
	/*MATERIAL_POLISHED_SILVER*/ 0.23125f, 0.23125f, 0.23125f, 1.0f, 0.2775f, 0.2775f, 0.2775f, 1.0f,0.773911f, 0.773911f, 0.773911f, 1.0f,89.6f,
	/*MATERIAL_BLACK_PLASTIC*/ 0.0f, 0.0f, 0.0f, 1.0f,0.01f, 0.01f, 0.01f, 1.0f,0.50f, 0.50f, 0.50f, 1.0f,32.0f,
	/*MATERIAL_BLACK_RUBBER*/ 0.02f, 0.02f, 0.02f, 1.0f,0.01f, 0.01f, 0.01f, 1.0f,0.4f, 0.4f, 0.4f, 1.0f,10.0f,
	/*MATERIAL_EMERALD*/ 0.0215f, 0.1745f, 0.0215f, 0.55f,0.07568f, 0.61424f, 0.07568f, 0.55f,0.633f, 0.727811f, 0.633f, 0.55f,76.8f,
	/*MATERIAL_JADE*/ 0.135f, 0.2225f, 0.1575f, 0.95f,0.54f, 0.89f, 0.63f, 0.95f,0.316228f, 0.316228f, 0.316228f, 0.95f,12.8f,
	/*MATERIAL_OBSIDIAN*/ 0.05375f, 0.05f, 0.06625f, 0.82f,0.18275f, 0.17f, 0.22525f, 0.82f,0.332741f, 0.328634f, 0.346435f, 0.82f, 38.4f,
	/*MATERIAL_PEARL*/ 0.25f, 0.20725f, 0.20725f, 0.922f,1.0f, 0.829f, 0.829f, 0.922f,0.296648f, 0.296648f, 0.296648f, 0.922f,11.264f,
	/*MATERIAL_RUBY*/ 0.1745f, 0.01175f, 0.01175f, 0.55f,0.61424f, 0.04136f, 0.04136f, 0.55f, 0.727811f, 0.626959f, 0.626959f, 0.55f,76.8f,
	/*MATERIAL_TURQUOISE*/ 0.1f, 0.18725f, 0.1745f, 0.8f,0.396f, 0.74151f, 0.69102f, 0.8f,0.297254f, 0.30829f, 0.306678f, 0.8f, 12.8f};

	int i;
	for (i = 0; i < 13; matparam[i] = material[matid*13+i], i ++)
		;
}


void vcbSetOGLMaterial(GLenum face, vcbMaterial materialId, float alpha)
{
	GLfloat matparam[13]; 
	GLfloat * amb, *dif, *spec, *exponent;
 
	vcbGetOGLMaterial(materialId, matparam);

	amb  = matparam;      amb[3] = alpha;
	dif  = amb + 4;       dif[3] = alpha;
	spec = dif + 4;       spec[3] = alpha;
	exponent = spec + 4;

	glMaterialfv(face, GL_AMBIENT,    amb );
	glMaterialfv(face, GL_DIFFUSE,    dif );
	glMaterialfv(face, GL_SPECULAR,   spec );
	glMaterialf (face, GL_SHININESS, *exponent);
}

void vcbEnableOGLlight(float ambient)
/* 
 * this is convenience function for enabling opengl lighting in general.
 * usually, it is not sufficient to call this function to enable lighting.
 * subsequently, one must call vcbSetOGLlight to enable each specific light
 * source.
 *
 * ambient is a float between 0 and 1.0 for setting global ambient light
 * intensity.
 */
{
   GLfloat lmodel_ambient[4];
   GLfloat local_view[]={0.0};

   lmodel_ambient[0] = ambient;
   lmodel_ambient[1] = ambient;
   lmodel_ambient[2] = ambient;
   lmodel_ambient[3] = 1.0;

   glLightModelfv(GL_LIGHT_MODEL_AMBIENT,lmodel_ambient);
   glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
   glEnable(GL_LIGHTING);
}

void vcbSetOGLlight(GLenum light, float ambient, float intensity, float posx, float posy, float posz, float spot_expon)
/*
 * this is a convenience function for enabling a specific light source, as specified
 * by "light", such as GL_LIGHT0.
 *
 * ambient   - ambient component of this light source (always assume white colored light)
 * intensity - diffusion and specular components of this light source (white light)
 * posx,y,z  - position of light source
 * spot_expon - Phong exponential term for the specular component
 *
 * Usage:
 *   For example, call this at the beginning of the display function
 *	    vcbEnableOGLlight(0.1f);
 *      vcbSetOGLlight(GL_LIGHT0, 0.1, 0.8, lightpos_x, lightpos_y, lightpos_z, 40.f);
 *
 * Note: if using arcball to control user movement, calling these two light functions before arcball
 *       calls in display results in a light source rotating with user. Putting them after arcball
 *       calls makes the light source stationary.
 */
{   

	GLfloat amb[4], dif[4], spe[4], pos[4];
	glEnable(light);

	amb[0] = amb[1] = amb[2] = ambient;   amb[3] = 1.f;
	dif[0] = dif[1] = dif[2] = intensity; dif[3] = 1.f;
	spe[0] = spe[1] = spe[2] = intensity; spe[3] = 1.f;
	pos[0] = posx; pos[1] = posy; pos[2] = posz; pos[3] = 0.0;
	
	glLightfv(light,GL_AMBIENT,amb);
	glLightfv(light,GL_DIFFUSE,dif);
	glLightfv(light,GL_SPECULAR,spe);
	glLightfv(light,GL_POSITION,pos);
	glLightf (light,GL_SPOT_EXPONENT,spot_expon);
}

int vcbIsPowerOf2 (int x)
{
	return ( (x&(x-1))==0 )? 1 : 0;
}


float vcbGetOGLVersion(void)
{
	return (float)(atof((char *) glGetString(GL_VERSION)));
}


/* 
INTERNAL USE by other functions in vrogl.c. 
SHOULD NOT BE ACCESSIBLE VIA vcbglutils.h.

a hack code to invert an ogl matrix. works well till now, may
replace it with a better code later. return 0 if failed, 1 otherwise. 
*/
int invertOGLMatrix(float *out, float *m)
{
	/* Note: OpenGL Matrices are COLUMN major. */ 

	#ifndef VCB_MAT
#define VCB_MAT(m,r,c) (m)[(c)*4+(r)] /* a handy indexing tool */
#endif
	#ifndef VCB_SWAP_ROWS
#define VCB_SWAP_ROWS(a, b) { float *_tmp = a; (a)=(b); (b)=_tmp; }
#endif

	float wtmp[4][8];
	float m0, m1, m2, m3, s;
	float *r0, *r1, *r2, *r3;
  
	r0 = wtmp[0], r1 = wtmp[1], r2 = wtmp[2], r3 = wtmp[3];

	r0[0] = VCB_MAT(m,0,0); r0[1] = VCB_MAT(m,0,1);
	r0[2] = VCB_MAT(m,0,2); r0[3] = VCB_MAT(m,0,3);
	r0[4] = 1.0, r0[5] = r0[6] = r0[7] = 0.0,
    
	r1[0] = VCB_MAT(m,1,0); r1[1] = VCB_MAT(m,1,1);
	r1[2] = VCB_MAT(m,1,2); r1[3] = VCB_MAT(m,1,3);
	r1[5] = 1.0, r1[4] = r1[6] = r1[7] = 0.0,
    
	r2[0] = VCB_MAT(m,2,0); r2[1] = VCB_MAT(m,2,1);
	r2[2] = VCB_MAT(m,2,2); r2[3] = VCB_MAT(m,2,3);
	r2[6] = 1.0, r2[4] = r2[5] = r2[7] = 0.0,
    
	r3[0] = VCB_MAT(m,3,0); r3[1] = VCB_MAT(m,3,1);
	r3[2] = VCB_MAT(m,3,2); r3[3] = VCB_MAT(m,3,3);
	r3[7] = 1.0, r3[4] = r3[5] = r3[6] = 0.0;
  
	/* choose pivot - or die */
	if (fabs(r3[0])>fabs(r2[0])) VCB_SWAP_ROWS(r3, r2);
	if (fabs(r2[0])>fabs(r1[0])) VCB_SWAP_ROWS(r2, r1);
	if (fabs(r1[0])>fabs(r0[0])) VCB_SWAP_ROWS(r1, r0);
	if (0.0 == r0[0]) {
		return 0;
	}
  
	/* eliminate first variable     */
	m1 = r1[0]/r0[0]; m2 = r2[0]/r0[0]; m3 = r3[0]/r0[0];
	s = r0[1]; r1[1] -= m1 * s; r2[1] -= m2 * s; r3[1] -= m3 * s;
	s = r0[2]; r1[2] -= m1 * s; r2[2] -= m2 * s; r3[2] -= m3 * s;
	s = r0[3]; r1[3] -= m1 * s; r2[3] -= m2 * s; r3[3] -= m3 * s;
	s = r0[4];
	if (s != 0.0) { r1[4] -= m1 * s; r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r0[5];
	if (s != 0.0) { r1[5] -= m1 * s; r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r0[6];
	if (s != 0.0) { r1[6] -= m1 * s; r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r0[7];
	if (s != 0.0) { r1[7] -= m1 * s; r2[7] -= m2 * s; r3[7] -= m3 * s; }
  
	/* choose pivot - or die */
	if (fabs(r3[1])>fabs(r2[1])) VCB_SWAP_ROWS(r3, r2);
	if (fabs(r2[1])>fabs(r1[1])) VCB_SWAP_ROWS(r2, r1);
	if (0.0 == r1[1]) {
		return 0;
	}
  
	/* eliminate second variable */
	m2 = r2[1]/r1[1]; m3 = r3[1]/r1[1];
	r2[2] -= m2 * r1[2]; r3[2] -= m3 * r1[2];
	r2[3] -= m2 * r1[3]; r3[3] -= m3 * r1[3];
	s = r1[4]; if (0.0 != s) { r2[4] -= m2 * s; r3[4] -= m3 * s; }
	s = r1[5]; if (0.0 != s) { r2[5] -= m2 * s; r3[5] -= m3 * s; }
	s = r1[6]; if (0.0 != s) { r2[6] -= m2 * s; r3[6] -= m3 * s; }
	s = r1[7]; if (0.0 != s) { r2[7] -= m2 * s; r3[7] -= m3 * s; }
  
	/* choose pivot - or die */
	if (fabs(r3[2])>fabs(r2[2])) VCB_SWAP_ROWS(r3, r2);
	if (0.0 == r2[2]) {
		return 0;
	}

	/* eliminate third variable */
	m3 = r3[2]/r2[2];
	r3[3] -= m3 * r2[3]; r3[4] -= m3 * r2[4];
	r3[5] -= m3 * r2[5]; r3[6] -= m3 * r2[6];
	r3[7] -= m3 * r2[7];
  
	/* last check */
	if (0.0f == r3[3]) {
		return 0;
	}
  
	s = 1.0f/r3[3];              /* now back substitute row 3 */
	r3[4] *= s; r3[5] *= s; r3[6] *= s; r3[7] *= s;
  
	m2 = r2[3];                 /* now back substitute row 2 */
	s  = 1.0f/r2[2];
	r2[4] = s * (r2[4] - r3[4] * m2); r2[5] = s * (r2[5] - r3[5] * m2);
	r2[6] = s * (r2[6] - r3[6] * m2); r2[7] = s * (r2[7] - r3[7] * m2);
	m1 = r1[3];
	r1[4] -= r3[4] * m1; r1[5] -= r3[5] * m1;
	r1[6] -= r3[6] * m1; r1[7] -= r3[7] * m1;
	m0 = r0[3];
	r0[4] -= r3[4] * m0; r0[5] -= r3[5] * m0,
	r0[6] -= r3[6] * m0; r0[7] -= r3[7] * m0;
  
	m1 = r1[2];                 /* now back substitute row 1 */
	s  = 1.0f/r1[1];
	r1[4] = s * (r1[4] - r2[4] * m1); r1[5] = s * (r1[5] - r2[5] * m1);
	r1[6] = s * (r1[6] - r2[6] * m1); r1[7] = s * (r1[7] - r2[7] * m1);
	m0 = r0[2];
	r0[4] -= r2[4] * m0; r0[5] -= r2[5] * m0;
	r0[6] -= r2[6] * m0; r0[7] -= r2[7] * m0;
  
	m0 = r0[1];                 /* now back substitute row 0 */
	s  = 1.0f/r0[0];
	r0[4] = s * (r0[4] - r1[4] * m0); r0[5] = s * (r0[5] - r1[5] * m0);
	r0[6] = s * (r0[6] - r1[6] * m0); r0[7] = s * (r0[7] - r1[7] * m0);
  
	VCB_MAT(out,0,0) = r0[4]; VCB_MAT(out,0,1) = r0[5],
	VCB_MAT(out,0,2) = r0[6]; VCB_MAT(out,0,3) = r0[7],
	VCB_MAT(out,1,0) = r1[4]; VCB_MAT(out,1,1) = r1[5],
	VCB_MAT(out,1,2) = r1[6]; VCB_MAT(out,1,3) = r1[7],
	VCB_MAT(out,2,0) = r2[4]; VCB_MAT(out,2,1) = r2[5],
	VCB_MAT(out,2,2) = r2[6]; VCB_MAT(out,2,3) = r2[7],
	VCB_MAT(out,3,0) = r3[4]; VCB_MAT(out,3,1) = r3[5],
	VCB_MAT(out,3,2) = r3[6]; VCB_MAT(out,3,3) = r3[7]; 
  
	return 1;
}


void vcbCalcViewLightVecs(float *m, 
			  float * eyepos, 
			  float * center, 
			  float * lightpos,
			  float * vdir,     /* output: view direction */
			  float * ldir)     /* input: light direction */
{  
	int i;
	float fNorm, lNorm;
	float p[4], p2[4], dst[4], dst2[4];
	float m_inv[16];
  
	/* calculate original view vector */
	vdir[0] = eyepos[0] - center[0];
	vdir[1] = eyepos[1] - center[1];
	vdir[2] = eyepos[2] - center[2];
  
	/* calculate light vector */
	ldir[0] = lightpos[0] - center[0];
	ldir[1] = lightpos[1] - center[1];
	ldir[2] = lightpos[2] - center[2];

	/* normalize both */
	fNorm = (float)sqrt(vdir[0]*vdir[0] + vdir[1]*vdir[1] + vdir[2]*vdir[2]);
	lNorm = (float)sqrt(ldir[0]*ldir[0] + ldir[1]*ldir[1] + ldir[2]*ldir[2]);

	vdir[0] /= fNorm;
	vdir[1] /= fNorm;
	vdir[2] /= fNorm;

	ldir[0] /= lNorm;
	ldir[1] /= lNorm;
	ldir[2] /= lNorm;

	/* calculate transformed view and light vectors */
	p[0]  = vdir[0]; p[1]  = vdir[1]; p[2] = vdir[2];  p[3] = 0; 
	p2[0] = ldir[0]; p2[1] = ldir[1]; p2[2] = ldir[2]; p2[3] = 0; 
	
	/* 
	 * using the rotation matrix inverse m_inv
	 * to transform the light and view directions
	 * to modelview coordinates
	 */
	if (invertOGLMatrix(m_inv,m)) {
		for (i=0; i<4; i++) {
			dst[i] = m_inv[i]*p[0] + m_inv[i+4]*p[1] + m_inv[i+8]*p[2] + m_inv[i+12]*p[3];
			dst2[i] = m_inv[i]*p2[0] + m_inv[i+4]*p2[1] + m_inv[i+8]*p2[2] + m_inv[i+12]*p2[3];
		}
	} else {
		fprintf(stderr, "Failed to invert rotation matrix\n");
	}

	vdir[0] = dst[0];
	vdir[1] = dst[1];
	vdir[2] = dst[2];

	ldir[0] = dst2[0];
	ldir[1] = dst2[1];
	ldir[2] = dst2[2];
}


void transposeMatrix(float dst[16], float src[16])
{
	dst[ 0] = src[0]; dst[ 1] = src[4]; dst[ 2] = src[ 8]; dst[ 3] = src[12];
	dst[ 4] = src[1]; dst[ 5] = src[5]; dst[ 6] = src[ 9]; dst[ 7] = src[13];
	dst[ 8] = src[2]; dst[ 9] = src[6]; dst[10] = src[10]; dst[11] = src[14];
	dst[12] = src[3]; dst[13] = src[7]; dst[14] = src[11]; dst[15] = src[15];
}


void vcbUpdateTexMatShadedStreamline(float * texmat, 
				     float * vdir, 
				     float * ldir)
{
	int i;
	float tempMat[16];

	for (i=0; i<16; i++)  texmat[i] = 0;
	texmat[3] = texmat[7] = 0.5;
	texmat[15] = 1;

	/* set the new light vector */
	texmat[0] = ldir[0]/2;
	texmat[1] = ldir[1]/2;
	texmat[2] = ldir[2]/2;

	/* set the new view vector */
	texmat[4] = vdir[0]/2;
	texmat[5] = vdir[1]/2;
	texmat[6] = vdir[2]/2;

	/* transpose */
	transposeMatrix(tempMat,texmat);
	for (i=0; i<16; i++) texmat[i] = tempMat[i];
}

int vcbBindOGLTexture1D(int texunit, 
                     int interp, int informat, int format, int valtype,
                     int length, 
                     const GLvoid* pixels, 
                     GLfloat tex_envmode,
                     const GLfloat * texmatrix)
{
	GLuint thisTexObj;

	if (vcbGetOGLVersion() < 2.0f) {
		if (vcbIsPowerOf2(length) == 0) {
			fprintf(stderr,"\aTexture length: %d\n Error. Your version of OpenGL does not support non power-of-two texture dimentions.\n",length);
			exit(0);
		}
	}

	if (pixels == NULL) {
		fprintf(stderr,"\aError. The texture points to NULL.\n");	
		exit(0);
	}

	glGenTextures(1,&thisTexObj);
	glBindTexture(GL_TEXTURE_1D, thisTexObj);
	glTexImage1D(GL_TEXTURE_1D, 
				0,                    /* always assume level 0 in LoD */
				informat,             /* internal format */
				length,
				0,                    /* always assume no border */
				format, 
				valtype, 
				pixels);

	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, interp);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, interp);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

	glActiveTexture(texunit);
	glEnable(GL_TEXTURE_1D);
	glBindTexture(GL_TEXTURE_1D, thisTexObj);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, tex_envmode);
  
	if (texmatrix != NULL) {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMultMatrixf(texmatrix);
	}

	return thisTexObj;
}

int vcbUpdateOGLTexture1D(int thisTexObj, int texunit, 
                     int interp, int informat, int format, int valtype,
                     int length, 
                     const GLvoid* pixels, 
                     GLfloat tex_envmode,
                     const GLfloat * texmatrix)
{
	if (vcbGetOGLVersion() < 2.0f) {
		if (vcbIsPowerOf2(length) == 0) {
			fprintf(stderr,"\aTexture length: %d\n Error. Your version of OpenGL does not support non power-of-two texture dimentions.\n",length);
			exit(0);
		}
	}

	if (pixels == NULL) {
		fprintf(stderr,"\aError. The texture points to NULL.\n");	
		exit(0);
	}

	glActiveTexture(texunit);
	glBindTexture(GL_TEXTURE_1D, thisTexObj);
	glTexImage1D(GL_TEXTURE_1D, 
				0,                    /* always assume level 0 in LoD */
				informat,             /* internal format */
				length,
				0,                    /* always assume no border */
				format, 
				valtype, 
				pixels);

	if (texmatrix != NULL) {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMultMatrixf(texmatrix);
	}

	return thisTexObj;
}

int vcbBindOGLTexture2D(int texunit, 
		     int interp, int informat, int format, int valtype,
		     int width, int height, 
		     const GLvoid* pixels, 
		     GLfloat tex_envmode,
		     const GLfloat * texmatrix)
{ 
	GLuint thisTexObj;

	if (vcbGetOGLVersion() < 2.0f) {
		if (vcbIsPowerOf2(width) == 0) {
			fprintf(stderr,"\aTexture width: %d\n Error. Your version of OpenGL does not support non power-of-two texture dimentions.\n",width);
			exit(0);
		}
		if (vcbIsPowerOf2(height) == 0) {
			fprintf(stderr,"\aTexture height: %d\n Error. Your version of OpenGL does not support non power-of-two texture dimentions.\n",height);
			exit(0);
		}
	}

	if (pixels == NULL) {
		fprintf(stderr,"\aError. The texture points to NULL.\n");	
		exit(1);
	}

	glGenTextures(1,&thisTexObj);
	glActiveTexture(texunit);
	glBindTexture(GL_TEXTURE_2D, thisTexObj);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interp);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interp);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#if 1
	gluBuild2DMipmaps(GL_TEXTURE_2D, informat, width, height, format, valtype, pixels);
#else
	glTexImage2D(GL_TEXTURE_2D, 
				0,                    /* always assume level 0 in LoD */
				informat,             /* internal format */
				width,
				height,
				0,                    /* always assume no border */
				format, 
				valtype, 
				pixels);
#endif
	glEnable(GL_TEXTURE_2D);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, tex_envmode);
  
	if (texmatrix != NULL) {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMultMatrixf(texmatrix);
	}

	return thisTexObj;
}

int vcbUpdateOGLTexture2D(int thisTexObj, int texunit, 
		     int interp, int informat, int format, int valtype,
		     int width, int height, 
		     const GLvoid* pixels, 
		     GLfloat tex_envmode,
		     const GLfloat * texmatrix)
{
	if (vcbGetOGLVersion() < 2.0f) {
		if (vcbIsPowerOf2(width) == 0) {
			fprintf(stderr,"\aTexture width: %d\n Error. Your version of OpenGL does not support non power-of-two texture dimentions.\n",width);
			exit(0);
		}
		if (vcbIsPowerOf2(height) == 0) {
			fprintf(stderr,"\aTexture height: %d\n Error. Your version of OpenGL does not support non power-of-two texture dimentions.\n",height);
			exit(0);
		}
	}

	if (pixels == NULL) {
		fprintf(stderr,"\aError. The texture points to NULL.\n");	
		exit(1);
	}

	glActiveTexture(texunit);
	glBindTexture(GL_TEXTURE_2D, thisTexObj);
#if 1
	gluBuild2DMipmaps(GL_TEXTURE_2D, informat, width, height, format, valtype, pixels);
#else
	glTexImage2D(GL_TEXTURE_2D, 
				0,                    /* always assume level 0 in LoD */
				informat,             /* internal format */
				width,
				height,
				0,                    /* always assume no border */
				format, 
				valtype, 
				pixels);
#endif
 
	if (texmatrix != NULL) {
		glMatrixMode(GL_TEXTURE);
		glLoadIdentity();
		glMultMatrixf(texmatrix);
	}

	return thisTexObj;
}

void vcbGenTextureShadedStreamline (vcbMaterial matid, float * texmat, float * shadingtex, int texres)
{
	float fInc, f1, f2, fR, fG, fB;
	float fL_N, fV_R, fL_T, fV_T;
	float fDterm, fSterm;
	float shadingparam[13];
	int i, j;

	/* the ambient, diffuse and specular components */
	/* of the 2D streamline texture */
	float Kar, Kag, Kab, Kdr, Kdg, Kdb, Ksr, Ksg, Ksb, Sn;
	float Dp = 2.0f;  /* correction of brightness (Banks SIGGRAPH'94, Zockler VIS'96] */

	vcbGetOGLMaterial(matid, shadingparam);

	/* only use the RGB and exponent components, ignore alpha's */

	Kar = shadingparam[0]; 
	Kag = shadingparam[1]; 
	Kab = shadingparam[2];
	Kdr = shadingparam[4];
	Kdg = shadingparam[5];
	Kdb = shadingparam[6];
	Ksr = shadingparam[8];
	Ksg = shadingparam[9];
	Ksb = shadingparam[10];
	Sn = shadingparam[12];

	/* init the texture matrix for streamline shading */
	for (i=0; i<16; i++)  texmat[i] = 0.f;

	texmat[3] = texmat[7] = 0.5f;
	texmat[15] = 1.f;

	/* init the texture for streamline shading */
	fInc = 1.f/(texres-1.f);	

	for (i=0; i<texres; i++) {
		for (j=0; j<texres; j++) {
			f1 = i*fInc;
			f2 = j*fInc;

			/* compute rgb invariant terms */
			fL_T = 2.f*f1-1.f;
			fV_T = 2.f*f2-1.f;
			fL_N = (float)sqrt(1.-(fL_T)*(fL_T));
			fV_R = (float)(fL_N*sqrt(1.-(fV_T)*(fV_T))) - (fL_T*fV_T);

			/* compute rgb components */
			if(fL_N < 0) fL_N = 0.f;
			if(fV_R < 0) fV_R = 0.f;
			
			fDterm = (float)pow(fL_N,Dp);
			fSterm = (float)pow(fV_R,Sn);
			fR = Kar + (Kdr*fDterm) + (Ksr*fSterm);
			fG = Kag + (Kdg*fDterm) + (Ksg*fSterm);
			fB = Kab + (Kdb*fDterm) + (Ksb*fSterm);

			if(fR > 1.f) fR = 1.f;
			if(fG > 1.f) fG = 1.f;
			if(fB > 1.f) fB = 1.f;

			shadingtex[(j*texres*3) + (i*3) + 0] = fR;
			shadingtex[(j*texres*3) + (i*3) + 1] = fG;
			shadingtex[(j*texres*3) + (i*3) + 2] = fB;
		}
	}
}


int vcbInitShadedStreamline(int texunit, vcbMaterial matid)
{
	float * texmat, * shadingtex;
	int     texres = 128, texobj;

	texmat     = (float *) malloc (sizeof(float)* 16); /* 4 by 4 matrix */
	shadingtex = (float *) malloc (sizeof(float)*texres*texres*3);

	vcbGenTextureShadedStreamline(matid, texmat,shadingtex,texres);

	texobj = vcbBindOGLTexture2D(texunit, GL_LINEAR, GL_RGB, GL_RGB, GL_FLOAT,
		texres, texres, shadingtex, GL_REPLACE, NULL);

	return texobj;
}

void vcbOGLprint(float x, float y, float r, float g, float b, char *instring)
/*
 * vcbOGLprint outputs a text string "instring" to location (x,y) on the
 * screen in a color specified by (r,g,b)
 *
 * Note: 1. x, y are in the vcbRange of 0 and 1, with (0,0) being the lower-left
 *          corner .
 *       2. instring must be NULL terminated. for instance, sprintf produces
 *          NULL terminted strings
 *       3. must turn off all texture units that affects RGBA channels.
 *          if multiple texture units are being used, must turn off them
 *          one by one.
 *
 * sample code:
 *    Here we turn off two texture units, printf a message
 *    at the lower-left corner, and then re-enable the texture
 *    units:
 *	       glActiveTexture(GL_TEXTURE0);
 *         glDisable(GL_TEXTURE_1D);
 *         glActiveTexture(GL_TEXTURE1);
 *         glDisable(GL_TEXTURE_2D);
 *           vcbOGLprint(0.01f,0.01f, 1.f, 1.f, 0.f, echofps);
 *         glEnable(GL_TEXTURE_2D);
 *         glActiveTexture(GL_TEXTURE0);
 *         glEnable(GL_TEXTURE_1D);
 *
 * must turn off all texture units that affects RGB channels
 */
{
	GLint matrixMode;
	GLboolean lightingOn, blendOn, atestOn;

	/* setup opengl states for text output, saving modified states too */
	if (lightingOn = glIsEnabled(GL_LIGHTING))
		glDisable(GL_LIGHTING);
	if (blendOn    = glIsEnabled(GL_BLEND))
		glDisable(GL_BLEND);
	if (atestOn = glIsEnabled(GL_ALPHA_TEST))
		glDisable(GL_ALPHA_TEST);
	glGetIntegerv(GL_MATRIX_MODE, &matrixMode);

	glPushAttrib(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0., 1., 0., 1., -1., 1.);
	/* end set up */

	glColor3f(r,g,b);
	glRasterPos3f(x, y, 0.f);
	while ((*instring) != '\0')
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, (int)(*instring++));

	/* restore opengl state machine */
	glPopAttrib();
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(matrixMode);
	if (lightingOn) glEnable(GL_LIGHTING);
	if (blendOn)    glEnable(GL_BLEND);
	if (atestOn)    glEnable(GL_ALPHA_TEST);

	glEnable(GL_BLEND);
	glEnable(GL_ALPHA_TEST);
}

void draw_string(GLfloat x, GLfloat y, GLfloat z, char *str) {
    glRasterPos3f(x, y, z);
    while ((*str) != '\0') glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, (int)(*str++));
}

/* ------------------------------------------------------------------------- */

GLuint make_axis_list(int viewport_size) {
    int CYL_RADIUS = 2;
    int CYL_HEIGHT = 40;
    int CAP_HEIGHT = 10;
    GLUquadric *quad;
    GLuint list_id;
    GLfloat ambient[4] = {0.4, 0.4, 0.4, 1.0};
    GLfloat specular[4] = {0.5, 0.5, 0.5, 1.0};
    GLfloat red[4] = {0.9, 0.1, 0.1, 1.0};
    GLfloat green[4] = {0.1, 0.9, 0.1, 1.0};
    GLfloat blue[4] = {0.1, 0.1, 0.9, 1.0};
    GLfloat pearl[4] = {1.0, 0.829, 0.829, 0.922};
    GLint shininess = 50;
    GLfloat light_diffuse[4] = {0.8, 0.8, 0.8, 1.0};
    GLfloat light_ambient[4] = {0.2, 0.2, 0.2, 1.0};
    GLfloat light_specular[4] = {1.0, 1.0, 1.0, 1.0};
    /* GLfloat light_position[4] = {0.0, 0.0, 0.0, 1.0}; */
    GLfloat light_position[4] = {0.0, 0.0, 0.0, 0.0};
    GLfloat extent;
    GLint num_lights;
    int i;

    /* Get number of total lights. */
    glGetIntegerv(GL_MAX_LIGHTS, &num_lights);

    /* Make list and quadrics object that will be used for axis parts. */
    list_id = glGenLists(1);
    quad = gluNewQuadric();
    extent = CYL_HEIGHT + CAP_HEIGHT + 10;

    /* Since we're drawing in a smaller portion of the screen, we'll need to
     * change the viewport.  When we're done drawing, we'll also want to restore
     * the original viewport.  This call grabs the values at at the time the
     * list is made, and the list will restore THESE values when it is executed.
     * If the calling application changes the viewport after the list is made,
     * it must restore the correct values.  This can't be helped: Get calls are
     * run in immediate mode. */
    /* Changed our minds.  Don't expect the user to update viewport values
     * when a window is changed. */
    /* glGetIntegerv(GL_VIEWPORT, view); */

    /* Configure the light. */
    light_position[2] = 3 * CYL_HEIGHT;

    /* Start the list. */
    glNewList(list_id, GL_COMPILE);

    /* Turn off all lights, but turn 7 back on for our purposes. */
    for (i = 0; i < num_lights; i++) {
       glDisable(GL_LIGHT0 + i);
    }
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT7);

    /* Make axis appear before all other geometry by clearing the depth buffer
     * for just its viewport.  This requires the scissor test. */
    glEnable(GL_SCISSOR_TEST);
    glScissor(0, 0, viewport_size, viewport_size);
    glClear(GL_DEPTH_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

    /* Use a standard projection instead of application's. */
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-extent, extent, -extent, extent, -extent, extent);
    glMatrixMode(GL_MODELVIEW);

    /* Set the light in the untransformed coordinate system. */
    glPushMatrix();
    glLoadIdentity();
    glLightfv(GL_LIGHT7, GL_POSITION, light_position);
    glLightfv(GL_LIGHT7, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT7, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT7, GL_SPECULAR, light_specular);
    glPopMatrix();

    /* Draw in the bottom left corner of the window. */
    glViewport(0, 0, viewport_size, viewport_size);

    /* Ambient, specular, and shininess is the same for all. */
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);

    /* Y Cylinder. */
    glPushMatrix();
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, green);
       glRotatef(-90.0, 1.0, 0.0, 0.0);
       gluCylinder(quad, CYL_RADIUS, CYL_RADIUS, CYL_HEIGHT, 20, 20);
       glTranslatef(0.0, 0.0, CYL_HEIGHT);
       gluCylinder(quad, 2 * CYL_RADIUS, 0, CAP_HEIGHT, 20, 20);
       draw_string(-6, 0, 5 + CAP_HEIGHT, "y");
    glPopMatrix();

    /* X Cylinder. */
    glPushMatrix();
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, blue);
       glRotatef(90.0, 0.0, 1.0, 0.0);
       gluCylinder(quad, CYL_RADIUS, CYL_RADIUS, CYL_HEIGHT, 20, 20);
       glTranslatef(0.0, 0.0, CYL_HEIGHT);
       gluCylinder(quad, 2 * CYL_RADIUS, 0, CAP_HEIGHT, 20, 20);
       draw_string(-6, 0, 5 + CAP_HEIGHT, "x");
    glPopMatrix();

    /* Z Cylinder. */
    glPushMatrix();
       glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, red);
       glTranslatef(0.0, 0.0, -CYL_HEIGHT);
       gluCylinder(quad, CYL_RADIUS, CYL_RADIUS, CYL_HEIGHT, 20, 20);
       glRotatef(180, 0.0, 1.0, 0.0);
       gluCylinder(quad, 2 * CYL_RADIUS, 0, CAP_HEIGHT, 20, 20);
       draw_string(-6, 0, 5 + CAP_HEIGHT, "z");
    glPopMatrix();

    /* Make a sphere at the origin. */
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, pearl);
    gluSphere(quad, 2 * CYL_RADIUS, 20, 20);

    /* Give the application back its projection matrix. */
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    /* Turn light 7 back off. */
    glDisable(GL_LIGHT7);

    /* Done with list. */
    glEndList();

    gluDeleteQuadric(quad);

    return list_id;
}

/* ------------------------------------------------------------------------- */

void vcbDrawAxis(GLfloat m[16], int size) {

    GLint matrixMode, view[4];
    static GLuint axis_list_id = 0;
    static int axis_size = -1;

    if (axis_size != size) {
       axis_size = size;
       axis_list_id = make_axis_list(size);
    }

    /* setup opengl states for image output, saving modified states too */
    glGetIntegerv(GL_MATRIX_MODE, &matrixMode);

    glPushAttrib(GL_CURRENT_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT);
    glPushMatrix();

    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glMultMatrixf(&m[0]);

    glGetIntegerv(GL_VIEWPORT, view);

    glCallList(axis_list_id);

    glMatrixMode(GL_MODELVIEW);
    glViewport(view[0], view[1], view[2], view[3]);

    /* restore opengl state machine */
    glPopMatrix();
    glPopAttrib();

    glMatrixMode(matrixMode);
}

/* ------------------------------------------------------------------------- */

