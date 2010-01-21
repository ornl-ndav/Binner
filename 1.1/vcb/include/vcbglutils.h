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

#ifndef _VCB_GLUTILS_H
#define _VCB_GLUTILS_H

#include "glew.h"

/** Enum
 * Name: vcbMaterial
 * Version: 0.9
 * Description:
 *	This enumeration is used to easily set the material to be used for a rendering.
 * Elements:
	MATERIAL_DEFAULT: The default material
	MATERIAL_BRASS: Brass
	MATERIAL_BRONZE: Bronze
	MATERIAL_POLISHED_BRONZE: Polished Bronze
	MATERIAL_CHROME: Chrome
	MATERIAL_COPPER: Copper
	MATERIAL_POLISHED_COPPER: Polished Copper
	MATERIAL_GOLD: Gold
	MATERIAL_POLISHED_GOLD: Polished Gold
	MATERIAL_PEWTER: Pewter
	MATERIAL_SILVER: Silver
	MATERIAL_POLISHED_SILVER: Polished Silver
	MATERIAL_BLACK_PLASTIC: Black Plastic
	MATERIAL_BLACK_RUBBER: Black Rubber

	MATERIAL_EMERALD: Emerald (semi-transparent)
	MATERIAL_JADE: Jade (semi-transparent)
	MATERIAL_OBSIDIAN: Obsidian (semi-transparent)
	MATERIAL_PEARL: Pearl (semi-transparent)
	MATERIAL_RUBY: Ruby (semi-transparent)
	MATERIAL_TURQUOISE: Turquoise (semi-transparent)
 **/ 
typedef enum _eMat {
	MATERIAL_DEFAULT,
	MATERIAL_BRASS,
	MATERIAL_BRONZE,
	MATERIAL_POLISHED_BRONZE,
	MATERIAL_CHROME,
	MATERIAL_COPPER,
	MATERIAL_POLISHED_COPPER,
	MATERIAL_GOLD,
	MATERIAL_POLISHED_GOLD,
	MATERIAL_PEWTER,
	MATERIAL_SILVER,
	MATERIAL_POLISHED_SILVER,
	MATERIAL_BLACK_PLASTIC,
	MATERIAL_BLACK_RUBBER,

	/* transparent ones */
	MATERIAL_EMERALD,
	MATERIAL_JADE,
	MATERIAL_OBSIDIAN,
	MATERIAL_PEARL,
	MATERIAL_RUBY,
	MATERIAL_TURQUOISE,
} vcbMaterial;

