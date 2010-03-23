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

#ifndef _VCB_VOLREN_H 
#define _VCB_VOLREN_H

#include "vcbutils.h"

#ifndef VCB_MAXLIGHT
#define VCB_MAXLIGHT 8
#endif
#ifndef VCB_PERSPECTIVE
#define VCB_PERSPECTIVE  0
#endif
#ifndef VCB_ORTHOGONAL
#define VCB_ORTHOGONAL 1
#endif

#define vcbid3(x,y,z, dsize,i,na) ((((x)*dsize[1] + (y)) * dsize[2] + (z))*(na) + (i))

typedef void (* VCB_TRANSFUNC) (float *, void *);

/** Enum
 * Name: vcbVolrenDst
 * Version: 0.92
 * Description: The destination of the volume renderer.
 * Elements:
 *	VCB_OPENGL: Rendered to an OpenGL screen
 *	VCB_MEMORY: Rendered to a user-provided memory framebuffer.
 **/
typedef enum _eFramebuffer {
	VCB_OPENGL,
	VCB_MEMORY,
} vcbVolrenDst;

/** Enum
 * Name: vcbOpticalModel
 * Version: 0.93
 * Description: The optical model to be used in a volume rendering context.
 * Elements:
 *	VCB_AOREONLY:	Absorption or emission only.
 *	VCB_ABSEMI:		Absorption and emission.
 **/
typedef enum _eOptical {
	VCB_AOREONLY,    /* absorption only or emission only */
	VCB_ABSEMI,      /* absorption and emission */
} vcbOpticalModel;

/** Struct
 * Name: VCB_volren
 * Version: 0.92
 * Description: typedefined&#58; * VCB_volren <br>
 *	The primary structure used by vcbvolren for storing various information.
 * Elements:
 *	int   projmode:	The projection mode: Perspective or Orthographic
 *	float fov:		The field of view, in degrees. Applies only to perspective mode.
 *	float aspect:	The aspect ratio winx/winy. Applies only to perspective mode.
 *	float hither:	The hither (near) clipping plane. Applies only to perspective mode.
 *	float yon:		The yon (far) clipping plane. Applies only to perspective mode.
 *	float xmin:		The minimum value on the x axis. Applies only to orthographic mode.
 *	float xmax:		The maximum value on the x axis. Applies only to orthographic mode.
 *	float ymin:		The minimum value on the y axis. Applies only to orthographic mode.
 *	float ymax:		The maximum value on the y axis. Applies only to orthographic mode.
 *	float center[3]:	The center of interest (x,y,z).
 *	float eye[3]:		The eye (camera) position (x,y,z).
 *	float up[3]:		The up vector.
 *	int   imgw:		The viewport/image width.
 *	int   imgh:		The viewport/image height.
 *	int   nlights:	The number of lights in use.
 *	float light[VCB_MAXLIGHT*3]:	The light positions. All lights are directional.
 *	float half[VCB_MAXLIGHT*3]:		The half vector ((view + light)/2) for each light.
 *	float lvec[VCB_MAXLIGHT*3]:		View dependent light vectors, directional lights.
 * 	float ambient[VCB_MAXLIGHT]:	The ambient light intensity for each light.
 *	float diff[VCB_MAXLIGHT]:		The diffuse light intensity for each light.
 *	float spec[VCB_MAXLIGHT]:		The specular light intensity for each light.
 *	VCB_TRANSFUNC transf:		The trasnfer function in use.
 *	vcbVolrenDst dst:			The rendering desintation (OpenGL framebuffer, memory)
 *	vcbdatatype  fbtype:		The data type used in the renderer.
 *	void * fbuffer:				The pointer to memory used when the destination is VCB_MEMORY
 *	vcbOpticalModel opticalmodel:	The optical model to be used in the renderer.
 *	float obj2scr[16]:	The object-space to screen-space transformation matrix.
 *	float scr2obj[16]:	The screen-space to object-space transformation matrix.
 **/
