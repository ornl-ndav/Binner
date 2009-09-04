#include <unistd.h>
#include "reducefunc.h"

#define ITEMSIZE 36 //(sizeof(int)*3 + sizeof(double)*2)

int reducefunc_unitsize()
{
	return ITEMSIZE;
}

int reducefunc(void * v, int k)
{
	/*
	 * the input contains k items
	 * starting at the address pointed to by v
	 */

	write(1, v, k*ITEMSIZE);

	return 0;
}
