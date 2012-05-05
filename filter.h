/*
 *  Copyright (c) 2012 Arash Kordi <arash.cordi@gmail.com>
 *
 *  This file is part of fpl.
 *
 *  fpl is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  fpl is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with fpl.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef FILTER_H_
#define FILTER_H_

#include "image.h"

#define MAX_FILTER_LEN		10

typedef struct {
	float column[MAX_FILTER_LEN];
	float row[MAX_FILTER_LEN];
	int column_count, row_count;
} kernel;

#define DIRECTION_NONE			0
#define DIRECTION_VERTICAL		1
#define DIRECTION_HORIZONTAL	2

#define KERNEL_SOBEL3x3		1
#define KERNEL_SCHARR3x3	2

#define KERNEL_NRIGO5x3		3
#define KERNEL_NRIGO7x5		4

#define KERNEL_GSF3x3		5
#define KERNEL_GSF5x5		6

#define KERNEL_LAPLACIAN3x3	7

#define KERNEL_MEAN5x5		8

int filter(image* img_in, mat** out, int krnl, int dir);

#endif /* FILTER_H_ */
