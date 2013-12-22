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

	int r = lro->segment_size/2;
	int s = lro->segment_size;

	int lx[s], ly[s];

	int w = lro->width;
	int h = lro->height;

	printf("%d %d %d %d\n", w, h, r, s);

	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			if (lro->rate[i*lro->width + j] > 1000)
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
	}

	save_to_file(file_name, tmp);
	free_image(tmp);

	return 0;
}



int lro_gradient(mat* grad, mat* grad_dir, int seg_size, int dir_count, ridge_orientation** lro)
{
	ridge_orientation* _lro = malloc(sizeof(ridge_orientation));
	_lro->segment_size = seg_size;
	_lro->height = grad->height / seg_size;
	_lro->width = grad->width / seg_size;
	_lro->dir = malloc(_lro->width*_lro->height*sizeof(int));
	_lro->rate = malloc(_lro->width*_lro->height*sizeof(int));

	for (int i=0; i<_lro->height; i++)
	{
		for (int j=0; j<_lro->width; j++)
		{
			_lro->rate[i*_lro->width + j] = 0;
			_lro->dir[i*_lro->width + j] = 0;
			for (int y=i*seg_size; y<(i+1)*seg_size; y++)
				for (int x=j*seg_size; x<(j+1)*seg_size; x++)
				{
					_lro->rate[i*_lro->width + j] += grad->data[y*grad->width + x];
					_lro->dir[i*_lro->width + j] += grad_dir->data[y*grad_dir->width + x];
				}

			_lro->dir[i*_lro->width + j] /= seg_size*seg_size;
			//printf("%d ", _lro->dir[i*_lro->width + j]);
		}
		//printf("\n");
	}

	*lro = _lro;

	return 0;
}
