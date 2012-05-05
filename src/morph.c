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

#include <stdlib.h>
#include <string.h>

#include "morph.h"

static int disk5[] = {
	0, 0, 1, 1, 1, 1, 1, 0, 0,
	0, 1, 1, 1, 1, 1, 1, 1, 0,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1,
	0, 1, 1, 1, 1, 1, 1, 1, 0,
	0, 0, 1, 1, 1, 1, 1, 0, 0
};

void strel(int el, int size, mat** se)
{
	mat* res = malloc(sizeof(mat));
	res->data = malloc(81 * sizeof(int));
	res->width = 9;
	res->height = 9;
	memcpy(res->data, disk5, 81*sizeof(int));
	*se = res;
}


void dilate(mat* img, mat* se)
{
	int cx = se->width/2;
	int cy = se->height/2;

	int *data = malloc(img->width*img->height * sizeof(int));

	for (int i=0; i<img->height; i++)
		for (int j=0; j<img->width; j++)
		{
			int max = 0;
			for (int k=0; k<se->height; k++)
				for (int l=0; l<se->width; l++)
				{
					int x = j - (l-cx);
					int y = i - (k-cy);
					if (x<img->width && x>=0 && y<img->height && y>=0)
					{
						if (img->data[y*img->width + x]*se->data[k*se->width+l] > max)
							max = img->data[y*img->width + x];
					}
				}
			data[i*img->width + j] = max;
		}
	memcpy(img->data, data, img->width*img->height*sizeof(int));
	free(data);
}

void erode(mat* img, mat* se)
{
	int cx = se->width/2;
	int cy = se->height/2;

	int *data = malloc(img->width*img->height * sizeof(int));

	for (int i=0; i<img->height; i++)
		for (int j=0; j<img->width; j++)
		{
			int min = 255;
			for (int k=0; k<se->height; k++)
				for (int l=0; l<se->width; l++)
				{
					int x = j - (l-cx);
					int y = i - (k-cy);
					if (x<img->width && x>=0 && y<img->height && y>=0)
					{
						if (img->data[y*img->width + x]*se->data[k*se->width+l]>0 && img->data[y*img->width + x]*se->data[k*se->width+l]<min)
							min = img->data[y*img->width + x];
					}
				}
			data[i*img->width + j] = min;
		}
	memcpy(img->data, data, img->width*img->height*sizeof(int));
	free(data);
}
