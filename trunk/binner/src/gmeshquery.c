#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "binner.h"

static char buf[4096];
static char fn[2048];

static double  fimin,fjmin,fkmin,fimax,fjmax,fkmax;

void init_bounds()
{
	fimin = -1e16;
	fimax = 1e16;
	fjmin = -1e16;
	fjmax = 1e16;
	fkmin = -1e16;
	fkmax = 1e16;
}

int main(int argc, char ** argv)
{
	int i, n, tag;
	double bounds[6];

	if ((argc <= 1) || (argc >= 10))
	{
		fprintf(stderr, "usage: %s [-x xmin xmax] [-y ymin ymax] [-z zmin zmax] \n", argv[0]);
		exit(1);
	}

#if REBINDEBUG
	for (i = 0; i < argc; printf("%s\n", argv[i]), i ++);
#endif

	init_bounds();

	i = 1;
	if (i <= argc - 3 && 0 == strcmp("-x", argv[i]))
	{
		fimin = atof(argv[i+1]);
		fimax = atof(argv[i+2]);
		i += 3;
	}

	if (i <= argc - 3 && 0 == strcmp("-y", argv[i]))
	{
		fjmin = atof(argv[i+1]);
		fjmax = atof(argv[i+2]);
		i += 3;
	}

	if (i <= argc - 3 && 0 == strcmp("-z", argv[i]))
	{
		fkmin = atof(argv[i+1]);
		fkmax = atof(argv[i+2]);
		i += 3;
	}
	
#if REBINDEBUG
	fprintf(stderr, "bounds: %le %le %le %le %le %le\n", fimin, fimax,fjmin,fjmax,fkmin,fkmax);
#endif

	for (i = 0; fgets(buf, 4096, stdin) != NULL; i++)
	{

		sscanf(buf, "%s %d %le %le %le %le %le %le\n", fn, &n, &bounds[0], &bounds[1], &bounds[2], &bounds[3], &bounds[4], &bounds[5]); 

		if (bounds[0] < fimin) bounds[0] = fimin;
		if (bounds[2] < fjmin) bounds[2] = fjmin;
		if (bounds[4] < fkmin) bounds[4] = fkmin;
		if (bounds[1] > fimax) bounds[1] = fimax;
		if (bounds[3] > fjmax) bounds[3] = fjmax;
		if (bounds[5] > fkmax) bounds[5] = fkmax;

		tag = 1;
		if (bounds[1] < fimin) tag = 0;
		if (bounds[3] < fjmin) tag = 0;
		if (bounds[5] < fkmin) tag = 0;
		if (bounds[0] > fimax) tag = 0;
		if (bounds[2] > fjmax) tag = 0;
		if (bounds[4] > fkmax) tag = 0;
		
		if (tag > 0)
			fprintf(stdout, "%s %d\n", fn, n);

	}

	return 0;
}
