/**
   \ingroup rebinner_core
   
   \file include/clip.h

   \brief CURRENT core API -- base module to support bin_para3dclip in binner.h

   $Id$

 */

#ifndef _BINNERCLIP_
#define _BINNERCLIP_


#define INTERP( t, x1, x2 )   ( (x1) + (t) * ((x2) - (x1)) )

#ifdef __cplusplus
extern "C" {
#endif

int clip(int nv, double * verts, double * vbuf, int * onbuf, double * cplane);

int clip_polyhedral(int nfaces, int ** nverts, double ** verts, double * cplane);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
