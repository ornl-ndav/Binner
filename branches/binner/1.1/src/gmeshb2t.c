/**

   \ingroup rebinner_execs
   
   \file src/gmeshb2t.c

   \brief CURRENT executable to convert from binary to ASCII gmesh formats.
  
   gmesht2b < BINARY_gmesh > ASCII_gmesh
  
   \note This executable is designed to act as a filter.
   
   $Id$

 */

#include <stdlib.h>
#include <stdio.h>


int main(int argc, char ** argv)
{

	int nfacets, i, buf[1];
	double vdata[4 + 8*3];

	for (nfacets = 0; ; nfacets ++) 
    {

		if (fread(buf, sizeof(int), 1, stdin) <= 0)
			break;

		printf("%d", buf[0]);
		
		fread(vdata, sizeof(double), 4 + 8 *3, stdin);
		
		for (i = 0; i < 4 + 8 *3; i ++)
			printf(" %le", vdata[i]);
		
		printf("\n");
	}

	return nfacets;
}
