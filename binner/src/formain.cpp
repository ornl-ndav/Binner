
/* for use with application main to reduce manual work */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "glew.h"
#include <glut.h>

#include "vcbutils.h"
#include "ArcBall.h"
#include "vcbglutils.h"
#include "vcbmath.h"
#include "vcbimage.h"
#include "atypes.h"

/* assumed external functions that will be defined with main() in a different file */
extern void display(void);

/* assumed globals */
extern char projmode; /* P for perspective, O for orthogonal */
extern projStruct proj;
extern viewStruct view;
extern lightStruct light0;
extern ArcBall spaceball;
extern int iwinWidth;
extern int iwinHeight;
extern bbox abbox;

void initApp (void)
{
	float mradius = 0.f;

	spaceball.SetWidth(iwinWidth);
	spaceball.SetHeight(iwinHeight);

	view.center[0] = (abbox.low[0] + abbox.high[0])/2.;
	view.center[1] = (abbox.low[1] + abbox.high[1])/2.;
	view.center[2] = (abbox.low[2] + abbox.high[2])/2.;

	if ((abbox.high[0]-abbox.low[0]) > mradius) mradius = abbox.high[0]-abbox.low[0];
	if ((abbox.high[1]-abbox.low[1]) > mradius) mradius = abbox.high[1]-abbox.low[1];
	if ((abbox.high[2]-abbox.low[2]) > mradius) mradius = abbox.high[2]-abbox.low[2];

	printf("view.center = (%f %f %f)\n",view.center[0],view.center[1],view.center[2]);

	view.eye[0] = view.center[0];
	view.eye[1] = view.center[1];
	view.eye[2] = view.center[2];
	view.eye[2] += mradius*10.f;

	view.up[0] = 0.;
	view.up[1] = 1.;
	view.up[2] = 0.;

	proj.aspect = 1.0;
	proj.fov = 10.;
	proj.hither = mradius*5.f;
	proj.yon = mradius*15.f;
	proj.xmin = mradius*(-1.);
	proj.xmax = mradius*1.;
	proj.ymin = mradius*(-1.);
	proj.ymax = mradius*1.;

	light0.lightpos[0] = view.eye[0]; /* using headlight */
	light0.lightpos[1] = view.eye[1];
	light0.lightpos[2] = view.eye[2];
}

void initGLsettings(void)
{
	glClearColor(0.,0.,0.,1.);
	
	glViewport(0,0,iwinWidth,iwinHeight);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glLoadIdentity();
	if (projmode == 'P')
		gluPerspective(proj.fov,
					   proj.aspect,
					   proj.hither,
					   proj.yon);
	else
		glOrtho(proj.xmin,proj.xmax,
				proj.ymin,proj.ymax,
				proj.hither,proj.yon);

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glLoadIdentity();
	gluLookAt(view.eye[0],view.eye[1],view.eye[2],
			  view.center[0],view.center[1],view.center[2],
			  view.up[0],view.up[1],view.up[2]);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(1);
	glClearDepth(1.);

}

void reshape(int width,int height)
{
	if (width <= 0 || height <= 0)
		return;

	iwinWidth  = width;
	iwinHeight = height;
	spaceball.SetWidth(width);
	spaceball.SetHeight(height);

	double aspect_ratio = width*1.0/height;
	proj.aspect = aspect_ratio;

	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	if (projmode == 'P') {
		gluPerspective(proj.fov,
					   proj.aspect,
					   proj.hither,
					   proj.yon);
	}
	else {
		double x_center, y_center, x_rad, y_rad;
		x_center = (proj.xmin + proj.xmax)/2.;
		y_center = (proj.ymin + proj.ymax)/2.;
		x_rad    = (proj.xmax - proj.xmin)/2.;
		y_rad    = (proj.ymax - proj.ymin)/2.;

		if (width < height) 
			y_rad = x_rad/proj.aspect;
		else
			x_rad = y_rad * proj.aspect;

		proj.xmin = x_center - x_rad;
		proj.xmax = x_center + x_rad;
		proj.ymin = y_center - y_rad;
		proj.ymax = y_center + y_rad;

		glOrtho(proj.xmin,proj.xmax,
				proj.ymin,proj.ymax,
				proj.hither,proj.yon);

	}

	display();
}

void keys(unsigned char c, int x, int y)
{
	float fzin = 0.95;
	float fzout = 1.05;
	unsigned char * pixels, *bmpbuf;
	int i;

	switch(c) {
      case 'i':
		  proj.xmin *= fzin;
		  proj.xmax *= fzin;
		  proj.ymin *= fzin;
		  proj.ymax *= fzin;
		  proj.fov  *= fzin;
		  reshape(iwinWidth,iwinHeight);
		  break;
	  case 'o':
		  proj.xmin *= fzout;
		  proj.xmax *= fzout;
		  proj.ymin *= fzout;
		  proj.ymax *= fzout;
		  proj.fov  *= fzout;
		  reshape(iwinWidth,iwinHeight);
		  break;
	  case 's': /* save framebuffer */
		  pixels = (unsigned char *) malloc(iwinWidth*iwinHeight*4*sizeof(unsigned char));
		  bmpbuf = (unsigned char *) malloc(iwinWidth*iwinHeight*3*sizeof(unsigned char));
		  glReadPixels(0,0,iwinWidth,iwinHeight,GL_RGBA,GL_UNSIGNED_BYTE,pixels);
		  for (i = 0; i < iwinWidth*iwinHeight; i ++)
		  {
			bmpbuf[i*3+0] = pixels[i*4+0];
			bmpbuf[i*3+1] = pixels[i*4+1];
			bmpbuf[i*3+2] = pixels[i*4+2];
		  }
		  vcbImgWriteBMP("screencap.bmp",bmpbuf,3, iwinWidth, iwinHeight);
		  free(bmpbuf);
		  free(pixels);
		  break;
	  case 'q':
		  exit(0);
		  break;
	  default:
		  break;
  }
}

void mouse_handler(int button, int bstate, int x, int y)
{   

	if (button == GLUT_LEFT_BUTTON) {
		if (bstate == GLUT_DOWN) {
		  spaceball.StartMotion( x, y, glutGet(GLUT_ELAPSED_TIME));
		} else if (bstate == GLUT_UP) {
			spaceball.StopMotion( x, y, glutGet(GLUT_ELAPSED_TIME));
		}
	}
}
   
void trackMotion(int x, int y) 
{

    if (spaceball.Track())
      spaceball.TrackMotion( x, y, glutGet(GLUT_ELAPSED_TIME)); 
}

int initGLUT(int argc, char **argv)
{
	int winid; 

	iwinWidth = 512;
	iwinHeight = 512;

	glutInit( &argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_DOUBLE|GLUT_DEPTH);

    glutInitWindowSize(iwinWidth,iwinHeight);
	glutInitWindowPosition(50,50);
	winid = glutCreateWindow(argv[0]);
	glutSetWindowTitle("SEECON - see convex cells");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutIdleFunc(display);
    glutMouseFunc(mouse_handler);
	glutMotionFunc(trackMotion);
	glutKeyboardFunc(keys);
    //glutSpecialFunc(SpecialKeys);

	return winid;
}
