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

#include "filter.h"
#include "convolution.h"

#include <stdlib.h>

static kernel sobel3x3 = {
		{1, 2, 1},
		{-1, 0, 1},
		3,3
};

static kernel scharr3x3 = {
		{3, 10, 3},
		{-1, 0, 1},
		3,3
};

static kernel nrigo5x3 = {
		{1/4.0f, 2/4.0f, 1/4.0f},
		{-1/8.0f, -2/8.0f, 0, 2/8.0f, 1/8.0f},
		3,5
};

static kernel nrigo7x5 = {
		{1/16.0f, 4/16.0f, 6/16.0f, 4/16.0f, 1/16.0f},
		{-1/32.0f,-4/32.0f, -5/32.0f, 0, 5/32.0f, 4/32.0f, 1/32.0f},
		5,7
};

static kernel gsf3x3 = {
		{1/4.0f, 2/4.0f, 1/4.0f},
		{1/4.0f, 2/4.0f, 1/4.0f},
		3,3
};

static kernel gsf5x5 = {
		{1/16.0f, 4/16.0f, 6/16.0f, 4/16.0f, 1/16.0f},
		{1/16.0f, 4/16.0f, 6/16.0f, 4/16.0f, 1/16.0f},
		5,5
};

static kernel laplacian3x3 = {
		{1, -2, 1},
		{1, -2, 1},
		3,3
};

static kernel mean5x5 = {
		{1/5.0f, 1/5.0f, 1/5.0f, 1/5.0f, 1/5.0f},
		{1/5.0f, 1/5.0f, 1/5.0f, 1/5.0f, 1/5.0f},
		5,5
};

int filter(image* img_in, mat** out, int krnl, int dir)
{
	kernel* k;
	switch (krnl)
	{
		case KERNEL_SOBEL3x3:k = &sobel3x3;break;
		case KERNEL_SCHARR3x3:k = &scharr3x3;break;
		case KERNEL_NRIGO5x3:k = &nrigo5x3;break;
		case KERNEL_NRIGO7x5:k = &nrigo7x5;break;
		case KERNEL_GSF3x3:k = &gsf3x3;break;
		case KERNEL_GSF5x5:k = &gsf5x5;break;
		case KERNEL_LAPLACIAN3x3:k = &laplacian3x3;break;
		case KERNEL_MEAN5x5:k = &mean5x5;break;

		default:
		{
			// invalid kernel
			return 1;
		}
	}

	(*out) = malloc(sizeof(mat));
	(*out)->width = img_in->width;
	(*out)->height = img_in->height;
	(*out)->data = malloc(img_in->width*img_in->height*sizeof(int));

	if (dir == DIRECTION_VERTICAL)
		convolve2DSeparable(img_in->data, (*out)->data, img_in->width, img_in->height,
				k->row, k->row_count, k->column, k->column_count);
	else
		convolve2DSeparable(img_in->data, (*out)->data, img_in->width, img_in->height,
				k->column, k->column_count, k->row, k->row_count);

	return 0;
}