typedef struct _volren_struct {
	/* projection */
	int   projmode; /* VCB_PERSPECTIVE OR ORTHOGRAPHICS */

	/* 1: these are for perspective */
	float fov;    /* field of view, in degrees */
	float aspect; /* aspect ratio winx/winy */
	float hither;
	float yon;
	/* 2: these are for orthogonal */
	float xmin, xmax;
	float ymin, ymax;
	
	/* viewing */
	float center[3];
	float eye[3];
	float up[3];

	/* viewport, image width, height */
	int   imgw, imgh;

	/* light source */
	int   nlights;
	float light[VCB_MAXLIGHT*3]; /* light positions. all lights are directional */
	float half[VCB_MAXLIGHT*3];  /* half vector (view + light)/2 */
	float lvec[VCB_MAXLIGHT*3];  /* view dependent light vectors, directional lights */
	float ambient[VCB_MAXLIGHT];
	float diff[VCB_MAXLIGHT];
	float spec[VCB_MAXLIGHT];

	/* transfer function */
	VCB_TRANSFUNC transf;

	/* destination */
	vcbVolrenDst dst;
	vcbdatatype  fbtype;
	void * fbuffer;

	/* optical model */
	vcbOpticalModel opticalmodel;

	/* transformation matrices */
	float obj2scr[16];
	float scr2obj[16];

} * VCB_volren;

