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

#include "config.h"
#include "directional.h"
#include "process.h"
#include "linear.h"

#include <stdlib.h>
#include <string.h>

static int d0[] = {
		1, 0, 1, 0, 1,
		1, 0, 1, 0, 1,
		1, 0, 1, 0, 1
};

static int d1[] = {
		1, 0, 0, 0, 0,
		1, 0, 1, 0, 0,
		1, 0, 1, 0, 0,
		0, 0, 1, 0, 1,
		0, 0, 0, 0, 1,
		0, 0, 0, 0, 1
};

static int d2[] = {
		1, 1, 1, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 1, 1, 1
};

static int d3[] = {
		1, 1, 1, 0, 0,
		0, 0, 0, 0, 0,
		0, 1, 1, 1, 0,
		0, 0, 0, 0, 0,
		0, 0, 1, 1, 1
};

static int d4[] = {
		1, 1, 1,
		0, 0, 0,
		1, 1, 1,
		0, 0, 0,
		1, 1, 1
};

static int d5[] = {
		0, 0, 1,
		0, 0, 1,
		0, 1, 1,
		0, 1, 0,
		1, 1, 0,
		1, 0, 0,
		1, 0, 0
};

static int d6[] = {
		0, 0, 0, 0, 1, 1, 1,
		0, 0, 0, 0, 0, 0, 0,
		0, 0, 1, 1, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		1, 1, 1, 0, 0, 0, 0
};

static int d7[] = {
		0, 0, 0, 0, 1, 1, 1,
		0, 0, 1, 1, 1, 0, 0,
		1, 1, 1, 0, 0, 0, 0
};

static mat df[] = {
		{5, 3, d0},
		{5, 6, d1},
		{7, 5, d2},
		{5, 5, d3},
		{3, 5, d4},
		{3, 7, d5},
		{7, 5, d6},
		{7, 3, d7}
};

void DMF(image* img)
{
	int *d = malloc(img->len * sizeof(int));

	if (!(img->flags & IMAGE_FLAG_HAS_LRO))
		ridge_orientation(img, lro_segment_size);

	for (int i=0; i<img->height; i++)
		for (int j=0; j<img->width; j++)
		{
			mat* f;
			int dir = img->lro_dir.data[i/(img->lro_radius*2+1)*img->lro_dir.width + j/(img->lro_radius*2+1)];
			if (dir < 11.25) f = &df[0];
			else if (dir < 33.75) f = &df[1];
			else if (dir < 56.25) f = &df[2];
			else if (dir < 78.75) f = &df[3];
			else if (dir < 101.25) f = &df[4];
			else if (dir < 123.75) f = &df[5];
			else if (dir < 146.25) f = &df[6];
			else if (dir < 168.75) f = &df[7];
			else f = &df[0];

			int sum=0, div=0;
			int cx = f->width / 2;
			int cy = f->height / 2;
			for (int k=-cy; k<=cy; k++)
				for (int l=-cx; l<=cx; l++)
				{
					if (i+k>=0 && i+k<img->height && j+l>=0 && j+l<img->width)
					{
						sum += img->data[(i+k)*img->width + (j+l)]*f->data[(k+cy)*f->width + (l+cx)];
						div += 1*f->data[(k+cy)*f->width + (l+cx)];
					}
				}
			sum /= div;
			d[i*img->width + j] = sum;
		}

	memcpy(img->data, d, img->len*sizeof(int));

	free(d);
}

void LDMF(image* img, int radius)
{
	int x[radius*2 + 1],y[radius*2 + 1];
	int *d = malloc(img->len*sizeof(int));

	for (int i=0; i<img->height; i++)
		for (int j=0; j<img->width; j++)
		{
			int a = img->lro_dir.data[i/(img->lro_radius*2+1)*img->lro_dir.width + j/(img->lro_radius*2+1)];
			get_line(j, i, radius, a, x, y);
			int sum = 0;
			int div = 0;
			for (int k=0; k<radius*2+1; k++)
			{
				if (x[k]>=0 && x[k]<img->width && y[k]>=0 && y[k]<img->height)
				{
					sum += img->data[y[k]*img->width + x[k]];
					div++;
				}
			}
			d[i*img->width+j] = sum/div;
		}

	memcpy(img->data, d, img->len*sizeof(int));

	free(d);
}

void DGF(image* img)
{
	/* Method Stub */
	/* TODO: implement Directional Gabor Filter */
}
