/*
 * dft.c
 *
 *  Created on: Nov 18, 2013
 *      Author: arash
 */

#include <stdio.h>
#include <math.h>

#include "mathfuncs.h"

int main()
{
	double re[] = {
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 9, 9, 9, 9, 0, 0,
			0, 0, 9, 9, 9, 9, 0, 0,
			0, 0, 9, 9, 9, 9, 0, 0,
			0, 0, 9, 9, 9, 9, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0
	};
	double im[] = {
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0
	};

	dft2d(1, 3, 3, re, im);
	for (int i=0; i<64; i++)
	{
		if (i % 8 == 0)
			printf("\n");
		printf("%4.0lf ", sqrt(re[i]*re[i] + im[i]*im[i]));
	}
	dft2d(-1, 3, 3, re, im);
	for (int i=0; i<64; i++)
	{
		if (i % 8 == 0)
			printf("\n");
		printf("%4.0lf %4.0lf ", re[i], im[i]);
	}
	printf("\n");
	return 0;
}
