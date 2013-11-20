/*
 * matfuncs.h
 *
 *  Created on: Nov 6, 2013
 *      Author: arash
 */

#ifndef MATFUNCS_H_
#define MATFUNCS_H_

#include "image.h"

#define PI	3.141592654

#define DIRECTION_NONE			0
#define DIRECTION_VERTICAL		1
#define DIRECTION_HORIZONTAL	2

#define KERNEL_SOBEL3x3		1
#define KERNEL_SCHARR3x3		2
#define KERNEL_NRIGO5x3		3
#define KERNEL_NRIGO7x5		4
#define KERNEL_GSF3x3			5
#define KERNEL_GSF5x5			6
#define KERNEL_LAPLACIAN3x3	7
#define KERNEL_MEAN5x5			8

#define KERNEL_SEPERABLE		100

#define KERNEL_GAUSSIANBLUR5x5	101


int convolve(mat* img_in, mat** out, int krnl, int dir);

void get_line(int cx, int cy, int radius, int a, int *x, int *y);

#define DFT_DIRECT		1
#define DFT_REVERSE	-1

void dft(short int dir, long m, double *x, double *y);
void dft2d(short int dir, long nx, long ny, double *x, double *y);

#endif /* MATFUNCS_H_ */
