/*
 * dft.c
 *
 *  Created on: Nov 18, 2013
 *      Author: arash
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "mathfuncs.h"
#include "image.h"
#include "process.h"

int main()
{

	mat* flt;
	load_from_file("square.pgm", &flt);

	double *fre = malloc(sizeof(double)*flt->len);
	double *fim = malloc(sizeof(double)*flt->len);
	for (int i=0; i<flt->len; i++)
		fre[i] = flt->data[i]>0?1:0;
	memset(fim, 0, sizeof(double)*flt->len);

	mat* img;
	load_from_file("resize.pgm", &img);

	double *re = malloc(sizeof(double)*img->len);
	for (int i=0; i<img->len; i++)
		re[i] = img->data[i];
	double *im = malloc(sizeof(double)*img->len);
	memset(im, 0, sizeof(double)*img->len);

	dft2d(1, 8, 8, re, im);
	for (int i=0; i<img->len; i++)
	{
		re[i] = fre[i]*re[i];
		im[i] = fre[i]*im[i];
	}
	dft2d(-1, 8, 8, re, im);
	for (int i=0; i<img->len; i++)
	{
		img->data[i] = re[i];
	}
	save_to_file("rdft.pgm", img);
	return 0;
}
