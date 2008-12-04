#ifndef _BINNERIO_
#define _BINNERIO_

#ifdef __cplusplus
extern "C" {
#endif

void	printvertexf(float *v, int ndim);
void	printvertexd(double *v, int ndim);

int		binnerin3d(int nfacets, float *v); /*return val: nfacets*/
void	binnerout3d(int nfacets, float *v);

int		get_polygonf(float * v);
int		get_polygond(double * v);
int     get_pixelf(int * id, float * cnt, float * err, float (* v)[4]);

int		binnerin_phcf(int nfacets, int * nverts, float * v);
int		binnerin_phcd(int nfacets, int * nverts, double * v);

void	binnerout_phcf(int nfacets, int * nverts, float *v);
void	binnerout_phcd(int nfacets, int * nverts, double *v);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
