#ifndef _REDUCEFUNC_
#define _REDUCEFUNC_

#ifdef __cplusplus
extern "C" {
#endif

int reducefunc_unitsize();

/*
 * the input contains k items
 * starting at the address pointed to by v
 *
 * return -1 to indicate that this reduction function
 * has failied
 */
int reducefunc(void * v, int k);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
