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
	mat* img;
	int res = load_from_file("sample2.pgm", &img);
	if (res)
	{
		printf("load failed\n");
		return -1;
	}

	resize(img, 256, 256);

	//	negative(img);

	mat* img2;
	convolve(img, &img2, KERNEL_GAUSSIANBLUR5x5, DIRECTION_NONE);
	normalize(img2, 0, 255);
	save_to_file("gsf.pgm", img2);
	negative(img2);

	ridge_orientation* lro;
	get_ridge_orientation(img2, &lro, LRO_GRAD);

/*
	mat* img3;
	gradient(img2, &img3, KERNEL_SOBEL3x3);
	save_to_file("gradient.pgm", img3);

	ROI(img2, img3, 16);
	save_to_file("roi.pgm", img2);

	binarize(img2, 10);
	save_to_file("bin.pgm", img2);

	thin(img2);
	save_to_file("thin.pgm", img2);

	free_image(img);
*/
	return 0;
}
