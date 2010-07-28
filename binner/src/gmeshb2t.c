#include <stdlib.h>
#include <stdio.h>

/**
 * $Id$
 *
 */

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