#ifdef __cplusplus
extern "C" {
#endif

/* beginning of API */

/* current raycasting API */

/* initialization and clear up*/

/** Function
 * Name: vcbNewVolRenderer
 * Version: 0.92
 * Description:
 *  Initializes a volume rendering context.
 *  The only parameter required here is the optical model
 *  to use, specifying the mathematics setup of the whole framework.
 * <p>
 *  Possible choices are&#58;<br>
 *    absorption only or emission only: VCB_AOREONLY <br>
 *    absorption and emission:          VCB_ABSEMI
 * Arguments:
 *	vcbOpticalModel opticalmodel: The optical model to be used in this context.
 * Return: VCB_volren; Will be NULL for failure, or a valid pointer to
 *    the volume rendering context on success.
 * Known Issues: None
 **/
VCB_volren vcbNewVolRenderer(vcbOpticalModel opticalmodel);

/** Function
 * Name: vcbFreeVolRenderer
 * Version: 0.92
 * Description:
 *	The function to remove a vcb volume rendering context pointed to
 *	by volr from memory.
 * Arguments:
 *	VCB_volren volr: The rendering context to be freed.
 * Return: None
 * Known Issues: None
 **/
void      vcbFreeVolRenderer(VCB_volren volr);

/* set up calls */

/** Function
 * Name: vcbVolrPerspective
 * Version: 0.92
 * Description:
 *  This function is used to set up a perspective projection. The inputs are the same as glPerspective.
 * Arguments:
 *    VCB_volren volr:	Pointer to the vcb volume rendering context in use
 *    float fov:		Field of view in Y direction
 *    float aspect:		Aspect ratio (Y/X)
 *    float hither:		Near clipping z
 *    float yon:		Far  clipping z
 * Return: None
 * Known Issues: None
 **/
void vcbVolrPerspective(VCB_volren volr, float fov, float aspect, float hither, float yon);

/** Function
 * Name: vcbVolrOrtho
 * Version: 0.92
 * Description:
 *  This function is used to set up an orthographic projection. The inputs are the same as glOrtho3.
 * Arguments:
 *    VCB_volren volr:   Pointer to the vcb volume rendering context in use
 *    float xmin:   Left boundary
 *    float xmax:   Right boundary
 *    float ymin:   Bottom boundary
 *    float ymax:   Top boundary
 *    float hither: Near clipping z
 *    float yon:    Far  clipping z
 * Return: None
 * Known Issues: None
 **/
void vcbVolrOrtho      (VCB_volren volr, float hither, float yon, float xmin, float xmax, float ymin, float ymax);

/** Function
 * Name: vcbVolrLookAt
 * Version: 0.92
 * Description: 
 *  This function is used to set up a camera system. The inputs are the same as gluLookAt.
 *  <p>
 *  Note&#58; The up direction given should not be parallel to the direction
 *       pointing from coi to eye. The results is undefined for this situation.
 * Arguments:
 *    VCB_volren volr:  Pointer to the vcb volume rendering context in use
 *    float centerx:	X position of center of interest
 *    float centery:	Y position of center of interest
 *    float centerz:	Z position of center of interest
 *    float eyex:		X position of eye/viewer/camera
 *    float eyey:		Y position of eye/viewer/camera
 *    float eyez:		Z position of eye/viewer/camera
 *    float upx:		The X component of the up vector (upward direction of the camera)
 *    float upy:		The Y component of the up vector (upward direction of the camera)
 *    float upz:		The Z component of the up vector (upward direction of the camera)
 * Return: None
 * Known Issues: None
 **/
void vcbVolrLookAt     (VCB_volren volr, float centerx, float centery, float centerz, float eyex, float eyey, float eyez, float upx, float upy, float upz);

/** Function
 * Name: vcbVolrLight
 * Version: 0.92
 * Description:
 *  IMPORTANT&#58; This API is now obsolete. One should always use
 *   vcbVolrSetLight instead.
 *  <p>
 *  This function is used to set up a new light source. <br>
 *  All vcb volume renderer light sources are directional
 *   light sources pointing towards the coi.
 *  <p>
 *  Note&#58; Currently, all light sources are defaulted to have
 *       0.f ambient intensity, 1.0 diffuse intensity and
 *       1.0 specular intensity. All lights sources are assumed to be
 *       of white color, and have an exponential coefficient of 40.
 *       Extending the vcbVolrLight API to allow specifying these
 *       parameters is not difficult, but not yet top priority.
 * Arguments:
 *  VCB_volren volr:	Pointer to the vcb volume rendering context in use
 *  float lightx:		X position of the light source
 *  float lighty:		Y position of the light source
 *  float lightz:		Z position of the light source
 * Return: int; The total number of light sources in volr context.
 * Known Issues: None
 **/
int  vcbVolrLight      (VCB_volren volr, float lightx, float lighty, float lightz);

/** Function
 * Name: vcbVolrSetLight
 * Version: 0.92
 * Description:
 *  This function is used to set up a particular light source. <br>
 *  All vcb volume renderer light sources are directional light
 *  sources pointing towards the coi.
 *  <p>
 *  Note&#58; Currently all light sources are defaulted to have
 *       0.f ambient intensity, 1.0 diffuse intensity and
 *       1.0 specular intensity. All lights sources are assumed to be
 *       of white color, a exponential coefficient of 40.
 *       Extending the vcbVolrLight API to allow specifying these
 *       parameters is not difficult, but not yet top priority.
 *  <p>
 *  Very Important&#58; Current vcbvolren libraries all assume only one light source. In other
 *       words, although one can set up as many as 8 light sources, only light source 0
 *       gets used for illumination computation.
 * Arguments:
 *  VCB_volren volr:	Pointer to the vcb volume rendering context in use
 *  int light_id:		Which light source to modify, valid numbers are 0 to (VCB_MAXLIGHT-1) <br>
 *                		 VCB_MAXLIGHT == 8 in current implementation
 *  float lightx:		X position of the light source
 *  float lighty:		Y position of the light source
 *  float lightz:		Z position of the light source
 * Return: int; -1 if invalid operation, 0 for normal exit
 * Known Issues: None
 **/
int  vcbVolrSetLight   (VCB_volren volr, int light_id, float lightx, float lighty, float lightz);

/** Function
 * Name: vcbVolrViewPort
 * Version: 0.92
 * Description:
 *  This function is used to set up a viewport.
 * Arguments:
 *  VCB_volren volr:	The vcb volume renderer context to use
 *  int w:				The width of the viewport
 *  int h:				The height of the viewport
 * Return: None
 * Known Issues: None
 **/
void vcbVolrViewPort   (VCB_volren volr, int w, int h);

/** Function
 * Name: vcbVolrTransFunc
 * Version: 0.92
 * Description:
 *  This function is used to specify a transfer function to use
 *  in the volr context. This is a unique feature offered in vcb volume renderer.
 *  <p>
 *  Here, the transfer function is literally a function that the user
 *  can specify and pass to vcb volume renderer as a function
 *  pointer.
 *  The only requirement is that a very general type is abided by&#58; <pre>
 *          void (* VCB_TRANSFUNC) (float *, void *); </pre>
 *  An example transfer function is provided below for illustration&#58; <pre>
 *    void voxtransf (float * rgba, void * vox)
 *    {
 *       unsigned char * voxel = (unsigned char *) vox;
 *       int id = voxel[0] * 4;
 *       rgba[0] = tlut[id+0];
 *       rgba[1] = tlut[id+1];
 *       rgba[2] = tlut[id+2];
 *       rgba[3] = tlut[id+3];
 *    }
 *  </pre>
 *  The output of transf is always the first pointer, rgba, pointing
 *  to 4 floating points, corresponding to R, G, B and ALPHA channels.
 *  <p>
 *  The input can vary. On each voxel in a volume, there can be multiple
 *  variables. vcb volume renderer reconstructs all those variables on
 *  every volume sample and passes to transf via the pointer, vox.
 *  <p>
 *  Inside transf, the user is free to interpret all these variables
 *  in whatever way that is the most meaningful. Here, in this example,
 *  all we are doing is a simple table lookup. tlut is a global variable
 *  in the user's main.c/cpp.
 *  <p>
 *  According to our measurement, the overhead of calling an actual
 *  function to obtain transfer function classification is below a
 *  neglible 1.5% of total rendering time.
 * Arguments:
 *  VCB_volren volr:	The vcb volume renderer context to use
 *  VCB_TRANSFUNCE transf: The pointer to the tranfer function.
 * Return: None
 * Known Issues: None
 **/
void vcbVolrTransFunc     (VCB_volren volr, VCB_TRANSFUNC transf);

/** Function
 * Name: vcbVolrFramebuffer
 * Version: 0.92
 * Description:
 *  This is a unique feature of vcb volume renderer. <br>
 *  Here, one can specify a destination of the renderer using the volr context.
 *  <p>
 *  The two choices are specified in dst&#58; <br>
 *    VCB_OPENGL: the result directly get shown on an OpenGL screen <br>
 *    VCB_MEMORY: the result is located a user provided memory framebuffer
 *  <p>
 *  fbtype specifies the type of each channel in the framebuffer. For
 *    instance, VCB_UNSIGNEDBYTE.
 *  <p>
 *  If the dst is VCB_MEMORY, then a 4th input argument is expect, that is
 *    a pointer to the user provided memory framebuffer.
 *  <p>
 *  Usage example&#58; <pre>
 *    unsigned char * framebuffer;
 *    framebuffer = (unsigned char *) malloc (w*h*4*sizeof(unsigned char));
 *    vcbVolrViewPort(volren, w, h);
 *    vcbVolrFramebuffer(volren, VCB_MEMORY, VCB_UNSIGNEDBYTE, framebuffer);
 *  </pre>
 *  Note&#58; Currently, only VCB_MEMORY destination is correctly implemented.
 *    Native handling of VCB_OPENGL destination assumes a primitive semantic,
 *    that is to render to an intern static memory segment which is not
 *    accessible to user program. To be fixed soon.
 * Arguments:
 *  VCB_volren volr:	The vcb volume renderer context to use
 *	vcbVolrenDst dst:	The renderer destination.
 *	vcbdatatype fbtype:	The data type to be used in the renderer.
 * Return: None
 * Known Issues: None
 **/
void vcbVolrFramebuffer(VCB_volren volr, vcbVolrenDst dst, vcbdatatype  fbtype, ...);

/* volume rendering calls */

/** Directions for vcbvolren_raycasting
 * <h2>Description:</h2>
 *	The VCB ray-casting API is very simple, and initialized with one call which
 *  takes a value from vcbOpticalModel as its only argument. The frame buffer setup 
 *  takes a value from vcbVolrenDst to dictate the output destination of the renderer.
 *  Other functions in the vcbvolren.h library have a very similar API to OpenGL. 
 *
 * <h2>Usage example:</h2><pre>
 *   VCB_volren volren;
 *   unsigned char * framebuffer;
 *   int imgfootprint[4];
 *   int origin = {32, 32, 32};
 *   int w = h = 512;
 *
 *   &#47;* set up *\/
 *   framebuffer = (unsigned char *) malloc (w*h*4*sizeof(unsigned char));
 *   volren = vcbNewVolRenderer(VCB_ABSEMI);
 *   vcbVolrPerspective(volren, 30.f, 1.f, .01f, 10000.f);
 *   vcbVolrLookAt(volren, 32.f, 32.f, 32.f, 160.f, 32.f, 160.f, 1.f, 0.f, 0.f);
 *   vcbVolrLight(volren, 160.f, 32.f, 160.f); &#47;*light position*\/
 *   vcbVolrViewPort(volren, w, h);
 *   vcbVolrTransFunc(volren, voxtransf);  &#47;* specify the transfer function*\/
 *   vcbVolrFramebuffer(volren, VCB_MEMORY, VCB_UNSIGNEDBYTE, framebuffer);
 *
 *   &#47;* volume render*\/
 *   vcbVolrRC(volren, VCB_UNSIGNEDBYTE, voxels, 4, origin, sz, scales, 0.5f, imgfootprint);
 *
 *   &#47;* done rendering, save image to disk*\/
 *   vcbImgWriteBMP("rcast.bmp",framebuffer, 4, w, h);
 *
 *   &#47;* clear up *\/
 *   vcbVolrCloseRC(volren);
 *   vcbFreeVolRenderer(volren);
 *   free(framebuffer);
 *	 </pre>
 **/

/** Function
 * Name: vcbVolrRC
 * Version: 0.92
 * Description:
 *  This is the ray-casting volume renderer.
 *  <p>
 *	If the destination mode is VCB_MEMORY, an output image is gererated and
 *       stored in the memory space given in vcbVolrFramebuffer.
 *  Else, an image is still rendered but stored in an internal static
 *       memory allocation not visible outside the volume renderer API.
 *       In this case, the internal static memory allocation is released
 *       when vcbVolrCloseRC is called.
 * Arguments:
 *  VCB_volren volr:		The vcb volume renderer context to use
 *  vcbdatatype vcbtype:	Data type of the volume
 *  void* voxels:			Pointer to the 3D volume, coordinate (i,j,k) is at location&#58;<br>
 *                           ((i * dsize[1] + j) * dsize[2] + k) * nval
 *	int nval:				Number of variables on each voxel
 *	int* origin:			Origin of the volume block given
 *  int* dsizes:			dsizes[0] through dsizes[3] describes number of voxels in the
 *							 3 dimensions of the volume block
 *  float* scales:			Relative scale of the volume, for instance 1&#58;1&#58;3 for a non-uniform
 *							 grid with a bigger spacing in Z direction
 *	float stepsz:			The step size of the ray.
 *	int* imgfootprint:		This is a pointer to 4 integer array. If imgfootprint is NULL,
 *            then no output via this pointer is given. But, if it's not NULL and
 *            (assuming it has a space for at least 4 integers), the actual
 *            bounds of the screen projection of the bounding box of this volume
 *            being rendered is given on the image plane.
 * Return: None
 * Known Issues: None
 **/
void vcbVolrRC(VCB_volren volr, vcbdatatype vcbtype, void * voxels, int nval, int * origin, int * dsizes, float * scales,   float stepsz, int * imgfootprint);

/** Function
 * Name: vcbVolrCloseRC
 * Version: 0.92
 * Description:
 *	After done using vcb volume renderer, you must call the
 *	corresponding vcbVolrClose function for a clean exit.<br>
 *  There are&#58; vcbVolrCloseRC/S/T for raycasting, splatting,
 *  and 3D texture mapping, respectively.
 * Arguments:
 *  VCB_volren volr:	The vcb volume renderer context to use
 * Return: None
 * Known Issues: None
 **/
void vcbVolrCloseRC(VCB_volren volr);

/* End Current Raycasting API */

/* current splatting API */

/** Directions for vcbvolren_splatting
 * <h2>Description:</h2>
 * VCB splatting uses OpenGL hardware for acceleration. It uses
 * two texture units, GL_TEXTURE0 and GL_TEXTURE1, and vertex
 * array extensions (both are already openGL standard in currently
 * prevalent OpenGL release 1.4).
 * <p>
 * To use vcb splatting, three API function calls are necessary.
 * The user program does not have to worry about setting up these
 * modules: <br>
 *	vcbVolrInitS <br>
 *	vcbVolrSsplats <br>
 *	vcbVolrCloseS <br>
 *
 * <h2>A note about front-to-back (FTB) compositing:</h2>
 * If the user defines: <pre>
 * #ifndef FTB
#define FTB 1 </pre>
#endif
 * then front-to-back compositing will be used by the splatting renderer. <br>
 * Otherwise, the back-to-front method will be used.
 * <p>
 * Because OpenGL only allows one multiplication on each fragment, FTB volume rendering
 * needs:<pre>
 *	dst = src * src_alpha * (1 - dst_alpha)</pre>
 * 
 * Pre-multiplying src_alpha to RGB channels in the transfer function look-up
 * table does not work in splatting, because the footprint takes effect via alpha
 * channel being multiplied as src alpha.
 *
 * <h2>Example code:</h2><pre>
 *
 * #include "glew.h"
 * #include &lt;GL/glut.h&gt;
 * 
 * #include "ArcBall.h"
 * #include "vcbvolren.h"
 * 
 * viewStruct view;
 * ArcBall spaceball;
 * 
 * unsigned char * voxels;
 * int             sz[3], nvox;
 * 
 * #ifndef FTB
#define FTB 1  &#47;* define use of front-to-back rendering *\/
#endif
 * 
 * int load_vox(char *rootname, int * dsizes, unsigned char **dataVol)
 * &#47;* -1 for failure. 0 for success *\/
 * {
 * 	char volumename[30], infoname[30], str[300], junk[30];  
 * 	float ax,ay,az;
 * 	FILE *fp;
 * 	int nx, ny, nz, voln;
 * 	
 * 	sprintf(volumename,"%s.vox",rootname);
 * 	sprintf(infoname,"%s.txt",rootname);
 * 	
 * 	if((fp=fopen(infoname,"r"))==NULL) {
 * 		fprintf(stderr,"load_vox, cannot open infofile %s\n",infoname);
 * 		return -1;
 * 	}
 *   
 * 	fgets(str,100,fp);
 * 	strtok(str," "); strtok(NULL," ");
 * 	nx = atoi(strtok(NULL,"x"));
 * 	ny = atoi(strtok(NULL,"x"));
 * 	nz = atoi(strtok(NULL," "));
 *   
 * 	printf("loading a volume of size %d x %d x%d\n", nx, ny, nz);
 * 	voln= nx*ny*nz;
 * 
 * 	fgets(str,100,fp);
 * 	sscanf(str,"%s%s%f:%f:%f",junk,junk,&ax,&ay,&az);
 * 	printf("aspect ratio = %f:%f:%f\n",ax, ay, az);
 * 	fclose(fp);
 * 
 * 	if((fp=fopen(volumename,"rb"))==NULL) {
 * 		fprintf(stderr,"load_vox, cannot open volume file %s\n", volumename);
 * 		return -1;
 * 	}
 * 
 * 	(*dataVol) = (unsigned char *) malloc (sizeof(unsigned char) * voln);
 * 
 * 	fread((*dataVol),sizeof(unsigned char),voln,fp);
 * 	dsizes[0] = nx;
 * 	dsizes[1] = ny;
 * 	dsizes[2] = nz;
 * 
 * 	fclose(fp);
 * 
 * 	return 0;
 * }
 * 
 * void load_data(void)
 * {
 * 	char * dataname = "datasets/vox/dataset2";
 * 	
 * 	if (load_vox(dataname, sz, &voxels) < 0) {
 * 		fprintf(stderr, "load_data: error loading datafile\n");
 * 		exit(-1);
 * 	}
 * 	&#47;* transfer look up table code here *\/
 * }
 * 
 * void display (void)
 * {
 * 	spaceball.Update();
 * 
 * 	glMatrixMode(GL_MODELVIEW);
 * 	glPushMatrix();
 * 	glMultMatrixf(spaceball.GetMatrix());
 * 	&#47;* manipulate view *\/
 * 	
 * 	glClear(GL_COLOR_BUFFER_BIT);
 * 	vcbVolrSsplats(voxels, nvox, sz);
 * 	glPopMatrix();
 * 
 * 	glutSwapBuffers();
 * }
 * 
 * void cleanup (void){
 * 	vcbVolrCloseS();
 * 	&#47;* cleanup other memory allocations on exit here *\/
 * }
 * 
 * int main(int argc, char ** argv){
 * 	load_data();
 * 	&#47;* initialize the application *\/
 * 
 * 	GLenum err = glewInit();
 * 	if (GLEW_OK != err) {
 * 		fprintf(stderr,"glew error, initialization failed\n");
 * 		fprintf(stderr,"Glew Error: %s\n", glewGetErrorString(err)); 
 * 	} 
 * 
 * 	&#47;* init OpenGLsettings *\/
 * 
 * 	atexit(cleanup);
 * 	vcbVolrInitS(voxtransf);
 * 	glutMainLoop();
 * 	return 0;
 * }</pre>
 **/

/** Function
 * Name: vcbVolrInitS
 * Version: 0.92
 * Description:
 *  vcbVolrInitS: This function specifies the transfer function to use.
 *  It should be called in the main program before entering the main loop.
 * Arguments:
 *  VCB_TRANSFUNC transf: The transfer function defined as VCB_TRANSFUNC. It has 
 *   the exact same syntax as that for ray-casting. Currently, vcb splatting 
 *   only uses 1D transfer functions.
 * Return: None
 * Known Issues: None
 **/
void vcbVolrInitS(VCB_TRANSFUNC transf);

/** Function
 * Name: vcbVolrSsplats
 * Version: 0.95
 * Description:
 *  This function calls with the display function after viewing, 
 *  lighting, etc. have been set up in OpenGL. 
 *	<p>
 *  Voxels must be in a standard format (deemed to be the most memory efficient 
 *  way to store sparse voxels). In total, 10 bytes are needed for each voxel.<br>
 *  For example, in the main program, voxels could be&#58;<pre>
 *	voxels = (unsigned char*)malloc(nvox*10*sizeof(unsigned char)); </pre>
 *	Where&#58; <table width=640>
 *	<tr><td>  voxels[i*10+ 0]   <td> an 8-bit value to be used to index into the transfer function
 *  <tr><td>  voxels[i*10+ 1]   <td> x-component of normal
 *  <tr><td>  voxels[i*10+ 2]   <td> y-component of normal
 *  <tr><td>  voxels[i*10+ 3]   <td> z-component of normal
 *  <tr><td>  voxels[i*10+ 4,5] <td> an unsigned short representing x-coordinate
 *  <tr><td>  voxels[i*10+ 6,7] <td> an unsigned short representing y-coordinate
 *  <tr><td>  voxels[i*10+ 8,9] <td> an unsigned short representing z-coordinate
 *  </table><br>
 *  To avoid problems with endian order&#58;<pre>
 *	unsigned short * pos;
 *	pos = &voxels[i*10+4];
 *	(*pos++) = (unsigned short) x_coord;
 *	(*pos++) = (unsigned short) y_coord;
 *	(*pos)   = (unsigned short) z_coord;</pre>
 * Arguments:
 *  unsigned char* voxels: A pointer to a sparse voxel list
 *  int nvox: The number of voxels in the list.
 *  int* dsizes: An integer array, with dsizes[0], [1], [2]
 *    describing the size of x, y and z dimensions, respectively.
 * Return: None
 * Known Issues: None
 **/
void vcbVolrSsplats(unsigned char * voxels, int nvox, int * dsizes);

/** Function
 * Name: vcbVolrCloseS
 * Version: 0.92
 * Description:
 *  Call this function to clear up all internal data structures before exiting
 *  user program.
 * Arguments: None
 * Return: None
 * Known Issues: None
 **/
void vcbVolrCloseS();

/* End current splatting API */

/* unfinished API */
void vcbVolrSC(VCB_volren volr, vcbdatatype vcbtype, void * voxels, int * dsizes, float * scales);
void vcbVolrTC(VCB_volren volr, vcbdatatype vcbtype, void * voxels, int * dsizes, float * scales);

/* end of API */

/* scene setup functions */

/** Function
 * Name: vcbVolrUpdateLight
 * Version: 0.92
 * Description:
 *	This is an internal function called by vcbVolrX calls at the
 *  beginning, to compute light vectors and half vectors for all
 *  light sources.
 * Arguments:
 *  VCB_volren volr:	The vcb volume renderer context to use
 * Return: None
 * Known Issues: None
 **/
void vcbVolrUpdateLight(VCB_volren volr);

/** Function
 * Name: vcbVolrApplyTransformation
 * Version: 0.92
 * Description:
 *  This function is to be called at the beginning to set
 *  up all transformation matrices. <br>
 *  All the other transformation related parameters are assumed
 *  to be already in place by calling the following&#58; <br>
 *        vcbVolrPerspective, or, vcbVolrOrtho <br>
 *        vcbVolrLookAt <br>
 *        vcbVolrViewPort <br>
 * Arguments:
 *  VCB_volren volr:	The vcb volume renderer context to use
 * Return: None
 * Known Issues: None
 **/
void vcbVolrApplyTransformation(VCB_volren volr);

/** Function
 * Name: vcbVolrUpdateTransformTransl
 * Version: 0.92
 * Description:
 *  This function is to be called to perform translation.
 * Arguments:
 *  VCB_volren volr:	The vcb volume renderer context to use
 *  float* translate[3]: The translation factors in x, y, z direction.
 * Return: None
 * Known Issues: None
 **/
void vcbVolrUpdateTransformTransl(VCB_volren volr, float * translate);

/** Function
 * Name: vcbVolrUpdateTransformRot
 * Version: 0.92
 * Description:
 *  This function is to be called to perform rotation.
 * Arguments:
 *  VCB_volren volr:	The vcb volume renderer context to use
 *  float* rotate[16]:  The rotation matrix
 * Return: None
 * Known Issues: None
 **/
void vcbVolrUpdateTransformRot(VCB_volren volr, float * rotate);

/** Function
 * Name: vcbVolrUpdateTransformScale
 * Version: 0.92
 * Description:
 *  This function is to be called to perform scaling.
 * Arguments:
 *  VCB_volren volr:	The vcb volume renderer context to use
 *  float* scales[3]:   The scaling factors in x, y, z direction
 * Return: None
 * Known Issues: None
 **/
void vcbVolrUpdateTransformScale(VCB_volren volr, float * scales);


#ifdef __cplusplus
}  /* extern C */
#endif


#endif
