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
#include "linear.h"
#include "filter.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

image* new_image(int w, int h, int m)
{
	image* img = malloc(sizeof(image));
	img->width = w;
	img->height = h;
	img->len = w*h;
	img->max_val = m;

	img->state = IMAGE_STATE_RAW;
	img->flags = 0;

	img->data = calloc(w*h, sizeof(int));

	return img;
}

image* clone_image(image* img)
{
	image* out = malloc(sizeof(image));
	out->width = img->width;
	out->height = img->height;
	out->len = img->len;
	out->max_val = img->max_val;

	out->state = IMAGE_STATE_RAW;
	out->flags = 0;

	out->data = calloc(img->width*img->height, sizeof(int));

	return out;
}

void free_image(image* img)
{
	free(img->data);

	if (img->flags & IMAGE_FLAG_HAS_GRAD)
		free(img->grad.data);

	if (img->flags & IMAGE_FLAG_HAS_LRO)
	{
		free(img->lro_dir.data);
		free(img->lro_rate.data);
	}

	if (img->flags & IMAGE_FLAG_HAS_LRF)
		free(img->lrf.data);

	free(img);
}


int load_from_file(char* file_name, image** img)
{
	FILE* f = fopen(file_name, "r");
	if (f == NULL)
		return 1;

	char tmp[20];
	fscanf(f, "%s", tmp);
	if (strcmp(tmp, "P5") != 0)
	{
		fclose(f);
		return 1;
	}

	int w,h,m;
	if (fscanf(f, "%d %d %d\n", &w, &h, &m) != 3)
	{
		fclose(f);
		return 1;
	}

	*img = new_image(w, h, m);

	unsigned char buff[10000];
	int count,pos=0;
	while ((count = fread(buff, 1, w, f)) > 0)
	{
		if (m < 256)
		{
			unsigned char* p = buff;
			for (int i=0; i<count; i++)
				(*img)->data[pos++] = *p++;
		}
		else
		{
			unsigned short* p = (unsigned short*)buff;
			for (int i=0; i<count/2; i++)
				(*img)->data[pos++] = *p++;
		}
	}

	fclose(f);
	return 0;
}

int save_data(char* file_name, int* data, int w, int h, int max_val)
{
	FILE* f = fopen(file_name, "w");
	if (f == NULL)
		return 1;

	int len = w*h;

	fprintf(f, "P5 %d %d %d\n", w, h, max_val);

	unsigned char buff[10000];
	int pos,p=0;
	for (int i=0; i<len/10000; i++)
	{
		pos=0;
		for (int j=0; j<10000; j++)
			buff[pos++] = data[p++];
		fwrite(buff, 1, 10000, f);
	}
	pos=0;
	for (int j=0; j<len%10000; j++)
		buff[pos++] = data[p++];
	fwrite(buff, 1, len%10000, f);

	fclose(f);

	return 0;

}

int save_orientation_image(char* file_name, image* img)
{
	image* tmp = clone_image(img);

	int r = img->lro_radius;
	int s = r*2 + 1;

	int lx[s], ly[s];

	int w = img->lro_dir.width;
	int h = img->lro_dir.height;

	printf("%d %d %d %d\n", w, h, r, s);

	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			int cx = j*s + r;
			int cy = i*s + r;

			int ang = img->lro_dir.data[i*w + j];
			printf("%d\n", ang);
			get_line(cx, cy, r, ang, lx, ly);

			for (int p=0; p<s; p++)
			{
				printf("%d %d %d %d\n", tmp->width, p, ly[p], lx[p]);
				tmp->data[ly[p]*tmp->width + lx[p]] = 255;
			}
		}
	}

	save_data(file_name, tmp->data, tmp->width, tmp->height,  tmp->max_val);
	free_image(tmp);

	return 0;
}

int save_to_file(char* file_name, image* img, int img_data)
{
	switch (img_data)
	{
		case IMAGE_DATA_MAIN:
			return save_data(file_name, img->data, img->width, img->height, img->max_val);
		break;

		case IMAGE_DATA_GRADIENT:
			if (img->flags & IMAGE_FLAG_HAS_GRAD)
				return save_data(file_name, img->grad.data, img->width, img->height, img->max_val);
		break;

		case IMAGE_DATA_LRO:
			if (img->flags & IMAGE_FLAG_HAS_LRO)
				return save_orientation_image(file_name, img);
		break;

		default:
			return -1;
	}

	return -1;
}
