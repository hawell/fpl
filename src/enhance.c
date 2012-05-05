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

#include "enhance.h"
#include "filter.h"
#include "config.h"
#include "morph.h"
#include "linear.h"
#include "clahe.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>

void normalize(mat* m, int min_val, int max_val)
{
	int max=min_val, min=max_val;
	int len = m->width*m->height;

	for (int i=0; i<len; i++)
	{
		if (m->data[i] > max) max = m->data[i];
		if (m->data[i] < min) min = m->data[i];
	}

	if (max!=max_val || min!=min_val)
	{
		max -= min;
		for (int i=0; i<len; i++)
		{
			m->data[i] -= min;
			m->data[i] = (m->data[i]*max_val) / max;
		}
	}
}

void histogram_equalization(image* img)
{
	int p[256];
	for (int i=0; i<256; i++) p[i] = 0;
	for (int i=0; i<img->len; i++)
		p[img->data[i]]++;

	int cdf[256];
	cdf[0] = p[0];
	for (int i=1; i<256; i++)
		cdf[i] = cdf[i-1] + p[i];

	int min=0;
	while (cdf[min] == 0) min++;

	int h[256];
	for (int i=0; i<256; i++)
	{
		if (cdf[i])
			h[i] = ((float)(cdf[i] - cdf[min]) / (float)(img->len - cdf[min]))*255 + 0.5;
		else
			h[i] = 0;
	}

	for (int i=0; i<img->len; i++)
		img->data[i] = h[img->data[i]];

	img->state = IMAGE_STATE_RAW_ENHANCED;
}

void clahe(image* img, int x_count, int y_count, float clip_limit)
{
	unsigned char *tmp = malloc(img->width*img->height);
	for (int i=0; i<img->len; i++)
		tmp[i] = img->data[i];

	CLAHE(tmp, img->width, img->height, 0, img->max_val, x_count, y_count, clahe_bin_count, clip_limit);
	for (int i=0; i<img->len; i++)
		img->data[i] = tmp[i];
	free(tmp);
}

void normalization_enhance(image* img, int m0, int v0)
{
	int m=0,v;

	for (int i=0; i<img->len; i++)
		m += img->data[i];
	m /= img->len;


}

float noise_variance(image* img)
{
	mat *lap;
	filter(img, &lap, KERNEL_LAPLACIAN3x3, DIRECTION_NONE);

	float sum=0;
	for (int i=0; i<img->len; i++)
		sum += abs(lap->data[i]);

	float stdev = sqrt(PI/2)*sum/(float)(6*img->width*img->height);

	return stdev*stdev;
}

void wiener_filter(image* img, int size)
{
	float nv = 0.04;//noise_variance(img);

	int* w = calloc(img->len, sizeof(int));

	for (int i=1; i<img->height-1; i++)
		for (int j=1; j<img->width-1; j++)
		{
			float m=0;
			for (int k=-1; k<=1; k++)
				for (int l=-1; l<=1; l++)
					m += img->data[(i+k)*img->width + (j+l)];
			m /= 9.0;


			float v=0;
			for (int k=-1; k<=1; k++)
				for (int l=-1; l<=1; l++)
					v += img->data[(i+k)*img->width + (j+l)]*img->data[(i+k)*img->width + (j+l)] - m*m;
			v /= 9.0;

			w[i*img->width + j] = m + ((v - nv)/v)*(img->data[i*img->width+j] - m);
		}

	for (int i=0; i<img->len; i++)
		img->data[i] = w[i];
}

void remove_block(image* img, int x, int y, int size, int threshold)
{
	if (x<0 || y<0 || x>img->width-size || y>img->height-size)
		return;

	int m = 0;
	for (int i=y; i<y+size; i++)
		for (int j=x; j<x+size; j++)
			m += img->data[i*img->width + j];

	if (m == 0)
		return;

	m /= size*size;

	if (m < threshold)
	{
		for (int i=y; i<y+size; i++)
			for (int j=x; j<x+size; j++)
				img->data[i*img->width + j] = 0;
		remove_block(img, x+size, y, size, threshold);
		remove_block(img, x-size, y, size, threshold);
		remove_block(img, x, y+size, size, threshold);
		remove_block(img, x, y-size, size, threshold);
	}
}

void remove_background(image* img, int size)
{
	mat m;
	m.width = img->width;
	m.height = img->height;
	m.data = malloc(img->len*sizeof(int));
	memcpy(m.data, img->data, img->len*sizeof(int));

	mat *se;
	strel(rmbg_strel_type, rmbg_strel_size, &se);

	for (int i=0; i<erode_dilate_count; i++)
	{
		erode(&m, se);
	}
	save_data("1.pgm", m.data, m.width, m.height, 255);

	for (int i=0; i<erode_dilate_count; i++)
	{
		dilate(&m, se);
	}
	save_data("2.pgm", m.data, m.width, m.height, 255);

	for (int i=0; i<img->len; i++)
	{
		img->data[i] -= m.data[i];
		if (img->data[i] < 0)
			img->data[i] = 0;
	}
}
