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

#include "image.h"
#include "filter.h"
#include "process.h"
#include "enhance.h"
#include "linear.h"
#include "clahe.h"
#include "morph.h"
#include "directional.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int main()
{
	image* img;
	int res = load_from_file("scan_02.pgm", &img);
	if (res)
		printf("load failed\n");

	ridge_orientation(img, 4);
		save_to_file("lro.pgm", img, IMAGE_DATA_LRO);

	remove_background(img, 8);
	save_to_file("rmbg.pgm", img, IMAGE_DATA_MAIN);

	LDMF(img, 3);
	save_to_file("ldmf.pgm", img, IMAGE_DATA_MAIN);

	clahe(img, 10, 14, 0.1);
	save_to_file("clahe.pgm", img, IMAGE_DATA_MAIN);

/*
	remove_background(img, 8);
	save_to_file("rmbg2.pgm", img, IMAGE_DATA_MAIN);

	LDMF(img, 3);
	save_to_file("ldmf2.pgm", img, IMAGE_DATA_MAIN);
*/

	binarize(img, 15);
	save_to_file("bin.pgm", img, IMAGE_DATA_MAIN);
	thin(img);
	save_to_file("thin.pgm", img, IMAGE_DATA_MAIN);

	free_image(img);

/*
	wiener_filter(img, 3);
	save_to_file("wiener.pgm", img, IMAGE_DATA_MAIN);



	save_to_file("clahe.pgm", img, IMAGE_DATA_MAIN);

	binarize(img, 13);
	save_to_file("bin.pgm", img, IMAGE_DATA_MAIN);
*/
/*
	gradient(img, KERNEL_SOBEL3x3);
	save_to_file("grad.pgm", img, IMAGE_DATA_GRADIENT);
	remove_background(img, 10, 30);
	save_to_file("rmbg.pgm", img, IMAGE_DATA_MAIN);
	ridge_orientation(img, 6);
	save_to_file("lro.pgm", img, IMAGE_DATA_LRO);
	binarize(img);
	save_to_file("bin.pgm", img, IMAGE_DATA_MAIN);
	thin(img);
	save_to_file("thin.pgm", img, IMAGE_DATA_MAIN);


	free_image(img);
*/
	return 0;
}
