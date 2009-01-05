#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <time.h>

#include "glew.h"
#include <glut.h>

#include "vcbutils.h"
#include "ArcBall.h"
#include "vcbglutils.h"
#include "vcbmath.h"
#include "atypes.h"

#include "vcbvolren.h"
#include "vcblinalg.h"
#include "vcbcolor.h"

char projmode = 'O'; /* P for perspective, O for orthogonal */
projStruct proj;
viewStruct view;
lightStruct light0;

ArcBall spaceball;
int iwinWidth;
int iwinHeight;

GLuint  texobj;
int     mainwin, sidewin;

clock_t time1, time2, time3, time4;

bbox abbox;

unsigned char * voxels;
int             nvox;
float           tlut[256*4];

#define FTB 1

double			*binvol;
int             sz[3], orig[3];
int             ndims,nattribs;
vcbdatatype     rawdatatype;

int load_vox(char * dataname)
{
	if ((binvol = (double*) vcbReadBinm(dataname, &rawdatatype, &ndims, orig, sz, &nattribs)) 
	    == NULL) {
		fprintf(stderr, "load_data: error loading datafile\n");
		return -1;
	}
	return 0;
}

void load_classify(void)
/*
 * -1 for failure, 0 for success 
 * transflut is assumed to have valid memory allocation
 */
{
	int i;
	 
	vcbCustomizeColorTableColdHot(tlut, 0, 255);

	for (i = 0; i < 256; i ++)
		tlut[i * 4+3] = 1.f;

}

void voxtransf (float * rgba, void * vox)
{
	unsigned char * voxel = (unsigned char *) vox;
	int id = voxel[0] * 4;
	rgba[0] = tlut[id+0];
	rgba[1] = tlut[id+1];
	rgba[2] = tlut[id+2];
	rgba[3] = tlut[id+3];
}

void load_data(char * dataname)
{
	unsigned char * fvox;
	unsigned short * cvox;
	int i, j, k, numvox;
	double val;
	
	if (load_vox(dataname) < 0) {
		fprintf(stderr, "load_data: error loading datafile\n");
		exit(-1);
	}
	
	load_classify();

	nvox = 0;
	for (i = 0; i < sz[0]; i ++)
		for (j = 0; j <sz[1]; j ++)
			for (k = 0; k <sz[2]; k ++) {

				if (binvol[vcbid3(i,j,k,sz,0,1)] > 1e-16)
				    nvox ++;
			}

	fvox = (unsigned char *) malloc(nvox*10*sizeof(unsigned char));

	nvox = 0;
	for (i = 0; i < sz[0]; i ++)
		for (j = 0; j <sz[1]; j ++)
			for (k = 0; k <sz[2]; k ++) {

				val = log10(binvol[vcbid3(i,j,k,sz,0,1)]);
				val += 16;
				val *= 16;
				if (val < 0) continue;
				
				cvox = (unsigned short*)&fvox[nvox*10+4];
				fvox[nvox*10+0] = val; //voxels[i*4+0];	
				fvox[nvox*10+1] = 0;  //voxels[i*4+1];
				fvox[nvox*10+2] = 0;  //voxels[i*4+2];
				fvox[nvox*10+3] = (unsigned char)((char)(127)); //voxels[i*4+3];
				cvox[0] = (unsigned short)i;	
				cvox[1] = (unsigned short)j;
				cvox[2] = (unsigned short)k;

				nvox ++;
			}	

	free(binvol);
	voxels = fvox;

	abbox.low[0] = abbox.low[1] = abbox.low[2] = 0.f;
	abbox.high[0] = sz[0];
	abbox.high[1] = sz[1];
	abbox.high[2] = sz[2];
}

void display (void)
{
	char echofps[80];
	float * m_spaceball;

	float x, y, xs, ys;
	int i;
	
	time1 = clock();

	glDisable(GL_LIGHTING);
	glColor4f(1.f, 1.f, 1.f, 1.f);

	spaceball.Update();

	m_spaceball = spaceball.GetMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslated(view.center[0],view.center[1],view.center[2]);
	glMultMatrixf(spaceball.GetMatrix());
	glTranslated(-view.center[0],-view.center[1],-view.center[2]);

	glClear(GL_COLOR_BUFFER_BIT);

	vcbVolrSsplats(voxels, nvox, sz);//, m_spaceball);

	glPopMatrix();

	time2 = clock();
	sprintf(echofps,"fps: %6.2f",(float)CLOCKS_PER_SEC/(time2-time1));

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_1D);
	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0., 1., 0., 1., -1., 1.);
	/* end set up */

	x  = 0.9;//(proj.xmax - proj.xmin)*0.95f + proj.xmin;
	xs = 0.05;//(proj.xmax - proj.xmin)*0.05f;
	y  = 0.05;//proj.ymin + (proj.ymax - proj.ymin)*0.1;
	ys = 0.9/256;//(proj.ymax - proj.ymin)*0.9/256;
	glBegin(GL_QUADS);							
	for (i = 0; i < 256; i ++)
	{
		glColor4fv(&tlut[i*4]);
		glVertex3f(x,    y+i*ys,     0.0f); // Bottom Left
		glVertex3f(x+xs, y+i*ys,     0.0f); // Bottom Right
		glVertex3f(x+xs, y+(i+1)*ys, 0.0f); // Top Right
		glVertex3f(x,    y+(i+1)*ys, 0.0f); // Top Left
	}
	glEnd();

	/* restore opengl state machine */
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	//glEnable(GL_LIGHTING);
    vcbDrawAxis(spaceball.GetMatrix(), 100);
	vcbOGLprint(0.01f,0.01f, 1.f, 1.f, 0.f, echofps);

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_1D);

	glutSwapBuffers();
}



void cleanup (void)
{
	vcbVolrCloseS();
	printf("finished cleaning up ...\n");
	fflush(stdout);
}

int main(int argc, char ** argv)
{
	float oglverno;
	int   ntexunits;

	load_data(argv[1]);

	printf("%d voxels in a bounding box of: (%f %f %f) (%f %f %f)\n", nvox, abbox.low[0], abbox.low[1],
		abbox.low[2], abbox.high[0],abbox.high[1],abbox.high[2]);

	initApp(); /* initialize the application */

	mainwin = initGLUT(argc,argv);

    GLenum err = glewInit();
    if (GLEW_OK != err) {
		fprintf(stderr,"glew error, initialization failed\n");
        fprintf(stderr,"Glew Error: %s\n", glewGetErrorString(err)); 
    } 

	initGLsettings();
	vcbEnableOGLlight(0.1f); 
	vcbSetOGLlight(GL_LIGHT0, 0.1f, 1.0f, light0.lightpos[0], light0.lightpos[1], light0.lightpos[2], 40.f);

	atexit(cleanup);
	oglverno = vcbGetOGLVersion();
	printf("using ogl version %f",oglverno);
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, & ntexunits);
	printf(" with %d texture units\n", ntexunits);

	vcbVolrInitS(voxtransf);


	glutMainLoop();

	return 0;
}
