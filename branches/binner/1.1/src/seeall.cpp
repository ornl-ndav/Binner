#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "glew.h"
#include <glut.h>

#include "vcbutils.h"
#include "ArcBall.h"
#include "vcbmath.h"
#include "vcbglutils.h"
#include "vcblinalg.h"
#include "vcbmcube.h"
#include "vcbimage.h"

#include "binnerio.h"

#include "atypes.h"

/**
 * $Id$
 *
 */

char projmode = 'O'; /* P for perspective, O for orthogonal */
projStruct proj;
viewStruct view;
lightStruct light0;

ArcBall spaceball;
int iwinWidth;
int iwinHeight;

int     mainwin, sidewin;

/* application specific data starts here */
int     nverts, nfacets;
float   vdata[1024];

/* end of application specific data */

bbox abbox;

char inputbuf[1024];

int list_id;

void display (void)
{
	int i;

	spaceball.Update();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslated(view.center[0],view.center[1],view.center[2]);
	glMultMatrixf(spaceball.GetMatrix());
	glTranslated(-view.center[0],-view.center[1],-view.center[2]);

	/* if want wireframe mode, then use the following */
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glCallList(list_id);

    vcbDrawAxis(spaceball.GetMatrix(), 100);

	glPopMatrix();
	glutSwapBuffers();
}


void cleanup (void)
{
	printf("done with app clean up, ogl version %2.1f ...\n",vcbGetOGLVersion());
	fflush(stdout);
}

int main(int argc, char ** argv)
{
  vcbdatatype dtype;
  int   i, n, ndims, orig, nvals, dummy;

  mainwin = initGLUT(argc,argv);
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr,"glew error, initialization failed\n");
    fprintf(stderr,"Glew Error: %s\n", glewGetErrorString(err)); 
  } 

  abbox.low[0] = abbox.low[1] = abbox.low[2] = 1e6;
  abbox.high[0] = abbox.high[1] = abbox.high[2] = -1e6;
  
  list_id = glGenLists(1);
  
  glNewList(list_id, GL_COMPILE);
  
  for (nfacets = 0; (n = get_polygonf(vdata)) > 0; nfacets ++) {
    for (i = 0; i < n; i ++) {
		abbox.low [0] = VCB_MINVAL(abbox.low [0],vdata[i*3]);
		abbox.high[0] = VCB_MAXVAL(abbox.high[0],vdata[i*3]);
		abbox.low [1] = VCB_MINVAL(abbox.low [1],vdata[i*3+1]);
		abbox.high[1] = VCB_MAXVAL(abbox.high[1],vdata[i*3+1]);
		abbox.low [2] = VCB_MINVAL(abbox.low [2],vdata[i*3+2]);
		abbox.high[2] = VCB_MAXVAL(abbox.high[2],vdata[i*3+2]);
    }

	glBegin(GL_POLYGON);
	for (i = 0; i < n; i ++)
		glVertex3fv(&vdata[i*3]);
	glEnd();
  }
  glEndList();

  printf("bounding box: (%f %f %f) (%f %f %f)\n",abbox.low[0], abbox.low[1],
	 abbox.low[2], abbox.high[0],abbox.high[1],abbox.high[2]);

  initApp(); /* initialize the application */
  initGLsettings();

  atexit(cleanup);

  glutKeyboardFunc(keys);

  //glColor4f(0.7038f, 0.27048f, 0.0828f, 1.0f);
  glColor4f(1.f, 1.f, 1.f, 1.0f);
  glutMainLoop();
  
  return 0;
}
