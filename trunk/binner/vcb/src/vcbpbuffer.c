/***************************************************************************
 *   Copyright (C) 2004  Jian Huang                                        *
 *   seelab@cs.utk.edu                                                     *
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

#include "vcbpbuffer.h"
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>


static void compile_attriblst(int * iattributes, float * fattributes, unsigned int mode)
/* 
 * internal function to compile a list of attributes for choosing
 * framebuffer (glx) or pixelformat (windows)
 */
{
	int i, niattribs, nfattribs;
	/* Query for a suitable pixel format based on the specified mode. */
	niattribs = nfattribs = 0;

	/* 
	 * Attribute arrays must be "0" terminated - for simplicity, first
	 * just zero-out the array entire, then fill from left to right.
	 */
	for (i = 0; i < 2*VCB_MAX_ATTRIBS; iattributes[i] = 0, fattributes[i] = 0.f, i++ );

	/* 
	 * set mode to choose framebuffer (X) or pixelformat (windows)
	 * Since we are trying to create a pbuffer, the pixel format we
	 * request (and subsequently use) must be "p-buffer capable".
	 */
#ifndef WIN32_LEAN_AND_MEAN
	iattributes[2*niattribs  ] = GLX_DRAWABLE_TYPE;
    iattributes[2*niattribs+1] = GLX_PBUFFER_BIT;
	niattribs++;
#else
	iattributes[2*niattribs  ] = WGL_DRAW_TO_PBUFFER_ARB;
	iattributes[2*niattribs+1] = 1;
	niattribs++;
	iattributes[2*niattribs  ] = WGL_SUPPORT_OPENGL_ARB;
	iattributes[2*niattribs+1] = 1;
	niattribs++;
#endif

	if ( mode & GLUT_INDEX )
	{
#ifndef WIN32_LEAN_AND_MEAN
		iattributes[2*niattribs  ] = GLX_RENDER_TYPE;
		iattributes[2*niattribs+1] = GLX_COLOR_INDEX_BIT;
#else
		iattributes[2*niattribs  ] = WGL_PIXEL_TYPE_ARB;
		iattributes[2*niattribs+1] = WGL_TYPE_COLORINDEX_ARB;
#endif
		niattribs++;
	}
	else
	{
#ifndef WIN32_LEAN_AND_MEAN
		iattributes[2*niattribs  ] = GLX_RENDER_TYPE;
		iattributes[2*niattribs+1] = GLX_RGBA_BIT;
		niattribs++;
		iattributes[2*niattribs  ] = GLX_RED_SIZE;
		iattributes[2*niattribs+1] = 8;
		niattribs++;
		iattributes[2*niattribs  ] = GLX_GREEN_SIZE;
		iattributes[2*niattribs+1] = 8;
		niattribs++;		
		iattributes[2*niattribs  ] = GLX_BLUE_SIZE;
		iattributes[2*niattribs+1] = 8;
		niattribs++;		
		iattributes[2*niattribs  ] = GLX_ALPHA_SIZE;
		iattributes[2*niattribs+1] = 8;
#else
		iattributes[2*niattribs  ] = WGL_PIXEL_TYPE_ARB;
		iattributes[2*niattribs+1] = WGL_TYPE_RGBA_ARB;
#endif
		niattribs++;
	}

	if ( mode & GLUT_DOUBLE )
	{
#ifndef WIN32_LEAN_AND_MEAN
		iattributes[2*niattribs  ] = GLX_DOUBLEBUFFER;
		iattributes[2*niattribs+1] = 1;
#else
		iattributes[2*niattribs  ] = WGL_DOUBLE_BUFFER_ARB;
		iattributes[2*niattribs+1] = 1;
#endif
		niattribs++;
	}

	if ( mode & GLUT_DEPTH )
	{
#ifndef WIN32_LEAN_AND_MEAN
		iattributes[2*niattribs  ] = GLX_DEPTH_SIZE;
		iattributes[2*niattribs+1] = 24;
#else
		iattributes[2*niattribs  ] = WGL_DEPTH_BITS_ARB;
		iattributes[2*niattribs+1] = 1;
#endif
		niattribs++;
	}

	if ( mode & GLUT_STENCIL )
	{
#ifndef WIN32_LEAN_AND_MEAN
		iattributes[2*niattribs  ] = GLX_STENCIL_SIZE;
		iattributes[2*niattribs+1] = 8;
#else
		iattributes[2*niattribs  ] = WGL_STENCIL_BITS_ARB;
		iattributes[2*niattribs+1] = 1;
#endif		
		niattribs++;
	}

	if ( mode & GLUT_ACCUM )
	{
#ifndef WIN32_LEAN_AND_MEAN
		iattributes[2*niattribs  ] = GLX_ACCUM_RED_SIZE;
		iattributes[2*niattribs+1] = 8;
		niattribs++;
		iattributes[2*niattribs  ] = GLX_ACCUM_GREEN_SIZE;
		iattributes[2*niattribs+1] = 8;
		niattribs++;
		iattributes[2*niattribs  ] = GLX_ACCUM_BLUE_SIZE;
		iattributes[2*niattribs+1] = 8;
		niattribs++;
		iattributes[2*niattribs  ] = GLX_ACCUM_ALPHA_SIZE;
		iattributes[2*niattribs+1] = 8;
#else
		iattributes[2*niattribs  ] = WGL_ACCUM_BITS_ARB;
		iattributes[2*niattribs+1] = 1;
#endif
		niattribs++;
	}
}

