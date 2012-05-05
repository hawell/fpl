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

#include "process.h"
#include "filter.h"
#include "linear.h"
#include "config.h"
#include "morph.h"
#include "enhance.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

#define INF		2000000000

void gradient(image* img, int kernel)
{
	mat *grad_x, *grad_y;

	if (!(img->flags & IMAGE_FLAG_HAS_GRAD))
		img->grad.data = malloc(img->width*img->height*sizeof(int));
	img->grad.width = img->width;
	img->grad.height = img->height;

	filter(img, &grad_x, kernel, DIRECTION_HORIZONTAL);
	filter(img, &grad_y, kernel, DIRECTION_VERTICAL);

	for (int i=0; i<img->width*img->height; i++)
		img->grad.data[i] = sqrt(grad_x->data[i]*grad_x->data[i] + grad_y->data[i]*grad_y->data[i]);
	normalize(&img->grad, 0, img->max_val);

	img->flags |= IMAGE_FLAG_HAS_GRAD;
}

int ridge_dir(mat* m, int cx, int cy, int radius, int *a)
{
	int lx[radius*2+1], ly[radius*2+1];

	int stdev[180];

	for (int a=0; a<180; a++)
	{
		stdev[a] = 0;
		get_line(cx, cy, radius, a, lx, ly);

		float mean = 0;
		for (int i=0; i<2*radius+1; i++)
		{
			mean += m->data[ly[i]*m->width + lx[i]];
		}
		mean /= 2*radius+1;
		for (int i=0; i<2*radius+1; i++)
			stdev[a] += (m->data[ly[i]*m->width + lx[i]] - mean)*(m->data[ly[i]*m->width + lx[i]] - mean);
		stdev[a] /= 2*radius+1;
	}

	int amin=0, amax=0, min=INF, max=0;

	for (int i=0; i<180; i++)
	{
		if (stdev[i] < min)
		{
			min = stdev[i];
			amin = i;
		}
		if (stdev[i] > max)
		{
			max = stdev[i];
			amax = i;
		}
	}

	*a = amin;

	return abs(amin - amax);
}

int angle_mean(int a, int b)
{
	if (a > b)
	{
		int t = a;
		a = b;
		b = t;
	}

	if (b - a >= 90)
		return ((a+b+180)/2) % 180;
	else
		return (a+b) / 2;
}

void regularize(int* d, int w, int h, int max_diff)
{
	for (int i=1; i<h-1; i++)
		for (int j=0; j<w; j++)
		{
			int mean = angle_mean(d[(i-1)*w + j], d[(i+1)*w + j]);
			if ((d[i*w+j]-d[(i-1)*w+j]>max_diff || d[i*w+j]-d[(i-1)*w+j]<-max_diff) &&
				(d[i*w+j]-d[(i+1)*w+j]>max_diff || d[i*w+j]-d[(i+1)*w+j]<-max_diff)
				)
				d[i*w+j] = mean;

		}

	for (int i=0; i<h; i++)
		for (int j=1; j<w-1; j++)
		{
			int mean = angle_mean(d[i*w + j-1], d[i*w + j+1]);
			if ((d[i*w+j]-d[i*w+j-1]>max_diff || d[i*w+j]-d[i*w+j-1]<-max_diff) &&
				(d[i*w+j]-d[i*w+j+1]>max_diff || d[i*w+j]-d[i*w+j+1]<-max_diff)
				)
				d[i*w+j] = mean;
		}

	for (int i=0; i<h; i++)
		for (int j=1; j<w-1; j++)
		{
			int mean = angle_mean(d[i*w + j-1], d[i*w + j+1]);
			if (d[i*w + j] - mean > max_diff || d[i*w + j] - mean < -max_diff)
				d[i*w+j] = mean;
		}

	for (int i=1; i<h-1; i++)
		for (int j=0; j<w; j++)
		{
			int mean = angle_mean(d[(i-1)*w + j], d[(i+1)*w + j]);
			if (d[i*w + j] - mean > max_diff || d[i*w + j] - mean < -max_diff)
				d[i*w+j] = mean;
		}

}

void regularize2(int* d, int w, int h)
{
	for (int i=1; i<h-1; i++)
		for (int j=1; j<w-1; j++)
		{
			int d1,d2,d3,d4;
			d1 = angle_mean(d[(i-1)*w + j-1], d[(i-1)*w + j+1]);
			d2 = angle_mean(d[(i+1)*w + j-1], d[(i+1)*w + j+1]);
			d3 = angle_mean(d[(i-1)*w + j-1], d[(i+1)*w + j-1]);
			d4 = angle_mean(d[(i-1)*w + j+1], d[(i+1)*w + j+1]);

			d1 = angle_mean(d1, d2);
			d3 = angle_mean(d3, d4);

			d[i*w +j] = angle_mean(d1, d3);
		}
}

