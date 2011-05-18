/**
   \ingroup rebinner_core
   
   \file include/cell.h

   \brief CURRENT core API -- functions to generate cells and fix topology

   $Id$

 */


#ifndef _BINNERCELL_
#define _BINNERCELL_


#ifdef __cplusplus
extern "C" {
#endif

int basicCubef(float * v);
int basicCubed(double * v);
int realCubef(float (* corners)[4], float * v);
int realCube3d(double * corners, double * v);
int correctCornersf3d(float (* vertices)[4], int * ids);
void parallelipedByInputd(double * v);
void parallelipedByInputf(float * v);
void facetsRot3f(int nfacets, float * v, float * axis, float angle);
int facetsScale3f(int nfacets, float * v, float * s);
void facetsTrans3d(int nfacets, float *v, float * t);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
