#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "vcbutils.h"
#include "vcbvolren.h"
#include "vcblinalg.h"

/* 
 * in this file we have all vcb volume rendering API calls that
 * are shared by all volume rendering methods, i.e.:
 *     - ray casting
 *     - splatting
 *     - 3D texture mapping hardware
 *
 * method specific API functions are in volray.c, volsplat,c
 *      vol3dtex.c, respectively
 *
 * for expected usage, refer to the description in
 * volray.c under function vcbVolrRC
 */

VCB_volren vcbNewVolRenderer(vcbOpticalModel opticalmodel)
/*
 * initializes a volume rendering context
 * the only parameter required here is the optical model
 * to use, specifying the mathematics setup of the whole framework
 *
 * possible choices are: 
 *    absorption only or emission only: VCB_AOREONLY
 *    absorption and emission:          VCB_ABSEMI
 *
 * return value: NULL for failure, a valid pointer to
 *    the volume rendering context on success
 */
{
	VCB_volren volr = (VCB_volren) malloc (sizeof(struct _volren_struct));
	if (volr == NULL)
		perror("vcbNewVolRenderer");
	else {
		volr->nlights = 0;
		volr->opticalmodel = opticalmodel;
	}

	return volr;
}

void vcbFreeVolRenderer(VCB_volren volr)
/* 
 * the function to remove a vcb volume rendering context pointed to
 * by volr from memory
 */
{
	if (volr != NULL)
		free(volr);
}

void vcbVolrPerspective(VCB_volren volr, float fov, float aspect, float hither, float yon)
/*
 * to set up a perspective projection
 * inputs (exactly the same as glPerspective):
 *    volr:   pointer to the vcb volume rendering context in use
 *    fov:    field of view in Y direction
 *    aspect: aspect ratio (Y/X)
 *    hither: near clipping z
 *    yon:    far  clipping z
 */
{
	volr->projmode = VCB_PERSPECTIVE;
	volr->fov = fov;
	volr->aspect = aspect;
	volr->hither = hither;
	volr->yon = yon;
}

void vcbVolrOrtho(VCB_volren volr, float hither, float yon, float xmin, float xmax, float ymin, float ymax)
/*
 * to set up a orthogonal projection
 * inputs (exactly the same as glOrho3):
 *    volr:   pointer to the vcb volume rendering context in use
 *    xmin:   left boundary
 *    xmax:   right boundary
 *    ymin:   bottom boundary
 *    ymax:   top boundary
 *    hither: near clipping z
 *    yon:    far  clipping z
 */
{
	volr->projmode = VCB_ORTHOGONAL;
	volr->hither = hither;
	volr->yon = yon;
	volr->xmin = xmin;
	volr->xmax = xmax;
	volr->ymin = ymin;
	volr->ymax = ymax;
}

void vcbVolrLookAt(VCB_volren volr,
				   float centerx, float centery, float centerz, 
				   float eyex, float eyey, float eyez, 
				   float upx, float upy, float upz)
/*
 * to set up a camera system
 * inputs (exactly the same as gluLookAt):
 *    volr:        pointer to the vcb volume rendering context in use
 *    centerx,y,z: position of center of interest
 *    eyex,y,z:    position of eye/viewer/camera
 *    upx,y,z:     the upward direction of the camera
 *
 * Note: the up direction given should not be parallel to the direction
 *       pointing from coi to eye. the results is undefined for this
 *       situation.
 */
{
	float vdir[3], dot;
	volr->center[0] = centerx;
	volr->center[1] = centery;
	volr->center[2] = centerz;

	volr->eye[0] = eyex;
	volr->eye[1] = eyey;
	volr->eye[2] = eyez;

	volr->up[0] = upx;
	volr->up[1] = upy;
	volr->up[2] = upz;
	la_normalize3(volr->up);

	la_dupvec(vdir, volr->eye);
	la_subtract(vdir, volr->center);
	la_normalize3(vdir);
	dot = la_dot3(vdir, volr->up);
	volr->up[0] -= vdir[0] * dot;
	volr->up[1] -= vdir[1] * dot;
	volr->up[2] -= vdir[2] * dot;
	la_normalize3(volr->up);

}