VCBpbuffer vcbNewPbuffer(int w, int h, unsigned int pmode)
{
	int i;
	VCBpbuffer vcbp;

#ifdef WIN32_LEAN_AND_MEAN
   	HDC hdc;
	HGLRC hglrc;
	int format;
	int pformat[VCB_MAX_PFORMATS];
	unsigned int nformats;
#endif

    vcbp = (VCBpbuffer) malloc (sizeof(struct vrpbuf_struct));
    vcbp->mode = pmode;
    vcbp->width = w;
    vcbp->height = h;
	compile_attriblst(vcbp->iattributes, vcbp->fattributes, pmode);

#ifndef WIN32_LEAN_AND_MEAN

    /* display Creation */
    vcbp->Disp = XOpenDisplay(":0");
    if (vcbp->Disp == NULL)
       fprintf(stderr,"XOpenDisplay failed on %s",getenv("DISPLAY"));
    else 
       fprintf(stderr,"XOpenDisplay on %s succeeded\n", getenv("DISPLAY"));

    /* connect to screen */
    /* printf("got %d screens in display\n",ScreenCount(Disp));*/
    vcbp->screen = DefaultScreen(vcbp->Disp);
    vcbp->FBConfig = glXChooseFBConfig(vcbp->Disp, vcbp->screen, vcbp->iattributes, &vcbp->nconfig);
	if (vcbp->nconfig < 1)
	{
		fprintf( stderr, "pbuffer creation error:  couldn't find a suitable pixel format.\n" );
		return NULL;
	}

    /*printf("after glXChooseFB %d %d\n",(int)FBConfig, (int)FBConfig_Count);*/
 
    /* Create the Pbuffer */
    i=0;
    vcbp->iattributes[i++] = GLX_PRESERVED_CONTENTS;
    vcbp->iattributes[i++] = 0;
    vcbp->iattributes[i++] = GLX_PBUFFER_WIDTH;
    vcbp->iattributes[i++] = w;
    vcbp->iattributes[i++] = GLX_PBUFFER_HEIGHT;
    vcbp->iattributes[i++] = h;
    vcbp->iattributes[i++] = 0; 

	/* use the first FBconfig choice to create pbuffer and context */
	vcbp->Gwin = glXCreatePbuffer(vcbp->Disp, vcbp->FBConfig[0], vcbp->iattributes);
    vcbp->context = glXCreateNewContext(vcbp->Disp, vcbp->FBConfig[0], GLX_RGBA_TYPE, NULL, True);

#else
	vcbp->myDC = NULL;
    vcbp->myGLctx = NULL;
    vcbp->buffer = NULL;

	/*
	if (firsttime > 0) {
		if !(init_wgl_pbufprocs()) {
			fprintf(stderr,"error in vcbNewPbuffer, unable to get wgl pbuffer proc entry addresses\n");
			return NULL;
		}
		firstime = 0;
	}
*/
   	hdc = wglGetCurrentDC();
	if (hdc == NULL) {
		fprintf(stderr,"error in vcbNewPbuffer, unable to get GL device context\n");
		return NULL;
	}

	hglrc = wglGetCurrentContext();
	if (hglrc == NULL) {
		fprintf(stderr,"error in vcbNewPbuffer, unable to get GL render context\n");
		return NULL;
	}

	if (wglChoosePixelFormatARB == NULL) {
		fprintf(stderr,"failed to grab extension\n");
		return NULL;
	}

	/*wglChoosePixelFormatARB( hdc, vcbp->iattributes, vcbp->fattributes, VCB_MAX_PFORMATS, pformat, &nformats);*/
    wglChoosePixelFormatARB(hdc,(const int*)vcbp->iattributes, NULL, 1,pformat,&nformats);

	if (nformats < 1)
	{
		fprintf( stderr, "pbuffer creation error:  Couldn't find a suitable pixel format.\n" );
		return NULL;
	}

	format = pformat[0];
	/* Create the p-buffer */
	i = 0;
	vcbp->iattributes[i] = 0;
	vcbp->buffer = wglCreatePbufferARB(hdc,format,vcbp->width, vcbp->height, vcbp->iattributes );
	if ( !vcbp->buffer )
	{
		DWORD err = GetLastError();
		fprintf( stderr, "pbuffer creation error:  wglCreatePbufferARB() failed\n" );
		if ( err == ERROR_INVALID_PIXEL_FORMAT )
		{
			fprintf( stderr, "error:  ERROR_INVALID_PIXEL_FORMAT\n" );
		}
		else if ( err == ERROR_NO_SYSTEM_RESOURCES )
		{
			fprintf( stderr, "error:  ERROR_NO_SYSTEM_RESOURCES\n" );
		}
		else if ( err == ERROR_INVALID_DATA )
		{
			fprintf( stderr, "error:  ERROR_INVALID_DATA\n" );
		}
		return NULL;
	}
	
	/* Get the device context. */
	vcbp->myDC = wglGetPbufferDCARB(vcbp->buffer);
	if (!vcbp->myDC) {
		fprintf( stderr, "pbuffer creation error:  wglGetPbufferDCARB() failed\n" );
		return NULL;
	}
	
	/* Create a gl context for the p-buffer.*/
	vcbp->myGLctx = wglCreateContext(vcbp->myDC);
	if (!vcbp->myGLctx) {
		fprintf( stderr, "pbuffer creation error:  wglCreateContext() failed\n" );
		return NULL;
	}

	/* Determine the actual width and height we were able to create. */
	wglQueryPbufferARB(vcbp->buffer, WGL_PBUFFER_WIDTH_ARB, &vcbp->width );
	wglQueryPbufferARB(vcbp->buffer, WGL_PBUFFER_HEIGHT_ARB, &vcbp->height );
	
	fprintf( stderr, "created a %d x %d pbuffer\n", vcbp->width, vcbp->height );

#endif

	return vcbp;
}

