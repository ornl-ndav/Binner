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

#ifndef _VCB_LINALG_H
#define _VCB_LINALG_H

/* 
 * three convenience macros exclusively used internally 
 * not robust at all. do not expose in documentation
 */
#ifndef VCB_CROSS
#define VCB_CROSS(dest,v1,v2) \
          dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
          dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
          dest[2]=v1[0]*v2[1]-v1[1]*v2[0];
#endif
#ifndef VCB_DOT
#define VCB_DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])
#endif
#ifndef VCB_SUB
#define VCB_SUB(dest,v1,v2) \
          dest[0]=v1[0]-v2[0]; \
          dest[1]=v1[1]-v2[1]; \
          dest[2]=v1[2]-v2[2]; 
#endif

#ifdef __cplusplus
extern "C" {
#endif


/*
 * For these Linear Algebra function all matrix are row-major:
 *     float mat [16];
 * All vector are either:
 *     float vec [3];     or
 *     float vec [4];
 *
 * All function calls assume valid memory allocation for the input and output arguements.
 */

char * vcbPrintVec (float * a, int n);

void  la_cross3(float * res, float * v1, float * v2);

float la_dot3(float * v1, float * v2);

void  la_subtract(float * v1, float * v2);

void  la_add(float * v1, float * v2);

void  la_dupvec(float * dup, float * v);

void  la_dupmat(float * dup, float * mat);

float la_norm3(float * v1);

void  la_normalize3(float * v);
 
/** Function
 * Name: vcbIdentity3D
 * Version: 1.0
 * Description: vcbIdentity3D sets the 4x4 matrix "mat" to the identity matrix. Matrix is assumed to
 *  be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 * Return: None
 * Known Issues: None
 **/
void  vcbIdentity3D(float * mat);

/** Function
 * Name: vcbTranslate3f
 * Version: 1.0
 * Description: vcbTranslate3f sets the 4x4 matrix "mat" to the translation matrix that 
 *  translates by (x, y, z). Matrix is assumed to be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float x: x coordinate of translation.
 *  float y: y coordinate of translation.
 *  float z: z coordinate of translation.
 * Return: None
 * Known Issues: None
 **/
void  vcbTranslate3f(float * mat, float x, float y, float z);

/** Function
 * Name: vcbTranslate3fv
 * Version: 1.0
 * Description: vcbTranslate3fv sets the 4x4 matrix "mat" to the translation matrix that 
 *  translates by vector v. Matrix is assumed to be row-major. 
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float* v: vector to translate by.
 * Return: None
 * Known Issues: None
 **/
void  vcbTranslate3fv(float * mat, float * v);

/** Function
 * Name: vcbScale3f 
 * Version: 1.0
 * Description: vcbScale3f sets the 4x4 matrix "mat" to the scaling matrix that 
 *  scales by ("sx", "sy", "sz"). Matrix is assumed to be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float sx: x factor of scaling.
 *  float sy: y factor of scaling.
 *  float sz: z factor of scaling.
 * Return: None
 * Known Issues: None
 **/
void  vcbScale3f(float * mat, float sx, float sy, float sz);

/** Function
 * Name: vcbScale3fv 
 * Version: 1.0
 * Description: vcbScale3fv sets the 4x4 matrix "mat" to the scaling matrix that 
 *  scales by vector sv. Matrix is assumed to be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float* sv: vector to scale by.
 * Return: None
 * Known Issues: None
 **/
void  vcbScale3fv(float * mat, float * sv);

/** Function
 * Name: vcbRotate3f
 * Version: 1.0
 * Description: vcbRotate3f sets the 4x4 matrix "mat" to the rotation matrix that rotates "angle"
 *  degrees about the axis of ("ax", "ay", "az"). This axis need not be normalized. Matrix is assumed
 *  to be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float ax: x portion of axis of rotation.
 *  float ay: y portion of axis of rotation.
 *  float az: z portion of axis of rotation.
 *  float angle: angle in degrees for amount of rotation.
 * Return: None
 * Known Issues: None
 **/
void  vcbRotate3f(float * mat, float ax, float ay, float az, float angle);

/** Function
 * Name: vcbRotate3fv
 * Version: 1.0
 * Description: vcbRotate3fv sets the 4x4 matrix "mat" to the rotation matrix that rotates "angle"
 *  degrees about an axis specified as a 3 component vector "axis". This axis
 *  need not be normalized. Matrix is assumed to be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float* axis: 3 component vector for axis of rotation.
 *  float angle: angle in degrees for amount of rotation.
 * Return: None
 * Known Issues: None
 **/
void  vcbRotate3fv(float * mat, float * axis, float angle);
void  vcbRotate3dv(double * mat, double * axis, double angle);

/** Function
 * Name: vcbInvMat
 * Version: 1.1
 * Description: vcbInvMat inverts matrix "mat" and stores result in "invMat". Matrix is assumed to be
 *  row-major.
 * Arguments:
 *  float* invMat: Result of inverting "origmat".
 *  float* origmat: Linear array of a 4x4 matrix.
 * Return: None
 * Known Issues: None
 **/
void  vcbInvMat(float * invMat, float * origmat);

/** Function
 * Name: vcbInvMatr
 * Version: 1.1
 * Description: vcbInvMatr inverts matrix "vcbinvmat" and stores result in "invmat". Matrix is assumed to
 *  be row-major.
 * Arguments:
 *  float* vcbInvMat: 4x4 matrix to be inverted. Original matrix is overwritten with result.
 * Return: None
 * Known Issues: None
 **/
void  vcbInvMatr(float * vcbinvmat);

/** Function
 * Name: vcbTransposeMat
 * Version: 1.0
 * Description: vcbTransposeMat computes the transpose of the 4x4 matrix "mat" and 
 *  stores the result in "tmat".
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float* tmat: Result of tranposing "mat".
 * Return: None
 * Known Issues: None
 **/
void  vcbTransposeMat(float *tmat, float * mat);

/** Function
 * Name: vcbTransposeMatr
 * Version: 1.0
 * Description: vcbTransposeMatr computes the transpose of the 4x4 matrix "tmat" 
 *  and stores the result in "tmat". Matrix is assumed to be row-major.
 * Arguments:
 *  float* tmat: 4x4 matrix to be transposed. Original matrix is overwritten with result.
 * Return: None
 * Known Issues: None
 **/
void  vcbTransposeMatr(float *tmat);

/** Function
 * Name: vcbMatMult4f
 * Version: 1.0
 * Description: vcbMatMult4f translates the 4-component homogeneous coordinate ("x", "y", "z", "w"),
 *   by the 4x4 matrix "mat" and places the result into the vector "v". The W component of the output
 *   vector will always have a value of 1.0. Matrix is assumed to be row-major.
 * Arguments:
 *  float* v: Pointer to memory where result will be stored.
 *  float* mat: Linear array of a 4x4 matrix.
 *  float x: x component of homogeneous vector.
 *  float y: y component of homogeneous vector.
 *  float z: z component of homogeneous vector.
 *  float w: w component of homogeneous vector.
 * Return: None
 * Known Issues: None
 **/
void  vcbMatMult4f(float * v, float * mat, float x, float y, float z, float w);

/** Function
 * Name: vcbMatMult4fv
 * Version: 1.0
 * Description:  * vcbMatMult4fv translates the 4-component homogeneous vector "v1" by the 4x4 matrix
 * "mat" and places the result into the vector "v". The W component of the result is always 1.0.
 *  Matrix is assumed to be row-major.
 * Arguments:
 *  float* v: Pointer to memory where result will be stored.
 *  float* mat: Linear array of a 4x4 matrix.
 *  float* v1: 4-component homogeneous coordinates.
 * Return: None
 * Known Issues: None
 **/
void  vcbMatMult4fv(float * v, float * mat, float * v1);

/** Function
 * Name: vcbMatMultM
 * Version: 1.0
 * Description: vcbMatMultM mulitplies the 4x4 matrices "fmat" and "mat" together and places the result
 *  into "fmat". Matrix is assumed to be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float* fmat: Linear array of a 4x4 matrix. Result of multiplication will be stored here.
 * Return: None
 * Known Issues: None
 **/
void  vcbMatMultM(float *fmat, float * mat);

/** Function
 * Name: vcbOrtho3D
 * Version: 1.0
 * Description: vcbOrtho3D generates an orthogonal projection matrix as defined in the OpenGL Red Book.
 *  Note: This function is equivalent to the glOrtho function. Matrix is assumed to be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix for orthogonal projection matrix.
 *  float l: the left coordinate.
 *  float r: the right coordinate.
 *  float b: the bottom coordinate.
 *  float t: the top coordinate.
 *  float n: the near coordinate.
 *  float f: the far coordinate.
 * Return: None
 * Known Issues: None
 **/
void  vcbOrtho3D(float * mat, float l, float r, float b, float t, float n, float f);

/** Function
 * Name: vcbFrustum3D
 * Version: 1.0
 * Description: vcbFrustum3D generates a general purpose projection matrix as defined in the OpenGL Red Book.
 *  Note: This function is equivalent to the glFrustum function. Matrix is assumed to be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float l: the left coordinate.
 *  float r: the right coordinate.
 *  float b: the bottom coordinate.
 *  float t: the top coordinate.
 *  float n: the near coordinate.
 *  float f: the far coordinate.
 * Return: None
 * Known Issues: None
 **/
void  vcbFrustum3D(float * mat, float l, float r, float b, float t, float n, float f);

/** Function
 * Name: vcbPerspective3D
 * Version: 1.0
 * Description: vcbPerspective3D generates a perspective projection matrix as defined in the OpenGL Red Book.
 *  Note: This function is equivalent to the gluPerspective function. Matrix is assumed to be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float fovy: Angle of the field of view in the y-direction.
 *  float aspect: The aspect ration of the width of the height.
 *  float zNear: The near clipping plane.
 *  float zFar: The far clipping plane.
 * Return: None
 * Known Issues: None
 **/
void  vcbPerspective3D(float * mat, float fovy, float aspect, float zNear, float zFar);

/** Function
 * Name: vcbLookAt
 * Version: 1.0
 * Description: vcbLookAt generates the 4x4 matrix "mat" which is a modelview matrix that transforms
 *  world space to eye space. Matrix is assumed to be row-major.
 *  Note: This function is equivalent to the gluLookAt function.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float* eye: A 3-component vector specifing the desired viewpoint.
 *  float* center: A 3-component vector specifing any point along the desired line of sight.
 *  float* up: A 3-component vector specifing which direction is up.
 * Return: None
 * Known Issues: None
 **/
void  vcbLookAt(float * mat, float * eye, float * center, float * up);

/** Function
 * Name: vcbLookAtRotMat
 * Version: 1.0
 * Description: vcbLookAtRotMat generates the 4x4 matrix "mat" which is a rotation matrix. Matrix is
 *   assumed to be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  float* eye: A 3-component vector specifing the desired viewpoint.
 *  float* center: A 3-component vector specifing any point along the desired line of sight.
 *  float* up: A 3-component vector specifing which direction is up.
 * Return: None
 * Known Issues: None
 **/
void  vcbLookAtRotMat(float * mat, float * eye, float * center, float * up);

/** Function
 * Name: vcbViewPortMat
 * Version: 1.0
 * Description: vcbViewPortMat generates "mat" which is a transformation matrix for viewport transformation
 *  as in the OpenGL Red Book, where "scrWidth" is the screen width (in pixels) and "scrHeight" is
 *  the screen height (in pixels). Matrix is assumed to be row-major.
 * Arguments:
 *  float* mat: Linear array of a 4x4 matrix.
 *  int scrWidth: Screen width in pixels.
 *  int scrHeight: Screen height in pixels.
 * Return: None
 * Known Issues: None
 **/
void  vcbViewPortMat(float * mat, int scrWidth, int scrHeight);

#ifdef __cplusplus
}  /* extern C */
#endif


#endif