int  vcbVolrLight(VCB_volren volr, float lightx, float lighty, float lightz)
/*
 * IMPORTANT: this API is now obsolete. one should always use
 *            vcbVolrSetLight instead
 *
 * to set up a new light source
 * inputs:
 *    volr:       pointer to the vcb volume rendering context in use
 *    lightx,y,z: position of the light source
 *                all vcb volume renderer light sources are directional
 *                light source pointing towards coi
 *
 * return value:  the total number of light sources in volr context
 *
 * Note: currently all light sources are defaulted to have
 *       0.f ambient intensity, 1.0 diffuse intensity and 
 *       1.0 specular intensity. all lights sources are assumed to be
 *       of white color, a exponential coefficient of 40.
 *       extending the vcbVolrLight API to allow specifying these
 *       parameters is not difficult, but not yet top priority.
 */
{
	int nlights = volr->nlights;
	if (nlights >= VCB_MAXLIGHT)
		return nlights; /* if already reached max number of lights, do nothing */

	volr->light[nlights * 3 + 0] = lightx;
	volr->light[nlights * 3 + 1] = lighty;
	volr->light[nlights * 3 + 2] = lightz;

	volr->ambient[nlights] = .2f;
	volr->diff[nlights] = 1.f;
	volr->spec[nlights] = 1.f;

	volr->nlights++;
	return volr->nlights;
}

int  vcbVolrSetLight(VCB_volren volr, int light_id, float lightx, float lighty, float lightz)
/*
 * to set up a particular light source
 * inputs:
 *    volr:       pointer to the vcb volume rendering context in use
 *    light_id:   which light source to modify, valid numbers are 0 to (VCB_MAXLIGHT-1)
 *                VCB_MAXLIGHT == 8 in current implementation
 *    lightx,y,z: position of the light source
 *                all vcb volume renderer light sources are directional
 *                light source pointing towards coi
 *
 * return value:  -1 if invalid operation, 0 for normal exit
 *
 * Note: currently all light sources are defaulted to have
 *       0.f ambient intensity, 1.0 diffuse intensity and 
 *       1.0 specular intensity. all lights sources are assumed to be
 *       of white color, a exponential coefficient of 40.
 *       extending the vcbVolrLight API to allow specifying these
 *       parameters is not difficult, but not yet top priority.
 * 
 * Very important: current vcbvolren libraries all assume only one light source. in other
 *       words, although one can set up as many as 8 light sources, only light source 0 
 *       gets used for illumination computation.
 */
{
	if (light_id < 0 || light_id > VCB_MAXLIGHT) return -1;

	volr->light[light_id * 3 + 0] = lightx;
	volr->light[light_id * 3 + 1] = lighty;
	volr->light[light_id * 3 + 2] = lightz;

	volr->ambient[light_id] = 0.f;
	volr->diff[light_id] = 1.f;
	volr->spec[light_id] = 1.f;

	volr->nlights = light_id + 1;

	return 0;
}


void vcbVolrUpdateLight(VCB_volren volr)
/*
 * this is an internal function called by vcbVolrX calls at the
 * beginning to computed light vectors and half vectors for all
 * light sources.
 * input:
 *    volr: the vcb volume renderer context to use
 */
{
	int i;
	float evec[3], *lvec, * half;

	la_dupvec(evec, volr->eye);
	la_subtract(evec, volr->center);
	la_normalize3(evec);

	for (i = 0; i < volr->nlights; i ++) {
		lvec = &volr->lvec[i*3];
		half = &volr->half[i*3];
		la_dupvec(lvec, &volr->light[i*3]);
		la_subtract(lvec, volr->center);
		la_normalize3(lvec);
		la_dupvec(half, evec);
		la_add(half, lvec);
		la_normalize3(half);
	}
}

void vcbVolrViewPort(VCB_volren volr, int w, int h)
/*
 * to set up a viewport
 * intputs:
 *    volr: the vcb volume renderer context to use
 *    w,h:  width and height of the viewport
 */
{
	volr->imgw = w;
	volr->imgh = h;
}

void vcbVolrTransFunc(VCB_volren volr, VCB_TRANSFUNC transf)
/*
 * to specify a transfer functiont to use in the volr context
 * this is a unique feature offered in vcb volume renderer.
 *
 * here, the transfer function is literally a function that user
 * can specify and pass to vcb volume renderer as a function
 * pointer. The only requirement is a very general type is 
 * obided by:
 *          void (* VCB_TRANSFUNC) (float *, void *);
 * An example transfer function is provided below for
 * illustration:
 *    void voxtransf (float * rgba, void * vox)
 *    {
 *       unsigned char * voxel = (unsigned char *) vox;
 *       int id = voxel[0] * 4;
 *       rgba[0] = tlut[id+0];
 *       rgba[1] = tlut[id+1];
 *       rgba[2] = tlut[id+2];
 *       rgba[3] = tlut[id+3];
 *    }
 *
 * the output of transf is always the first pointer, rgba, pointing
 * to 4 floating points, corresponding to R, G, B and ALPHA channels.
 *
 * the input can vary. on each voxel in a volume, there can be multiple
 * variables. vcb volume renderer reconstructs all those variables on
 * every volume sample and pass to transf via the pointer vox.
 *
 * inside transf, the user is free to interpret all these variables
 * in whatever way that is the most meaningful. here, in this example,
 * all we are doing is a simple table lookup. tlut is a global variable
 * in the user's main.c/cpp.
 *
 * according to our measurement, the overhead of calling and actual
 * function to obtain transfer function classification is below a
 * neglible 1.5% of total rendering time.
 */
{
	volr->transf = transf;
}

