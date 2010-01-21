/****************************************************************************
 *   Copyright (C) 2004  by Jian Huang										*
 *	 seelab@cs.utk.edu														*
 *																			*
 *   This library is free software; you can redistribute it and/or			*
 *   modify it under the terms of the GNU Lesser General Public				*
 *   License as published by the Free Software Foundation; either			*
 *   version 2.1 of the License, or (at your option) any later version.		*
 *																			*
 *   This library is distributed in the hope that it will be useful,		*
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of			*
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU		*
 *   Lesser General Public License for more details.						*
 *																			*
 *   You should have received a copy of the GNU Lesser General Public		*
 *   License along with this library; if not, write to the					*
 *   Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,			*
 *   Boston, MA  02110-1301  USA											*
 ****************************************************************************/

#ifndef _VCB_LIC_H 
#define _VCB_LIC_H

#ifdef __cplusplus
extern "C" {
#endif

/** Function
 * Name: vcbGenLicTex3D
 * Version: 0.9
 * Description:
 *  vcbGenLicTex3D generates a LIC texture from a 3d flow field.
 *  Note: vcbGenLicTex3D assumes a stable fluid and uses the methods of slic.
 * Arguments:
 *  float* vectors: The flow field.
 *  unsigned char* lictex: The texture to be generated.
 *  int xsize: The vector size for the x dimension.
 *  int ysize: The vector size for the y dimension.
 *  int zsize: The vector size for the z dimension.
 * Return: None
 * Known Issues: None
 **/
void vcbGenLicTex3D (float * vectors, unsigned char * lictex, int xsize, int ysize, int zsize);

/** Function
 * Name: vcbGenLicTex2D
 * Version: 0.9
 * Description:
 *  vcbGenLicTex2D generates a LIC texture from a 2d flow field.
 *  Note: vcbGenLicTex2D assumes a stable fluid and uses the methods of slic.
 * Arguments:
 *  float* vectors: The flow field. It should contain Vx, Vy, Vx, Vy, Vx, Vy, ... , 
 *   in floating point.
 *  unsigned char* lictex: The texture to be generated.
 *  int xsize: The vector size for the x dimension.
 *  int ysize: The vector size for the y dimension.
 * Return: None
 * Known Issues: None
 **/
void vcbGenLicTex2D (float * vectors, unsigned char * lictex, int xsize, int ysize);

#ifdef __cplusplus
}  /* extern C */
#endif


#endif