#ifdef __cplusplus
extern "C" {
#endif

/** Directions for vcbglutils_shaded_streamlined_rendering
 *  <h2>Description:</h2>
 *  It is very difficult to see the effect of a one dimensional line 
 *  moving through a three dimensional space. Shaded Streamline Rendering 
 *  (SSR) provides a very useful way to see such a line and therefore 
 *  understand its relationship within the dataset. SSR works by giving 
 *  that line a texture and material that is lit accordingly by lights 
 *  within the scene. Being able to see how the shading of the texture 
 *  responds to those lights effectively allow you to understand the 
 *  line's motion within the scene. 
 *
 *  <h2>How to implement SSR in your project:</h2>
 *  Suprisingly, it takes little work to actually set up your scene 
 *  for SSR. First you will need to include vcbglutils.h. Next, you will 
 *  need to declare a GLuint global variable, lets call it texobj. 
 *  Then in your main function you will need to set texobj to equal 
 *  the value returned by vcbInitShadedStreamline(texunit, matid), 
 *  where "texunit" specifies which texture unit to use and matid is the
 *  vcbMaterial that you wish the object to be rendered with. 
 *  <p>
 *  If later during run time, with a different material property of the shaded
 *  streamline, just make another call to vcbInitShadedStreamline with a
 *  different material id and a texture unit number. Next, in your display
 *  function you need to declare a one dimensional matrix that is the size of
 *  sixteen floats; lets call it m_shadedstreamline. 
 *  Also in your display function declare two more one dimensional 
 *  matrices that are the size of three floats each, lets call them 
 *  cur_view and cur_light. These two matrices, cur_view and cur_light, 
 *  represent the current viewing vectors and current light vectors 
 *  (from one light source), respectively. In the code below we 
 *  use Arcball for an interface and the function vcbCalcViewLightVecs 
 *  to compute cur_view and cur_light. However it is not necessary 
 *  to use Arcball as an interface in SSR, nor is it necessary to 
 *  use vcbCalcViewLightVecs to compute  cur_view and cur_light as long
 *  as cur_view and cur_light contain the proper values. Next comes the magic,
 *  call vcbUpdateTexMatShadedStreamline(m_shadedstreamline, cur_view, cur_light)
 *  where m_shadedstreamline, cur_view, and cur_light are the matrices from above.
 *  Lastly, multiply the texture matrix by m_shadedstreamline. When this texture 
 *  is applied to the object to be rendered you will see the effects of SSR.
 * 
 *  <h2>Example of the above using GLUT code:</h2>
 *  <pre>
 * #include &lt;ArcBall.h&gt;
 * #include &lt;vcbglutils.h&gt;
 * int height = 800;
 * int width = 600;
 *
 * GLuint texobj;
 * ArcBall Spaceball;
 * 
 * int main(int argc, char **argv) {
 * 	init();
 * 	&#47;* your code here *\/
 * 	glutDisplayFunc(display);
 * 	glutReshapeFunc(reshape);
 * 	glutIdleFunc(display);
 * 	glutMouseFunc(mouse_handler);
 * 	glutMotionFunc(trackMotion);
 * 	texobj = vcbInitShadedStreamline(GL_TEXTURE0_ARB, MATERIAL_BRASS); &#47;* Any material will work *\/
 * 	glutMainLoop();
 * 	&#47;* your code here *\/
 * }
 * 
 * void init(void) {
 * 	&#47;* your code here *\/
 * 	Spaceball.SetWidth(width);
 * 	Spaceball.SetHeight(height);
 * 	&#47;* your code here *\/
 * }
 * 
 * void reshape(void) {
 * 	&#47;* your code here *\/
 * 	Spaceball.SetWidth(width);
 * 	Spaceball.SetHeight(height);
 * 	&#47;* your code here *\/
 * }
 * 
 * void display(void) {
 * 	float cur_view[3], cur_light[3];
 * 	float m_shadedstreamline[16];
 * 	&#47;* your code here *\/
 * 	Spaceball.Update();
 * 	vcbCalcViewLightVecs(Spaceball.GetMatrix(),
 * 			     &#47;* coordinates of the eye *\/,
 * 			     &#47;* coordinates of the center of the scene *\/,
 * 			     &#47;* coordinates of the light *\/
 * 			     cur_view,
 * 			     cur_light);
 * 	vcbUpdateTexMatShadedStreamline(m_shadedstreamline, cur_view, cur_light);
 * 	glMatrixMode(GL_TEXTURE);
 * 	glLoadIdentity();
 * 	glMultiMatrix(m_shadedstreamline);
 * 	&#47;* your code here *\/
 * 	glMultMatrix(Spaceball.GetMatrix());
 * 	&#47;* your code here *\/
 * }
 * 
 * void mouse_handler(int button, int bstate, int x, int y) {
 * 	&#47;* your code here *\/
 * 	if (button == GLUT_LEFT_BUTTON) {
 * 		if (bstate == GLUT_DOWN) {
 * 			Spaceball.StartMotion(x, y, glutGet(GLUT_ELAPSED_TIME));
 * 		}
 * 		else if (bstate == GLUT_UP) {
 * 			Spaceball.StopMotion(x, y, glutGet(GLUT_ELAPSED_TIME));
 * 		}
 * 	}
 * }
 * 
 * void trackMotion(int x, int y) {
 * 	&#47;* your code here *\/
 * 	if (Spaceball.Track())
 * 		Spaceball.TrackMotion(x, y, glutGet(GLUT_ELAPSED_TIME));
 * }
 * </pre>
 **/
	
/** Function
 * Name: vcbGetOGLMaterial
 * Version: 0.91
 * Description: 
 *  This function places material values of the material located at
 *  "matid" into "matparam"; see detailed description for material values.
 *  <br>
 *  This function looks up a precompiled list of material parameters specified 
 *  by "matid".  The result is that a list of RGBA coefficients for ambient,
 *  diffuse, specular, and specular exponential, in this order, is placed in 
 *  "matparam".  The "matparam" is assumed to have storage for at least
 *  13 floats.  If for certain materials the alpha channel is not preset, a 
 *  value of 1.0 is put in the corresponding location.
 *  <br>	
 *  Outside the function, after the "vcbGetOGLMaterial" call, materials can be set as&#58;<pre>
 *	GLfloat * amb, *dif, *spec, *exponent;
 *	GLenum face;
 *	amb  = materialparam;
 *	dif  = amb + 4;
 *	spec = dif + 4;
 *	exponent = spect + 4;
 *	glMaterialfv(face, GL_AMBIENT,    amb );
 *	glMaterialfv(face, GL_DIFFUSE,    dif );
 *	glMaterialfv(face, GL_SPECULAR,   spec );
 *	glMaterialf (face, GL_SHININESS, *exponent);
 *	</pre>
 * Arguments:
 *  vcbMaterial matid: Enumerated material id. 
 *  GLfloat* matparam: Output RGBA coefficients for ambient,
 *   diffuse, specular, and specular exponential, respectively.<br> 
 *   "matparam" is assumed to have storage for at least 13 floats.  
 * Return: None 
 * Known Issues: None
 **/
void vcbGetOGLMaterial(vcbMaterial matid, GLfloat * matparam);


/** Function
 * Name: vcbSetOGLMaterial
 * Version: 0.91
 * Description: 
 *  This function sets the material of "face" to the material located at 
 *  "materialID" with the alpha value of "alpha".
 *
 *  This is a utility function that uses "vcbGetOGLMaterial" to set the ogl 
 *  material for a given "face" using RGBA color values.  This
 *  function is primarily meant to be used for time-saving purposes, as well as 
 *  to show how vcbGetOGLMaterial should be used.  Vis
 *  algorithms that do not rely on ogl materials to perform shading can use 
 *  "vcbGetOGLMaterial" for a simple lookup and use the retrieved
 *  parameters however desired.
 *  <p>
 *  To determine what the value of "materialId" should be in order to 
 *  place the material values you wish onto "face", see vcbMaterial or refer to
 *  the following list&#58;<br> 
 *	==================================================================<br><table>
 *	<TR><TH> materialId   <TH> material&#58;
 *	<TR><TD> 0			  <TD> Default
 *	<TR><TD> 1			  <TD> Brass
 *	<TR><TD> 2			  <TD> Bronze
 *	<TR><TD> 3			  <TD> Polished Bronze
 *	<TR><TD> 4			  <TD> Chrome
 *	<TR><TD> 5			  <TD> Copper
 *	<TR><TD> 6			  <TD> Polished Copper
 *	<TR><TD> 7			  <TD> Gold
 *	<TR><TD> 8			  <TD> Polished Gold
 *	<TR><TD> 9			  <TD> Pewter
 *  <TR><TD> 10			  <TD> Silver
 *	<TR><TD> 11			  <TD> Polished Silver
 *	<TR><TD> 12			  <TD> Black Plastic
 *	<TR><TD> 13			  <TD> Black Rubber
 *	<TR><TD> 14			  <TD> Emerald
 *	<TR><TD> 15			  <TD> Jade
 *	<TR><TD> 16			  <TD> Obsidian
 *	<TR><TD> 17			  <TD> Pearl
 *	<TR><TD> 18			  <TD> Ruby
 *	<TR><TD> 19			  <TD> Turquoise </table>
 *	================================================================== 
 * Arguments:
 *  GLenum face: Output material from materialId and alpha.
 *  vcbMaterial materialId: Enumerated material id. 
 *  float alpha: The alpha value for face. 
 * Return: None 
 * Known Issues: None
 **/
void vcbSetOGLMaterial(GLenum face, vcbMaterial materialId, float alpha);

/** Function
 * Name: vcbEnableOGLlight
 * Version: 0.91
 * Description:
 *  This is convenience function for enabling opengl lighting in general. <br>
 *  It is usually, not sufficient to call this function to enable lighting.
 *  Subsequently, one must call vcbSetOGLlight to enable each specific light
 *  source.
 * Arguments:
 *  float ambient: Value between 0 and 1.0 for setting global ambient light intensity.
 * Return: None
 * Known Issues: None
 **/
void vcbEnableOGLlight(float ambient);

/** Function
 * Name: vcbSetOGLlight
 * Version: 0.91
 * Description:
 *  This is a convenience function for enabling a specific light source, as specified
 *  by "light", such as GL_LIGHT0. <br>
 *  Usage&#58;
 *   For example, call this at the beginning of the display function <pre>
 *	vcbEnableOGLlight(0.1f);
 *	vcbSetOGLlight(GL_LIGHT0, 0.1, 0.8, lightpos_x, lightpos_y, lightpos_z, 40.f); </pre><br>
 *  Note&#58; If using arcball to control user movement, calling these two light functions before the arcball
 *       calls in your display function results in a light source rotating with user. Putting them after arcball
 *       calls makes the light source stationary.
 * Arguments:
 *  GLenum light: An OpenGL light. i.e. GL_LIGHT0...
 *  float ambient: The ambient component of this light source (always assume white colored light)
 *  float intensity: The diffusion and specular components of this light source (white light)
 *  float posx: The X coordinate of the position of the light source.
 *  float posy: The Y coordinate of the position of the light source.
 *  float posz: The Z coordinate of the position of the light source.
 *  float spot_expon: The Phong exponential term for the specular component.
 * Return: None
 * Known Issues: None
 **/
void vcbSetOGLlight(GLenum light, float ambient, float intensity, float posx, float posy, float posz, float spot_expon);

/** Function
 * Name: vcbIsPowerOf2
 * Version: 0.91
 * Description: 
 *  This is a utility function that determines if a given integer "x" is a 
 *  power of 2 or not; returns 1 if true, 0 if false.
 *  Note&#58; 1 and 2 are both powers of 2.
 * Arguments:
 *  int x: Integer to be tested whether or not it is a power of two.
 * Return: int; 1 if "x" is power of two or 0 if it is not.
 * Known Issues: None
 **/
int vcbIsPowerOf2 (int x);

/** Function
 * Name: vcbGetOGLVersion
 * Version: 0.91
 * Description: 
 *  Returns a floating point value, Ex. 1.4, of the current Opengl version running on the local machine.
 * Arguments: None
 * Return: float; Version of OpenGL
 * Known Issues: None
 **/
float vcbGetOGLVersion(void);


/** Function
 * Name: vcbCalcViewLightVecs
 * Version: 0.91
 * Description:
 *  When using ArcBall, use this function to compute light vectors for a headlamp.
 *  Use this function to compute the current viewing and lighting vectors after 
 *  rotation. This function assumes that you are using a headlight
 *  (ie. light vectors that rotate with the viewing vectors).  
 *  Authors&#58; Nathan Fout, Jeff Burton, Jian Huang
 * Arguments:
 *  float* m: The xform matrix from arcball.
 *  float* eyepos: Eye position.
 *  float* center: Center position.
 *  float* lightpos: Light position.
 *  float* vdir: Viewing direction.
 *  float* ldir: Light direction.
 * Return: None
 * Known Issues: None
 **/
void vcbCalcViewLightVecs(float *m, 
			  float * eyepos, 
			  float * center, 
			  float * lightpos,
			  float * vdir,
			  float * ldir);


/** Function
 * Name: vcbUpdateTexMatShadedStreamline
 * Version: 0.91
 * Description:
 *  This function updates the values in "texmat" that should then be applied to 
 *  your texture matrix to use Shaded Streamline Rendering.
 *  This function uses the input arguments to transform the texture matrix accordingly.<br>
 *  Note&#58; After this function call you should multiply your current texture matrix by the value 
 *  of the variable you passed to "texmat" so that your object is properly rendered.
 * Arguments:
 *  float* texmat: Pointer to storage of resulting texture matrix.
 *  float* vdir: Viewing direction.
 *  float* ldir: Light direction.
 * Return: None
 * Known Issues: None
 **/
void vcbUpdateTexMatShadedStreamline(float * texmat, 
				     float * vdir, 
				     float * ldir);

/** Function
 * Name: vcbBindOGLTexture2D
 * Version: 0.91
 * Description:
 *  This function eases the manual work of setting up a texture.  
 *  Each call uses system resources to bind the texture image to a texture unit.
 *  <br>
 *  It should be noted that the values passed to vcbBindOGLTexture2D are subsequently 
 *  passed to their respective OpenGL functions.  As such, for a full description 
 *  of the capabilites of this function one should also see the OpenGL Red Book.  
 *  Therefore, all input parameters are cross referenced with their relevant pages 
 *  in the OpenGL Red Book Third Edition.
 * Arguments:
 *  int texunit: The active texture unit, pp. 394.
 *  int interp: The type of interpolation to use to apply the texture to your 
 *    object, pp. 361.
 *  int informat: The luminance, intensity, or RGBA components that are used 
 *    to describe the texels of the image, pp. 361.
 *  int format: Format of the texture image data, pp. 361
 *  int valtype: Data type of the texture image data, pp. 361.
 *  int width: The horizontal size of the texture image, pp. 361
 *  int height: The vertical size of the texture image, pp. 361.
 *  GLfloat tex_envmode: The texturing function used, pp. 402.
 *  const Glfloat* texmatrix: The current texture matrix being used, pp. 420. <br> 
 *    If "texmatrix" is set to NULL then no texture matrix update is needed, which 
 *    should be the most common case.  
 *  const GLvoid* pixels: Contains the actual texture image data, pp. 361.
 *
 * Return: int; the texture object id.
 * Known Issues: 
 *  Repeatedly calling the vcbBindOGLtexture*D functions may deplete system resources 
 *  and cause the OS kernel to freeze on systems with limited system memory. 
 *  In this case, if you wish to simply replace a texture unit by a new texture, 
 *  see the vcbUpdateOGLtexture2D function.
 **/    
int vcbBindOGLTexture2D(int texunit, 
                     int interp, int informat, int format, int valtype,
                     int width, int height, 
                     const GLvoid* pixels, 
                     GLfloat tex_envmode,
                     const GLfloat * texmatrix);

/** Function
 * Name: vcbBindOGLTexture1D
 * Version: 0.91
 * Description:
 *  This function eases the manual work of setting up a texture.  
 *  Each call uses system resources to bind the texture image to a texture unit.
 *  <br>
 *  It should be noted that the values passed to vcbBindOGLTexture1D are subsequently 
 *  passed to their respective OpenGL functions.  As such, for a full description 
 *  of the capabilites of this function one should also see the OpenGL Red Book.  
 *  Therefore, all input parameters are cross referenced with their relevant pages 
 *  in the OpenGL Red Book Third Edition.
 *
 * Arguments:
 *  int texunit: The active texture unit, pp. 394.
 *  int interp: The type of interpolation to use to apply the texture to your 
 *    object, pp. 361.
 *  int informat: The luminance, intensity, or RGBA components that are used 
 *    to describe the texels of the image, pp. 361.
 *  int format: Format of the texture image data, pp. 361
 *  int valtype: Data type of the texture image data, pp. 361.
 *  int length: The size of the texture image.
 *  GLfloat tex_envmode: The texturing function used, pp. 402.
 *  const Glfloat* texmatrix: The current texture matrix being used, pp. 420. <br> 
 *    If "texmatrix" is set to NULL then no texture matrix update is needed, which 
 *    should be the most common case.  
 *  const GLvoid* pixels: Contains the actual texture image data, pp. 361.
 *
 * Return: int; the texture object id.
 * Known Issues: 
 *  Repeatedly calling the vcbBindOGLtexture*D functions may deplete system resources 
 *  and cause the OS  kernel to freeze on systems with limited system memory. 
 *  In this case, if you wish to simply replace a texture unit by a new texture, 
 *  see the vcbUpdateOGLtexture1D function.
 **/
int vcbBindOGLTexture1D(int texunit, 
                     int interp, int informat, int format, int valtype,
                     int length, 
                     const GLvoid* pixels, 
                     GLfloat tex_envmode,
                     const GLfloat * texmatrix);

/** Function
 * Name: vcbUpdateOGLTexture2D
 * Version: 0.91
 * Description:
 *  The vcbUpdateOGLtexture2D function eases the manual work of changing
 *  the texture for a texture unit that has already been configured using the
 *  corresponding vcbBindOGLtexture2D function. Calls to the vcbBindOGLtexture*D
 *  functions require system resources, so using vcbUpdateOGLtexture functions
 *  helps reuse memory and prevent system hangups.
 *
 * Arguments:
 *  int thisTexObj: The texture object id returned by the corresponding vcbBindOGLtexture*D function.
 *  int texunit: The active texture unit, pp. 394.
 *  int interp: The type of interpolation to use to apply the texture to your 
 *    object, pp. 361.
 *  int informat: The luminance, intensity, or RGBA components that are used 
 *    to describe the texels of the image, pp. 361.
 *  int format: Format of the texture image data, pp. 361
 *  int valtype: Data type of the texture image data, pp. 361.
 *  int width: The horizontal size of the texture image, pp. 361
 *  int height: The vertical size of the texture image, pp. 361.
 *  GLfloat tex_envmode: The texturing function used, pp. 402.
 *  const Glfloat* texmatrix: The current texture matrix being used, pp. 420.  
 *    If "texmatrix" is set to NULL then no texture matrix update is needed, which 
 *    should be the most common case.  
 *  const GLvoid* pixels: Contains the actual texture image data, pp. 361.
 *
 * Return: int; The texture object id specified by "thisTexObj".
 * Known Issues: None
 **/    
int vcbUpdateOGLTexture2D(int thisTexObj, int texunit, 
                       int interp, int informat, int format, int valtype,
                       int width, int height, 
                       const GLvoid* pixels, 
                       GLfloat tex_envmode,
                       const GLfloat * texmatrix);

/** Function
 * Name: vcbUpdateOGLTexture1D
 * Version: 0.91
 * Description:
 *  The vcbUpdateOGLtexture1D function eases the manual work of changing
 *  the texture for a texture unit that has already been configured using the
 *  corresponding vcbBindOGLtexture*D functions. Calls to the vcbBindOGLtexture*D
 *  functions require system resources, so using vcbUpdateOGLtexture functions
 *  helps reuse memory and prevent system hangups.
 *
 * Arguments:
 *  int thisTexObj: The texture object id returned by the corresponding bindOGLtexture function.
 *  int texunit: The active texture unit, pp. 394.
 *  int interp: The type of interpolation to use to apply the texture to your 
 *    object, pp. 361.
 *  int informat: The luminance, intensity, or RGBA components that are used 
 *    to describe the texels of the image, pp. 361.
 *  int format: Format of the texture image data, pp. 361
 *  int valtype: Data type of the texture image data, pp. 361.
 *  int length: The size of the texture image, pp. 361
 *  GLfloat tex_envmode: The texturing function used, pp. 402.
 *  const Glfloat* texmatrix: The current texture matrix being used, pp. 420. <br> 
 *    If "texmatrix" is set to NULL then no texture matrix update is needed, which 
 *    should be the most common case.  
 *  const GLvoid* pixels: Contains the actual texture image data, pp. 361.
 *
 * Return: int; The texture object id specified by "thisTexObj".
 * Known Issues: None
 **/   
int vcbUpdateOGLTexture1D(int thisTexObj, int texunit, 
                       int interp, int informat, int format, int valtype,
                       int length, 
                       const GLvoid* pixels, 
                       GLfloat tex_envmode,
                       const GLfloat * texmatrix);

/** Function
 * Name: vcbGenTextureShadedStreamline
 * Version: 0.91
 * Description:
 *  This function generates a Shaded Streamline Texture to be applied to the 
 *  object in your OpenGL application. <br>
 *  This is the magic of Shaded Streamline Rendering.  As such, you shouldn't 
 *  need to call this function directly in your code.  It is used by
 *  "vcbInitShadedStreamline" to set up the values of the shading texture 
 *  that is used during the Shaded Streamline Rendering.
 * Arguments:
 *  float* texmat: The texture matrix that will be used.
 *	vcbMaterial matid: The vcbMaterial will be be used to render the object.
 *  float* shadingtex: Shading texture.
 *  int texres: The resolution of the texture being used.
 * Return: None
 * Known Issues: None
 **/
void vcbGenTextureShadedStreamline (vcbMaterial matid, float * texmat, float * shadingtex, int texres);

/** Function
 * Name: vcbInitShadedStreamline
 * Version: 0.91
 * Description:
 *  This function sets up your OpenGL scene for Shaded Streamline Rendering.
 *  This function needs to be called in your main function in order to properly 
 *  set up your scene for rendering.   <br>
 *  The return value is a handler returned by glGenTextures call. Also if at 
 *  run time, one needs to vary the material property of the shaded streamline,
 *  just make another call to vcbInitShadedStreamline with a different material
 *  id and a texture unit number.
 * Arguments:
 *  int texunit: Specifies which texture unit to use, for instance GL_TEXTURE0_ARB.
 *  vcbMaterial matid: The vcbMaterial will be be used to render the object.
 * Return: int; handler returned by glGenTextures call.
 * Known Issues: None
 **/
int vcbInitShadedStreamline(int texunit, vcbMaterial matid);

/** Function
 * Name: vcbOGLprint
 * Version: 0.91
 * Description:
 *  This function prints a text string "instring" to the OpenGL screen at location (x,y)
 *  in a color specified by (r,g,b).
 *  <br>
 *  Also, one must turn off all texture units that affect RGBA channels. If
 *  multiple texture units are being used, they must be turned of one by one.
 *  <br>
 *  Example&#58;<br>
 *    Here we turn off two texture units, printf a message
 *    at the lower-left corner, and then re-enable the texture
 *    units&#58;<br><pre>
 *	glActiveTexture(GL_TEXTURE0);
 *	glDisable(GL_TEXTURE_1D);
 *	glActiveTexture(GL_TEXTURE1);
 *	glDisable(GL_TEXTURE_2D);
 *	vcbOGLprint(0.01f,0.01f, 1.f, 1.f, 0.f, echofps);
 *	glEnable(GL_TEXTURE_2D);
 *	glActiveTexture(GL_TEXTURE0);
 *	glEnable(GL_TEXTURE_1D);</pre>
 *
 * Arguments:
 *  float x: x coordinate for location of string to be printed. Must be in the
 *    range of 0 and 1, with (0,0) being the lower-left corner.
 *  float y: y coordinate for location of string to be printed. Must be in the
 *    range of 0 and 1, with (0,0) being the lower-left corner.
 *  float r: Red portion of color that string will be printed in. Same as red in glColor3f().
 *  float g: green portion of color that string will be printed in. Same as green in glColor3f().
 *  float b: blue portion of color that string will be printed in. Same as blue in glColor3f().
 *  char* instring: This is the char pointer to the string that will be displayed.<br>
 *   "instring" must be NULL terminated, e.g. sprintf produces NULL terminted strings.
 * Return: None
 * Known Issues: None
 **/
void vcbOGLprint(float x, float y, float r, float g, float b, char *instring);

/** Function:
 * Name: vcbDrawAxis
 * Version: 0.92
 * Description:
 *  This function displays the axis which indicates the current
 *  transformation in the bottom left corner of the current window.
 *  It gives the axis its own camera and projection settings, which
 *  is necessary for this to work arbitrarily in all applications. 
 * Arguments:
 *   GLfloat* m: The current transformation barring the camera settings. (array
 *               of 16 elements) <br> This is not the current modelview matrix,
 *               but rather the matrix kept up by a trackball routine, which
 *               the modelview matrix is multiplied by.
 *   int size: The size of the axis to be drawn. Subsequent calls of
 *             vcbDrawAxis with a different size will create a new list and
 *             draw the axis with the new size.
 * Return: None
 * Known Issues: None
 **/
void vcbDrawAxis(GLfloat m[16], int size);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif

