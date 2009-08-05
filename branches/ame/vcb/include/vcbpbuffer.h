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

#ifndef _VCB_PBUFFER_H
#define _VCB_PBUFFER_H

#include "glew.h"

#if defined(_WIN32) || defined(__CYGWIN__) || defined(__MINGW32__) && !defined(APIENTRY)
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include "wglew.h"
#endif

#ifndef WIN32_LEAN_AND_MEAN
  #include <GL/glx.h>
  #include <GL/glxtokens.h>
#endif

#ifndef VCB_MAX_PFORMATS
#define VCB_MAX_PFORMATS 256
#endif
#ifndef VCB_MAX_ATTRIBS
#define VCB_MAX_ATTRIBS  32
#endif

/** Directions for vcbpbuffer
 * <h2>Description:</h2>
 *  By using a pbuffer it is possible to do off screen rendering in OpenGL. 
 * The pbuffer is roughly analogous to the frame buffer of a machine that would
 * otherwise be displaying to a monitor. Any visualization algorithm that can use
 * OpenGL hardware acceleration can use a pbuffer. For instance, a batch processing
 * run that uses OpenGL hardware to generate a movie is an application of pbuffers.
 *
 * <h2>How to Implement vcbpbuffer in your project:</h2>
 * First, since the VCB implementation of pbuffer uses the file vcbpbuffer.h you will
 * need to include that file in your project. Next, declare a VCBpbuffer object;
 * let's call it pbuff. After your scene is initialized, make a call to
 * vcbNewPbuffer(w,h,mode) where w and h are the width and height of the VCBpbuffer
 * you wish to create, respectively, and mode is the OpenGL parameters you wish your
 * VCBpbuffer to have. If you are using GLUT then examples of mode are: GLUT_RGBA or
 * GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH, ect. After this it is a good idea to check if
 * pbuff is equal to NULL because that would mean your VCBpbuffer failed to initialize.
 * Next, you will need to call vcbPbufferMakeCurrent(pbuff) which makes the VCBpbuffer
 * pbuff be the current VCBpbuffer that your scene will render to. Now just render your
 * scene as you normally would. When you are finished, make a call to vcbFreePbuffer(pbuff)
 * to free up the memory your VCBpbuffer was using.
 * 
 * <h2>Example of the above using GLUT code:</h2><pre>
 * #include "vcbpbuffer.h"
 * #include &lt;GL/glut.h&gt;
 * #include &lt;stdlib.h&gt;
 * #include &lt;stdio.h&gt;
 * 
 * int main (int argc, char ** argv)
 * {
 * 	VCBpbuffer pbuff;  &#47;* Declare your VCBpbuffer object *\/
 * 	unsigned char * pixels;
 * 
 * 	int w = 512, h = 512;  &#47;* the width and height of your pbuffer *\/
 * 	
 * #ifdef _WIN32
 * 	&#47;* there is no need to use glut to initialize pbuffer on linux *\/
 * 	int winid;
 * 	glutInit(&argc, argv);
 * 	glutInitDisplayMode(GLUT_DOUBLE| GLUT_RGBA | GLUT_DEPTH);
 * 	glutInitWindowSize(w,h);
 * 	winid = glutCreateWindow(argv[0]);
 * #endif
 * 
 * 	printf("initialize \n");
 * 
 * 	&#47;* always initialize OGL extensions *\/
 * 	if (initOGLext() != 0)
 * 		fprintf(stderr,"extgl error, initialization failed\n");
 * 
 * 	&#47;* make your VCBpbuffer *\/
 * 	pbuff =  vcbNewPbuffer(w,h,GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);
 * 
 * 	if (pbuff == NULL) {
 * 		fprintf(stderr, "failed to create pbuffer, quitting \n");
 * 		return 0;
 * 	}
 * 
 * 	&#47;* set the current pbuffer to pbuff *\/
 * 	vcbPbufferMakeCurrent(pbuff);
 * 
 * 	&#47;* RENDER YOUR SCENE HERE *\/	
 * 
 * 	vcbFreePbuffer(pbuff);
 * 
 * 	return 1;
 * }</pre>
 **/