void vcbPbufferModeSwitch(VCBpbuffer vcbp)
/* check to see if pbuffer is lost. if so, delete it and reinitialize */
{
#ifndef WIN32_LEAN_AND_MEAN

#else
	int lost = 0;
	
	wglQueryPbufferARB(vcbp->buffer, WGL_PBUFFER_LOST_ARB, &lost );
	
	if ( lost )
	{
		int w, h, mode;
		w = vcbp->width;
		h = vcbp->height;
		mode = vcbp->mode;
		vcbFreePbuffer(vcbp);
		vcbp = vcbNewPbuffer(w, h, mode);
	}
#endif
}

void vcbPbufferMakeCurrent(VCBpbuffer vcbp)
{
#ifndef WIN32_LEAN_AND_MEAN
	/* Link GLX context to PBuffer */
	if (!glXMakeCurrent(vcbp->Disp, vcbp->Gwin, vcbp->context)) {
#else
	if (!wglMakeCurrent( vcbp->myDC, vcbp->myGLctx)) {
#endif
		fprintf(stderr, "vrpbufMakeCurrent() failed\n" );
	}
}


void vcbFreePbuffer(VCBpbuffer vcbp)
{
#ifndef WIN32_LEAN_AND_MEAN
    glXDestroyContext (vcbp->Disp, vcbp->context);
	glXDestroyPbuffer(vcbp->Disp,vcbp->Gwin);
#else
   	if (vcbp->buffer)
	{
		wglDeleteContext(vcbp->myGLctx);
		wglReleasePbufferDCARB(vcbp->buffer,vcbp->myDC);
		wglDestroyPbufferARB(vcbp->buffer);
	}
#endif
   free(vcbp);
}
