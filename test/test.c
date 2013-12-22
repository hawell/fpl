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

#include "mathfuncs.h"
#include "image.h"
#include "process.h"
#include "clahe.h"
#include "lro.h"
#include "fingerprint.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main()
{
	mat* orig;
	int res = load_from_file("test.pgm", &orig);
	if (res)
	{
		printf("load failed\n");
		return -1;
	}

	resize(orig, 256, 256);

	mat* smooth;
	convolve(orig, &smooth, KERNEL_GSF5x5, DIRECTION_NONE);

	save_to_file("smooth.pgm", smooth);

	mat* grad;
	mat* dir;

	gradient(smooth, &grad, &dir, KERNEL_NRIGO7x5);

	save_to_file("grad.pgm", grad);

	ridge_orientation* lro;
	lro_gradient(grad, dir, 7, 4, &lro);
	save_orientation_image("lro.pgm", lro);

	for (int i=1; i<dir->height-1; i++)
		for (int j=1; j<dir->width-1; j++)
		{
			double theta = (dir->data[(i)*dir->width + j]*grad->data[(i)*grad->width + j] +
					dir->data[(i-1)*dir->width + j]*grad->data[(i-1)*grad->width + j] + dir->data[(i+1)*dir->width + j]*grad->data[(i+1)*grad->width + j] +
					dir->data[(i)*dir->width + j-1]*grad->data[(i)*grad->width + j-1] + dir->data[(i)*dir->width + j+1]*grad->data[(i)*grad->width + j+1] +
					dir->data[(i-1)*dir->width + j+1]*grad->data[(i-1)*grad->width + j+1] + dir->data[(i+1)*dir->width + j+1]*grad->data[(i+1)*grad->width + j+1] +
					dir->data[(i-1)*dir->width + j-1]*grad->data[(i-1)*grad->width + j-1] + dir->data[(i+1)*dir->width + j]*grad->data[(i+1)*grad->width + j]) / grad->max_val*9.0;

			int n1, n2;
			if (theta < 22.5)
			{
				n1 = grad->data[i*grad->width + j-1];
				n2 = grad->data[i*grad->width + j+1];
			}
			else if (theta < 67.5)
			{
				n1 = grad->data[(i-1)*grad->width + j+1];
				n2 = grad->data[(i+1)*grad->width + j-1];
			}
			else if (theta < 112.5)
			{
				n1 = grad->data[(i-1)*grad->width + j];
				n2 = grad->data[(i+1)*grad->width + j];
			}
			else if (theta < 157.5)
			{
				n1 = grad->data[(i-1)*grad->width + j-1];
				n2 = grad->data[(i+1)*grad->width + j+1];
			}
			else
			{
				n1 = grad->data[i*grad->width + j-1];
				n2 = grad->data[i*grad->width + j+1];
			}
			int* n = &grad->data[i*grad->width + j];
			if (*n < n1 || *n < n2)
				*n = 0;
		}

	save_to_file("nmsup2.pgm", grad);

	for (int i=0; i<grad->height; i++)
		for (int j=0; j<grad->width; j++)
		{
			int* pos = &grad->data[i*grad->width + j];
			if (*pos > 50)
				*pos = 255;
			else if (*pos > 20)
				*pos = 128;
			else
				*pos = 0;
		}

	save_to_file("dthresh.pgm", grad);

	return 0;
}
