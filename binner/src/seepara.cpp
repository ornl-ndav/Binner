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
#include "vcbcolor.h"

char projmode = 'O'; /* P for perspective, O for orthogonal */
projStruct proj;
viewStruct view;
lightStruct light0;

ArcBall spaceball;
int iwinWidth;
int iwinHeight;

int     mainwin, sidewin;
extern int     wireframe;

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

/* texture to hold grid info */
#define gridtexsz 32
static float gridtex[gridtexsz*gridtexsz];
static int gtex;
float xmin, xmax, ymin, ymax;

extern int ncuts;

void makeGrid(void)
{
	int i, j, k;

	for (k = 0; k < gridtexsz/2; k ++)
		for (i = k; i < gridtexsz-k; i ++)
			for (j = k; j < gridtexsz-k; j ++)
			{
				gridtex[i*gridtexsz+j] = (gridtexsz/2.f - k)/(gridtexsz/2.f)*0.3;
			}

	xmin = view.center[0]+5*proj.xmin;
	ymin = view.center[1]+5*proj.ymin;
	xmax = view.center[0]+5*proj.xmax;
	ymax = view.center[1]+5*proj.ymax;

}


extern float mradius;
void drawGrid(void)
{
	float divfac = ncuts/mradius;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBegin(GL_QUADS);
	glColor3f(0.774597, 0.774597, 0.774597);
	glTexCoord2f(xmin*divfac, ymin*divfac); //0.f, 0.f);
	glVertex2f(xmin, ymin);
	glTexCoord2f(xmin*divfac, ymax*divfac); //0.f, 1.f);
	glVertex2f(xmin, ymax);
	glTexCoord2f(xmax*divfac, ymax*divfac); //1.f, 1.f);
	glVertex2f(xmax, ymax);
	glTexCoord2f(xmax*divfac, ymin*divfac); //1.f, 0.f);
	glVertex2f(xmax, ymin);
	glEnd();
	glDisable(GL_TEXTURE_2D);

}


void display (void)
{
	char echofps[80];
	int i;

	glClear(GL_COLOR_BUFFER_BIT);

	drawGrid();

	glClear(GL_DEPTH_BUFFER_BIT);

	spaceball.Update();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslated(view.center[0],view.center[1],view.center[2]);
	glMultMatrixf(spaceball.GetMatrix());
	glTranslated(-view.center[0],-view.center[1],-view.center[2]);

	/* if want wireframe mode, then use the following */

	if (wireframe > 0)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	glFrontFace(GL_CCW);
	glDisable(GL_CULL_FACE);

	glCallList(list_id);

	glColor3f(0.f, 0.f, 1.f);
    vcbDrawAxis(spaceball.GetMatrix(), 100);

	glPopMatrix();

 	sprintf(echofps,"fps: %4.2f %6.2f mil tri/sec: division factor: %d",fps, fps*npara*12/1e6, ncuts);
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

float           tlut[256*4];

int main(int argc, char ** argv)
{
  vcbdatatype dtype;
  int   i, j, n, ndims, orig, nvals, dummy;
  int   sliceid;
  float hitcnt, hiterr, corners[8][4];
  double red;

  vcbCustomizeColorTableColdHot(tlut, 0, 255);

  for (i = 0; i < 256; i ++)
	tlut[i * 4+3] = 1.f;

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

	correctCornersf3d(corners, NULL);
	realCubef(corners, vdata);

    for (i = 0; i < 6*4; i ++) {
		abbox.low [0] = VCB_MINVAL(abbox.low [0],vdata[i*4]);
		abbox.high[0] = VCB_MAXVAL(abbox.high[0],vdata[i*4]);
		abbox.low [1] = VCB_MINVAL(abbox.low [1],vdata[i*4+1]);
		abbox.high[1] = VCB_MAXVAL(abbox.high[1],vdata[i*4+1]);
		abbox.low [2] = VCB_MINVAL(abbox.low [2],vdata[i*4+2]);
		abbox.high[2] = VCB_MAXVAL(abbox.high[2],vdata[i*4+2]);
    }

	red = (16+log10(hitcnt+1e-16));
	if (red < 0) red = 0;
	if (red > 16) red = 16;
	glColor3fv(&tlut[((int)(red+0.5))*16*4]);

	for (i = 0; i < 6; i ++)
	  for (j = 0; j < 4; j ++)
		glVertex3fv(&vdata[(i*4+j)*4]);

  }
  glEnd();
  glEndList();

  printf("number of parallelipeds = %d\n",npara);
  printf("bounding box: (%f %f %f) (%f %f %f)\n",abbox.low[0], abbox.low[1],
	 abbox.low[2], abbox.high[0],abbox.high[1],abbox.high[2]);

  wireframe = 1;
  initApp(); /* initialize the application */
  initGLsettings();

  makeGrid();
  gtex = vcbBindOGLTexture2D(GL_TEXTURE0, GL_NEAREST, GL_LUMINANCE, GL_LUMINANCE, GL_FLOAT, 
							gridtexsz, gridtexsz, gridtex, GL_MODULATE, NULL);

  atexit(cleanup);

  //glutKeyboardFunc(keys);
  
  //glColor4f(0.7038f, 0.27048f, 0.0828f, 1.0f);
  glColor4f(1.f, 1.f, 1.f, 1.0f);
  glutMainLoop();
  
  return 0;
}
