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

double * dvol;
unsigned short * coord;
int             sz[3], orig[3];
int             ndims,nattribs;
vcbdatatype     rawdatatype;

int load_vox(char * dataname)
{
/*
	if ((binvol = (double*) vcbReadBinm(dataname, &rawdatatype, &ndims, orig, sz, &nattribs)) 
	    == NULL) {
		fprintf(stderr, "load_data: error loading datafile\n");
		return -1;
	}
	return 0;
*/
	FILE * hdr;
	char fullname[256], data[256];
	double ori[3], spacing[3];
	int i, j, k, n;
	int dims[3], dsize[3];

	sprintf(fullname, "%s/run.hdr", dataname);	
	fprintf(stderr,"trying %s\n", fullname);
	
	hdr = fopen(fullname, "r");
	
	fscanf(hdr, "REBINNED Qxyz HISTOGRAM\n%s\n", data);
	fscanf(hdr, "DIMENSIONS %d %d %d\n", &sz[0], &sz[1], &sz[2]);
	fscanf(hdr, "ORIGIN %lf %lf %lf\n", &ori[0], &ori[1], &ori[2]);
	fscanf(hdr, "SPACING %lf %lf %lf\n", &spacing[0], &spacing[1], &spacing[2]);	
	fclose(hdr);

	fprintf(stderr,"vol size %d %d %d\n", sz[0],sz[1],sz[2]);
	fprintf(stderr,"vol orig %lf %lf %lf\n", ori[0],ori[1],ori[2]);

	sprintf(fullname, "%s/combined.bin.d", dataname);
	dvol = (double*)vcbReadBinm(fullname, &rawdatatype, dims, orig, dsize, &nattribs);
	if (dvol == NULL)
		return -1;

	sprintf(fullname, "%s/combined.bin.us", dataname);
	coord = (unsigned short *)vcbReadBinm(fullname, &rawdatatype, dims, orig, dsize, &nattribs);
	if (coord == NULL)
		return -1;
	
	fprintf(stderr,"done reading %s, %d voxels\n", fullname, dsize[0]);
	return dsize[0];
}

void load_classify(void)
/*
 * -1 for failure, 0 for success 
 * transflut is assumed to have valid memory allocation
 */
{
	int i;
	float a;
	 
	vcbCustomizeColorTableColdHot(tlut, 0, 255);

	for (i = 0; i < 256; i ++)
	{
		a = (float)i/255;
		tlut[i * 4+3] = i/255.;//1.f;// a*a;
	}

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
	int i, j, k, numvox, t;
	double val, range;
	double minval, maxval, total;
	
	minval = 1e6;
	maxval = -1e6;
	total = 0;
	
	nvox = load_vox(dataname);
	if (nvox < 0) {
		fprintf(stderr, "load_data: error loading datafile\n");
		exit(-1);
	}
	
	load_classify();
	fprintf(stderr, "loaded %d voxels\n", nvox);

	fvox = (unsigned char *) malloc(nvox*10*sizeof(unsigned char));

	for (i = 0; i < nvox; i ++)
	{
		val = dvol[i];
		total += val;

		if (minval > val) minval = val;
		if (maxval < val) maxval = val;
	}

#define logon 0

#if logon
	range = log10(maxval) - log10(minval);
#else
	range = maxval - minval;
#endif

	range = 255/range;

	for (i = 0; i < nvox; i ++)
	{
		val = dvol[i];

#if logon
		val = log10(val + 1e-16) - log10(minval + 1e-16);
#else
		val = val - minval;
#endif
		
		val = val - minval; //log10(val + 1e-16) - log10(minval + 1e-16);
		t = (int)(val*range+0.5);
		if (t < 0) t = 0;
		//t = t * 32;
		if (t > 255) t = 255;

		cvox = (unsigned short*)&fvox[i*10+4];
		fvox[i*10+0] = (unsigned char)t; //voxels[i*4+0];	
		fvox[i*10+1] = 0;  //voxels[i*4+1];
		fvox[i*10+2] = 0;  //voxels[i*4+2];
		fvox[i*10+3] = (unsigned char)((char)(127)); //voxels[i*4+3];
		cvox[0] = coord[i*3+0];	
		cvox[1] = coord[i*3+1];
		cvox[2] = coord[i*3+2];
	}	

	printf("domain sz: %d %d %d\n", sz[0], sz[1], sz[2]);
	printf("total sum: %e; range: [%e %e]\n",total, minval, maxval);

	free(dvol);
	free(coord);
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

	printf("%d non-empty voxels\nbounding box: (%f %f %f) (%f %f %f)\n", nvox, abbox.low[0], abbox.low[1],
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
