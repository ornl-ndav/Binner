/**
   \ingroup rebinner_sdk

   \file include/reducefunc.h

   \brief CURRENT sdk API for controlling map/reduce functions.

   $Id$
*/


#ifndef _REDUCEFUNC_
#define _REDUCEFUNC_


#ifdef __cplusplus
extern "C" {
#endif

int reduce_init(int argc, char ** argv);

/*
 * the input contains k items
 * starting at the address pointed to by v
 *
 * return -1 to indicate that this reduction function
 * has failied
 */
int reduce_func(void * v, int k);

int reduce_done(int nd);

#ifdef __cplusplus
}  /* extern C */
#endif

#endif