int ridge_orientation(image* img, int radius)
{
	int w = img->width/(2*radius+1);
	int h = img->height/(2*radius+1);

	if (!(img->flags & IMAGE_FLAG_HAS_GRAD))
		gradient(img, KERNEL_NRIGO7x5);

	if (img->flags & IMAGE_FLAG_HAS_LRO)
	{
		free(img->lro_dir.data);
		free(img->lro_rate.data);
	}

	img->lro_dir.data = malloc(w*h*sizeof(int));
	img->lro_dir.width = w;
	img->lro_dir.height = h;

	img->lro_rate.data = malloc(w*h*sizeof(int));
	img->lro_rate.width = w;
	img->lro_rate.height = h;

	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			int cx = j*(2*radius+1) + radius;
			int cy = i*(2*radius+1) + radius;

			int ang;
			img->lro_rate.data[i*w + j] = ridge_dir(&img->grad, cx, cy, radius, &ang);

			img->lro_dir.data[i*w + j] = ang;
		}
	}

	regularize2(img->lro_dir.data, w, h);
	regularize(img->lro_dir.data, w, h, 30);

	img->lro_radius = radius;

	img->flags |= IMAGE_FLAG_HAS_LRO;

	return 0;
}

get_background(image* img, int size, image** bg)
{

}

void binarize(image* img, int size)
{
	int nx = img->width%size?img->width/size+1:img->width/size;
	int ny = img->height%size?img->height/size+1:img->height/size;

	int lx,ly;
	for (int i=0; i<ny; i++)
	{
		ly = (i==ny-1 && img->height%size)?img->height%size:size;
		for (int j=0; j<nx; j++)
		{
			lx = (j==nx-1 && img->width%size)?img->width%size:size;
			int m=0;
			for (int k=0; k<ly; k++)
				for (int l=0; l<lx; l++)
					m += img->data[(i*size+k)*img->width + j*size+l];
			m /= lx*ly;

			for (int k=0; k<ly; k++)
				for (int l=0; l<lx; l++)
				{
					if (img->data[(i*size+k)*img->width + j*size+l] <= m)
						img->data[(i*size+k)*img->width + j*size+l] = 0;
					else
						img->data[(i*size+k)*img->width + j*size+l] = 1;
				}
		}
	}

	img->max_val = 1;

	img->state = IMAGE_STATE_BINARY;
}

void binarize2(image* img, int threshold)
{
	for (int i=0; i<img->len; i++)
		if (img->data[i]<threshold)
			img->data[i] = 0;
		else
			img->data[i] = 1;

	img->max_val = 1;
	img->state = IMAGE_STATE_BINARY;
}

void thin(image* img)
{
	int *m = calloc(img->len, sizeof(int));

	for (int i=0; i<img->width; i++)
	{
		img->data[i] = 0;
		img->data[img->width*(img->height-1) + i] = 0;
	}
	for (int i=0; i<img->height; i++)
	{
		img->data[img->width*i] = 0;
		img->data[img->width*(i+1) - 1] = 0;
	}

	int c = 0;
	while (1)
	{
		c = 0;
		for (int i=1; i<img->height; i++)
			for (int j=1; j<img->width; j++)
				if (img->data[i*img->width + j])
				{
					int p[9];
					p[0] = img->data[i*img->width + j];
					p[1] = img->data[(i-1)*img->width + j];
					p[2] = img->data[(i-1)*img->width + j+1];
					p[3] = img->data[i*img->width + j+1];
					p[4] = img->data[(i+1)*img->width + j+1];
					p[5] = img->data[(i+1)*img->width + j];
					p[6] = img->data[(i+1)*img->width + j-1];
					p[7] = img->data[i*img->width + j-1];
					p[8] = img->data[(i-1)*img->width + j-1];

					int A=0;
					for (int k=1; k<8; k++)
						if (p[k]==0 && p[k+1]==1) A++;
					if (p[8]==0 && p[1]==1) A++;
					int B=0;
					for (int k=1; k<9; k++)
						B += p[k];

					if (A==1 && B>=2 && B<=6 && p[1]*p[3]*p[5]==0 && p[3]*p[5]*p[7]==0)
					{
						c++;
						m[i*img->width + j] = 1;
					}
				}

		if (c == 0)
			break;

		for (int i=0; i<img->len; i++)
			if (m[i])
			{
				img->data[i] = 0;
				m[i] = 0;
			}

		c = 0;
		for (int i=1; i<img->height; i++)
			for (int j=1; j<img->width; j++)
				if (img->data[i*img->width + j])
				{
					int p[9];
					p[0] = img->data[i*img->width + j];
					p[1] = img->data[(i-1)*img->width + j];
					p[2] = img->data[(i-1)*img->width + j+1];
					p[3] = img->data[i*img->width + j+1];
					p[4] = img->data[(i+1)*img->width + j+1];
					p[5] = img->data[(i+1)*img->width + j];
					p[6] = img->data[(i+1)*img->width + j-1];
					p[7] = img->data[i*img->width + j-1];
					p[8] = img->data[(i-1)*img->width + j-1];

					int A=0;
					for (int k=1; k<8; k++)
						if (p[k]==0 && p[k+1]==1) A++;
					if (p[8]==0 && p[1]==1) A++;
					int B=0;
					for (int k=1; k<9; k++)
						B += p[k];

					if (A==1 && B>=2 && B<=6 && p[1]*p[3]*p[7]==0 && p[1]*p[5]*p[7]==0)
					{
						c++;
						m[i*img->width + j] = 1;
					}
				}

		if (c == 0)
			break;

		for (int i=0; i<img->len; i++)
			if (m[i])
			{
				img->data[i] = 0;
				m[i] = 0;
			}

	}

	free(m);

	img->state = IMAGE_STATE_THINNED;
}
