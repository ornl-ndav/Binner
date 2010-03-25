#include <stdlib.h>
#include <stdio.h>

void  matMult3x3(double * v0, double * mat, double * v1)
{
	double v[4];
	v[0] = mat[0] *v1[0] + mat[1] *v1[1] + mat[2] *v1[2]; /* + mat[3] *v1[3]; */
	v[1] = mat[4] *v1[0] + mat[5] *v1[1] + mat[6] *v1[2]; /* + mat[7] *v1[3]; */
	v[2] = mat[8] *v1[0] + mat[9] *v1[1] + mat[10]*v1[2]; /* + mat[11]*v1[3]; */
	/*v[3] = mat[12]*v1[0] + mat[13]*v1[1] + mat[14]*v1[2] + mat[15]*v1[3];*/

	v0[0] = v[0]; v0[1] = v[1]; v0[2] = v[2]; /* v0[3] = 1.f; */
}

int main(int argc, char ** argv)
{
	int i, n, buf[1];
	double vdata[4 + 8*3], *v;
	double mat[16];
	
	if (argc != 10)
	{
		fprintf(stderr, "usage: %s mat0 mat1 mat2 mat3 mat4 mat5 mat6 mat7 mat8\n", argv[0]);
		fprintf(stderr, "       %s expects a 3x3 matrix in row-major order for input\n", argv[0]);
		exit(1);
	}
	
	mat[0] = atof(argv[1]);
	mat[1] = atof(argv[2]);
	mat[2] = atof(argv[3]);
	mat[4] = atof(argv[4]);
	mat[5] = atof(argv[5]);
	mat[6] = atof(argv[6]);
	mat[8] = atof(argv[7]);
	mat[9] = atof(argv[8]);
	mat[10] = atof(argv[9]);

	v = vdata + 4;
	
	for (n = 0; ; n ++) 
    {

		if (fread(buf, sizeof(int), 1, stdin) <= 0)
			break;

		fwrite(buf, sizeof(int), 1, stdout);
		
		fread(vdata, sizeof(double), 4 + 8 *3, stdin);

		for (i = 0; i < 8; i ++)
			matMult3x3(v + i * 3, mat, v + i*3);

		fwrite(vdata, sizeof(double), 4 + 8*3, stdout);
	}

	return 0;
}
