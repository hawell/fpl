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
	int res = load_from_file("resize.pgm", &orig);
	if (res)
	{
		printf("load failed\n");
		return -1;
	}

	mat* smooth;
	convolve(orig, &smooth, KERNEL_GAUSSIANBLUR5x5, DIRECTION_NONE);

	save_to_file("smooth.pgm", smooth);

	histogram_equalization(smooth);

	save_to_file("smooth2.pgm", smooth);


	mat *grad, *gx, *gy;
	mat* dir;

	gradient_x(smooth, &gx, KERNEL_SOBEL3x3);
	save_to_file("gx.pgm", gx);

	gradient_y(smooth, &gy, KERNEL_SOBEL3x3);
	save_to_file("gy.pgm", gy);

	gradient(smooth, &grad, &dir, KERNEL_SOBEL3x3);

	save_to_file("grad.pgm", grad);

	ridge_orientation* lro;
	lro_stdev(grad, 7, 16, &lro);
	save_orientation_image("lro6.pgm", lro);

	LDMF(smooth, lro, 5);

	save_to_file("ldmf.pgm", smooth);

	binarize(smooth, 10);

	save_to_file("bin.pgm", smooth);

	thin(smooth);

	save_to_file("thin.pgm", smooth);

	return 0;
}