/** Struct
 * Name: VCBpbuffer
 * Version: 0.9
 * Description:
 *  typedefined&#58; * VCBpbuffer <br>
 *  VCBpbuffer is the structure that holds the values for using Visual Recipes 
 *  pbuffer routines.
 * Elements:
 *  int width: width of the pbuffer
 *  int height: height of the pbuffer
 *  int iattributes[2*VCB_MAX_ATTRIBS]:	integer attributes for the pbuffer
 *  float fattributes[2*VCB_MAX_ATTRIBS]: floating point attributes for the pbuffer 
 *  unsigned int mode: glut flags indicating the type of pbuffer 
 *  int nconfig: number of configs returned 
 *
 *  int screen: For the X version.
 *  GLXContext context: For the X version.
 *  GLbyte* pixels: For the X version.
 *  GLXPbuffer Gwin: For the X version.
 *  Display* Disp: For the X version.
 *  GLXFBConfig* FBConfig: Config pointer for Frame Buffer. For the X version.
 *
 *  HDC myDC: Handle to a device context.
 *  HGLRC myGLctx: Handle to a GL context.
 *  HPBUFFERARB buffer: Handle to a pbuffer.
 **/
typedef struct vrpbuf_struct
{
	int          width;							/* width of the pbuffer */
	int          height;						/* height of the pbuffer */
	int          iattributes[2*VCB_MAX_ATTRIBS];	/* integer attributes for the pbuffer */
	float        fattributes[2*VCB_MAX_ATTRIBS];	/* floating point attributes for the pbuffer */
	unsigned int mode;							/* glut flags indicating the type of pbuffer */
    int          nconfig;						/* number of configs returned */

#ifndef  WIN32_LEAN_AND_MEAN
    int          screen;						/* the X version */
    GLXContext   context; 
    GLbyte      *pixels;
    GLXPbuffer   Gwin; 
    Display     *Disp; 
    GLXFBConfig *FBConfig;						/* config pointer for Frame Buffer */
#else
	HDC          myDC;							/* Handle to a device context. */
    HGLRC        myGLctx;						/* Handle to a GL context. */
	HPBUFFERARB  buffer;						/* Handle to a pbuffer. */
#endif

} * VCBpbuffer;

#ifdef __cplusplus
extern "C" {
#endif

/** Function
 * Name: vcbNewPbuffer
 * Version: 0.91
 * Description: 
 *  vcbNewPbuffer initializes a VCBpbuffer struct.
 * Arguments:
 *  int w: The width of the pbuffer
 *  int h: The height of the pbuffer
 *  unsigned int pmode: The OpenGL modes that you wish to use. Ex. GLUT_RGBA,
 *   GLUT_DEPTH, ect.
 * Return: VCBpbuffer; The newly initialized VCBpbuffer.
 * Known Issues: None
 **/
VCBpbuffer vcbNewPbuffer(int w, int h, unsigned int pmode);

/** Function
 * Name: vcbPbufferModeSwitch
 * Version: 0.9
 * Description: 
 *  vcbPbufferModeSwitch checks to see if a pbuffer is lost.  If so, it deletes 
 *  it and reinitializes it.
 * Arguments:
 *  VCBpbuffer vcbp: pbuffer that will be checked.
 * Return: None
 * Known Issues: None
 **/
void vcbPbufferModeSwitch(VCBpbuffer vcbp);

/** Function
 * Name: vcbPbufferMakeCurrent
 * Version: 0.9
 * Description: 
 *  vcbPbufferMakeCurrent sets the current working pbuffer to be the VCBpbuffer 
 *  specified in the input argument.
 * Arguments:
 *  VCBpbuffer vcbp: pbuffer to set as current working pbuffer.
 * Return: None
 * Known Issues: None
 **/
void vcbPbufferMakeCurrent(VCBpbuffer vcbp);

/** Function
 * Name: vcbFreePbuffer
 * Version: 0.9
 * Description: 
 *  vcbFreePbuffer frees from memory the VCBpbuffer specified in the input 
 *  argument.
 * Arguments:
 *  VCBpbuffer vcbp: pbuffer to free.
 * Return: None
 * Known Issues: None
 **/
void vcbFreePbuffer(VCBpbuffer vcbp);


#ifdef __cplusplus
} /*end of extern C */
#endif

#endif
