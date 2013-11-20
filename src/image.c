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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

mat* new_image(int w, int h, int m)
{
	mat* img = malloc(sizeof(mat));
	img->width = w;
	img->height = h;
	img->len = w*h;
	img->max_val = m;

	img->data = calloc(w*h, sizeof(int));

	return img;
}

mat* clone_image(mat* img)
{
	mat* out = malloc(sizeof(mat));
	out->width = img->width;
	out->height = img->height;
	out->len = img->len;
	out->max_val = img->max_val;

	out->data = calloc(img->width*img->height, sizeof(int));

	return out;
}

void free_image(mat* img)
{
	free(img->data);

	free(img);
}


int load_from_file(const char* file_name, mat** img)
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

int save_to_file(const char* file_name, mat* img)
{
	FILE* f = fopen(file_name, "w+");
	if (f == NULL)
		return 1;

	fprintf(f, "P5 %d %d %d\n", img->width, img->height, img->max_val);

	unsigned char buff[10000];
	int pos,p=0;
	for (int i=0; i<img->len/10000; i++)
	{
		pos=0;
		for (int j=0; j<10000; j++)
			buff[pos++] = img->data[p++];
		fwrite(buff, 1, 10000, f);
	}
	pos=0;
	for (int j=0; j<img->len%10000; j++)
		buff[pos++] = img->data[p++];
	fwrite(buff, 1, img->len%10000, f);

	fclose(f);

	return 0;
}

int resize(mat* img, int width, int height)
{
	int* data = malloc(width*height*sizeof(int));

	for (int i=0; i<height; i++)
		for (int j=0; j<width; j++)
		{
			int x = j*img->width/width;
			int y = i*img->height/height;

			data[width*i + j] = img->data[y*img->width + x];
		}

	free(img->data);

	img->width = width;
	img->height = height;
	img->len = width*height;
	img->data = data;

	return 0;
}