void vcbVolrFramebuffer(VCB_volren volr, vcbVolrenDst dst, vcbdatatype  fbtype, ...)
/*
 * this is a unique feature of vcb volume renderer
 *
 * here one can specify a destination of the renderer using the volr context
 *
 * the two choices are specified in dst: 
 *    VCB_OPENGL: the result directly get shown on an OpenGL screen
 *    VCB_MEMORY: the result is located a user provided memory framebuffer
 *
 * fbtype: specifies the type of each channel in the framebuffer, for
 *    instance, VCB_UNSIGNEDBYTE.
 *
 * if the dst is VCB_MEMORY, then a 4th input argument is expect, that is
 *    a pointer to the user provided memory framebuffer.
 *
 * Usage example:
 *	  unsigned char * framebuffer;
 *    framebuffer = (unsigned char *) malloc (w*h*4*sizeof(unsigned char));
 *    vcbVolrViewPort(volren, w, h);
 *    vcbVolrFramebuffer(volren, VCB_MEMORY, VCB_UNSIGNEDBYTE, framebuffer);
 *
 * Note: currently, only VCB_MEMORY destination is correctly implemented,
 *    native handling of VCB_OPENGL destination assumes a primitive semantic,
 *    that is to render to an intern static memory segment which is not
 *    accessible to user program. To be fixed soon.
 */
{
	va_list ap;

	volr->dst = dst;
	volr->fbtype = fbtype;

	va_start(ap, fbtype);
	if (dst == VCB_MEMORY)
		volr->fbuffer = va_arg(ap, void *);

	va_end(ap);
}

void vcbVolrApplyTransformation(VCB_volren volr)
/* 
 * a function to be called at the beginning to set 
 * up all transformation matrices.
 *   volr:   the vcb volume renderer context to use
 *  
 *   all the other transformation related parameters are assumed
 *   to be already in place by calling:
 *
 *        vcbVolrPerspective, or, vcbVolrOrtho
 *        vcbVolrLookAt
 *        vcbVolrViewPort
 */
{
	float lookatmat[16], projmat[16], viewportmat[16];

	vcbLookAt(lookatmat, volr->eye, volr->center, volr->up);

	if (volr->projmode == VCB_PERSPECTIVE)
		vcbPerspective3D(projmat,volr->fov, volr->aspect, volr->hither, volr->yon);
	else
		vcbOrtho3D(projmat,volr->xmin, volr->xmax, volr->ymin, volr->ymax, volr->hither, volr->yon);

	vcbViewPortMat(viewportmat, volr->imgw, volr->imgh);

	vcbIdentity3D(volr->obj2scr);
	vcbMatMultM(volr->obj2scr, viewportmat);
	vcbMatMultM(volr->obj2scr, projmat);
	vcbMatMultM(volr->obj2scr, lookatmat);

	vcbInvMat(volr->scr2obj, volr->obj2scr);
}


void vcbVolrUpdateTransformTransl(VCB_volren volr, float * translate)
/* 
 * a function to be called when translation is performed
 *   volr:         the vcb volume renderer context to use
 *   translate[3]: the translation factors in x, y, z direction
 */
{
	float translatemat[16];

	vcbTranslate3fv(translatemat,translate);
    vcbMatMultM(volr->obj2scr, translatemat);
	vcbInvMat(volr->scr2obj, volr->obj2scr);
}

void vcbVolrUpdateTransformRot(VCB_volren volr, float * rotate)
/* 
 * a function to be called when rotation is performed
 *   volr:       the vcb volume renderer context to use
 *   rotate[16]: the rotation matrix
 */
{
    vcbMatMultM(volr->obj2scr, rotate);
	vcbInvMat(volr->scr2obj, volr->obj2scr);
}


void vcbVolrUpdateTransformScale(VCB_volren volr, float * scales)
/* 
 * a function to be called when scaling is performed
 *   volr:      the vcb volume renderer context to use
 *   scales[3]: the scaling factors in x, y, z direction
 */
{
	float scalemat[16];

	vcbScale3fv(scalemat,scales);
    vcbMatMultM(volr->obj2scr, scalemat);
	vcbInvMat(volr->scr2obj, volr->obj2scr);
}
