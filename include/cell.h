#ifndef _BINNERCELL_
#define _BINNERCELL_

#ifdef __cplusplus
extern "C" {
#endif

int basicCubef(float * v);
int basicCubed(double * v);
void parallelipedByInputd(double * v);
void parallelipedByInputf(float * v);
void facetsRot3f(int nfacets, float * v, float * axis, float angle);
int facetsScale3f(int nfacets, float * v, float * s);
void facetsTrans3d(int nfacets, float *v, float * t);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif