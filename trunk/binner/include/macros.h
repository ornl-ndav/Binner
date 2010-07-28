/**
 * $Id$
 *
 * \file include/macros.h
 */

#ifndef _BINNERMACROS_
#define _BINNERMACROS_


/*! 
   \def BINNER_EPSILON
   Rebinner truncates all numbers smaller than threshold to zero.
*/
#ifndef BINNER_EPSILON
#define BINNER_EPSILON 1e-6
#endif

/*! A convenience macro to compute the norm of a vector \a n.*/
#define BINNER_NORM(n)	sqrt((n)[0]*(n)[0]+(n)[1]*(n)[1]+(n)[2]*(n)[2])

#define REBINDEBUG 0

#endif