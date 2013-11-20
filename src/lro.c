/*
 * lro.c
 *
 *  Created on: Nov 3, 2013
 *      Author: arash
 */

#include "image.h"
#include "lro.h"
#include "process.h"
#include "mathfuncs.h"
#include "config.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

int save_orientation_image(char* file_name, ridge_orientation* lro)
{
	mat* tmp = new_image(lro->width*lro->segment_size, lro->height*lro->segment_size, 255);

	int r = lro->segment_size/2 - 1;
	int s = r*2 + 1;

	int lx[s], ly[s];

	int w = lro->width;
	int h = lro->height;

	printf("%d %d %d %d\n", w, h, r, s);

	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			int cx = j*s + r;
			int cy = i*s + r;

			int ang = lro->dir[i*w + j];
			printf("%d\n", ang);
			get_line(cx, cy, r, ang, lx, ly);

			for (int p=0; p<s; p++)
			{
				printf("%d %d %d %d\n", tmp->width, p, ly[p], lx[p]);
				tmp->data[ly[p]*tmp->width + lx[p]] = 255;
			}
		}
	}

	save_to_file(file_name, tmp);
	free_image(tmp);

	return 0;
}



static int lro_ratha(mat* img, ridge_orientation* lro)
{
	mat *gx, *gy;
	gradient_x(img, &gx, KERNEL_SOBEL3x3);
	gradient_y(img, &gy, KERNEL_SOBEL3x3);

	for (int y=0; y<img->height; y+=lro_segment_size)
	{
		for (int x=0; x<img->width; x+=lro_segment_size)
		{
			double num = 0, den = 0;
			for (int i=y; i<y+lro_segment_size; i++)
				for (int j=x; j<x+lro_segment_size; j++)
				{
					num += 2*gx->data[i*img->width + j]*gy->data[i*img->width + j];
					den += (gx->data[i*img->width + j]*gx->data[i*img->width + j] - gy->data[i*img->width + j]*gy->data[i*img->width + j]);
				}
			double teta = 0;
			if (den != 0)
				teta = 0.5*atan(num / den);
			int rad = teta*180 / PI + 90;
			if (rad > 180)
				rad -= 180;
			lro->dir[y/lro_segment_size * lro->width + x/lro_segment_size] = rad;
			//printf("%d ", rad);
		}
		//printf("\n");
	}

	free_image(gx);
	free_image(gy);

	return 0;
}

static int lro_grad(mat* img, ridge_orientation* lro)
{
	mat *g_x, *g_y;

	gradient_x(img, &g_x, KERNEL_SOBEL3x3);
	gradient_y(img, &g_y, KERNEL_SOBEL3x3);

	for (int i=0; i<lro->height; i++)
	{
		for (int j=0; j<lro->width; j++)
		{
			int Gx = 0, Gy = 0;
			for (int y=i*lro_segment_size; y<(i+1)*lro_segment_size; y++)
				for (int x=j*lro_segment_size; x<(j+1)*lro_segment_size; x++)
				{
					Gx += g_x->data[y*img->width + x];
					Gy += g_y->data[y*img->width + x];
				}
			lro->rate[i*lro->width + j] = sqrt(Gx*Gx + Gy*Gy);
			lro->dir[i*lro->width + j] = 0;
			if (Gy != 0)
				lro->dir[i*lro->width + j] = atan((double)Gy/(double)Gx)*180.0 / PI;
		}
	}
	return 0;
}

int get_ridge_orientation(mat* img, ridge_orientation** lro, int method)
{
	ridge_orientation* _lro = malloc(sizeof(ridge_orientation));
	_lro->segment_size = lro_segment_size;
	_lro->height = img->height / lro_segment_size;
	_lro->width = img->width / lro_segment_size;
	_lro->dir = malloc(_lro->width*_lro->height*sizeof(int));
	_lro->rate = malloc(_lro->width*_lro->height*sizeof(int));

	switch (method)
	{
		case LRO_RATHA:
			lro_ratha(img, _lro);
		break;
		case LRO_GRAD:
			lro_grad(img, _lro);
		break;
	}

	*lro = _lro;

	save_orientation_image("lro.pgm", _lro);

	return 0;
}
