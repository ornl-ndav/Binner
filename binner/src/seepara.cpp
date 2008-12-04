#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#include "glew.h"
#include <glut.h>

#include "vcbutils.h"
#include "ArcBall.h"
#include "vcbmath.h"
#include "vcbglutils.h"
#include "vcblinalg.h"
#include "vcbmcube.h"
#include "vcbimage.h"
#include "cell.h"
#include "binnerio.h"

#include "atypes.h"

char projmode = 'O'; /* P for perspective, O for orthogonal */
projStruct proj;
viewStruct view;
lightStruct light0;

ArcBall spaceball;
int iwinWidth;
int iwinHeight;

int     mainwin, sidewin;

/* application specific data starts here */
int     nverts, npara;
float   vdata[1024];

/* end of application specific data */
bbox abbox;

char inputbuf[1024];

int list_id;

/* nframes and fps are used for calculating frame rates in formain.cpp:idle() */
extern int nframes;
extern float fps;

void display (void)
{
	char echofps[80];
	int i;

	spaceball.Update();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslated(view.center[0],view.center[1],view.center[2]);
	glMultMatrixf(spaceball.GetMatrix());
	glTranslated(-view.center[0],-view.center[1],-view.center[2]);

	/* if want wireframe mode, then use the following */
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glPolygonMode(GL_FRONT, GL_LINE);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);

	glClear(GL_COLOR_BUFFER_BIT);
	glClear(GL_DEPTH_BUFFER_BIT);

	glCallList(list_id);

    vcbDrawAxis(spaceball.GetMatrix(), 100);

	glPopMatrix();

 	sprintf(echofps,"fps: %4.2f %6.2f mil tri/sec",fps, fps*npara*12/1e6);
/*
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_1D);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
*/
	vcbOGLprint(0.01f,0.01f, 1.f, 1.f, 0.f, echofps);
/*
	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_1D);
*/
	nframes ++;
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
  int   i, j, n, ndims, orig, nvals, dummy;
  int   sliceid;
  float hitcnt, hiterr, corners[8][4];
  double red;

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
  glBegin(GL_QUADS);

  for (npara = 0; (n = get_pixelf(&sliceid,&hitcnt,&hiterr,corners)) > 0; npara ++) {
    /*
	for (i = 0; i < 8; i ++)
		printf("%f, %f, %f, %f\n", corners[i][0],
		                          corners[i][1],
								  corners[i][2],
								  corners[i][3]);
	*/

	correctCornersf3d(corners);
	realCubef(corners, vdata);

    for (i = 0; i < 6*4; i ++) {
		abbox.low [0] = VCB_MINVAL(abbox.low [0],vdata[i*4]);
		abbox.high[0] = VCB_MAXVAL(abbox.high[0],vdata[i*4]);
		abbox.low [1] = VCB_MINVAL(abbox.low [1],vdata[i*4+1]);
		abbox.high[1] = VCB_MAXVAL(abbox.high[1],vdata[i*4+1]);
		abbox.low [2] = VCB_MINVAL(abbox.low [2],vdata[i*4+2]);
		abbox.high[2] = VCB_MAXVAL(abbox.high[2],vdata[i*4+2]);
    }

	red = 8+log10(hitcnt/1.20+0.00000001);
	glColor3f(0.5f+(float)red/16.f, 0.5f, 0.5f);
	for (i = 0; i < 6; i ++)
	  for (j = 0; j < 4; j ++)
		glVertex3fv(&vdata[(i*4+j)*4]);

  }
  glEnd();
  glEndList();

  printf("number of parallelipeds = %d\n",npara);
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
