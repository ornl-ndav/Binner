#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "vcbmath.h"
#include "kernel.h"

static double stepsz = 0.025; /* step size in numeric integration */

double fptenc(double r, double fz, double lz)
/* 
 * numerically integrate the footprint entry of a crawfisian
 * kernel at radius r, from fz to lz, all in image space
 */
{
	double val = 0, r3;

	while (fz < lz) {
		r3   = sqrt(fz*fz+r*r);
		val += crawfisian(r3)*stepsz;
		fz  += stepsz;
	}

	return val;
}

double fpteng(double r, double fz, double lz)
/* 
 * numerically integrate the footprint entry of a gaussian
 * kernel at radius r, from fz to lz, all in image space
 */
{
	double val = 0, r3;

	while (fz < lz) {
		r3   = sqrt(fz*fz+r*r);
		val += gaussian(r3)*stepsz;
		fz  += stepsz;
	}

	return val;
}

static double (* fpten) (double, double, double);
static double kradius;
#ifndef LUTRES
#define LUTRES 256
#endif

static void   gen_fptTable2d(int res, float * tab, float fz, float lz)
{
	float t;
	float lut[LUTRES];
	double x, y, cloc, radius;
	int i, id, tabsz;

	for (i = 0; i < LUTRES; i ++)
		lut[i] = (float) fpten(i*kradius/LUTRES, fz, lz);

	tabsz = res * res;
	cloc = (res - 1.)/2.; /* center location */
	for (i = 0; i < tabsz; i ++) {
		x = ((i % res) - cloc)/cloc; /* normalized x coordinate: [0,1] */
		y = ((i / res) - cloc)/cloc; /* normalized y coordinate: [0,1] */
		radius = sqrt(x*x + y*y)*LUTRES; /* index into the 1D lookup table */
		if (radius >= LUTRES)
			tab[i] = 0.f;
		else {
			id = (int) radius;
			t  = (float)(radius - id);
			tab[i] = lut[id] * (1.f-t) + lut[id+1]*t; /* linear interpolate */
		}
	}
}

static void   gen_fptTable1d(int res, float * tab, float fz, float lz)
{
	int i;
	for (i = 0; i < res; i ++)
		tab[i] = (float) fpten(i*kradius/res, fz, lz);
}

void fptTableC2d(int res, float * tab, float fz, float lz)
{
	fpten = fptenc;
	kradius = CRAWFISIANFOOTPRINTRADIUS;
	if (fz < NCRAWFISIANFOOTPRINTRADIUS)
		fz = (float)NCRAWFISIANFOOTPRINTRADIUS;
	if (lz > CRAWFISIANFOOTPRINTRADIUS)
		lz = (float)CRAWFISIANFOOTPRINTRADIUS;

	gen_fptTable2d(res, tab, fz, lz);
}

void fptTableG2d(int res, float * tab, float fz, float lz)
{
	fpten = fpteng;
	kradius = GAUSSIANFOOTPRINTRADIUS;
	if (fz < NGAUSSIANFOOTPRINTRADIUS)
		fz = (float)NGAUSSIANFOOTPRINTRADIUS;
	if (lz > GAUSSIANFOOTPRINTRADIUS)
		lz = (float)GAUSSIANFOOTPRINTRADIUS;

	gen_fptTable2d(res, tab, fz, lz);
}

void fptTableC1d(int res, float * tab, float fz, float lz)
{
	fpten = fptenc;
	kradius = CRAWFISIANFOOTPRINTRADIUS;
	if (fz < NCRAWFISIANFOOTPRINTRADIUS)
		fz = (float)NCRAWFISIANFOOTPRINTRADIUS;
	if (lz > CRAWFISIANFOOTPRINTRADIUS)
		lz = (float)CRAWFISIANFOOTPRINTRADIUS;

	gen_fptTable1d(res, tab, fz, lz);
}

void fptTableG1d(int res, float * tab, float fz, float lz)
{
	fpten = fpteng;
	kradius = GAUSSIANFOOTPRINTRADIUS;
	if (fz < NGAUSSIANFOOTPRINTRADIUS)
		fz = (float)NGAUSSIANFOOTPRINTRADIUS;
	if (lz > GAUSSIANFOOTPRINTRADIUS)
		lz = (float)GAUSSIANFOOTPRINTRADIUS;

	gen_fptTable1d(res, tab, fz, lz);
}

