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

	//printf("%d %d %d %d\n", w, h, r, s);

	for (int i=0; i<h; i++)
	{
		for (int j=0; j<w; j++)
		{
			//if (lro->rate[i*w + j] < 20)
			{
				int cx = j*s + r;
				int cy = i*s + r;

				int ang = lro->dir[i*w + j];
				//printf("%d\n", ang);
				get_line(cx, cy, r, ang, lx, ly);

				for (int p=0; p<s; p++)
				{
					//printf("%d %d %d %d\n", tmp->width, p, ly[p], lx[p]);
					tmp->data[ly[p]*tmp->width + lx[p]] = 255;
				}
			}
		}
	}

	save_to_file(file_name, tmp);
	free_image(tmp);

	return 0;
}

static int angle_mean(int a1, int a2)
{
	int diff = abs(a1-a2);
	if (diff <= 90)
		return (a1+a2) / 2;
	else
	{
		int x = (a1+a2+180) / 2;
		if (x >= 180)
			x -= 180;
		return x;
	}
}


int lro_gradient(mat* img, int seg_size, int dir_count, ridge_orientation** lro)
{
	mat *g_x, *g_y;
	convolve(img, &g_x, KERNEL_SOBEL3x3, DIRECTION_HORIZONTAL);
	convolve(img, &g_y, KERNEL_SOBEL3x3, DIRECTION_VERTICAL);

	ridge_orientation* _lro = malloc(sizeof(ridge_orientation));
	_lro->segment_size = seg_size;
	_lro->height = img->height / seg_size;
	_lro->width = img->width / seg_size;
	_lro->dir = malloc(_lro->width*_lro->height*sizeof(int));
	_lro->rate = malloc(_lro->width*_lro->height*sizeof(int));

	for (int i=0; i<_lro->height; i++)
	{
		for (int j=0; j<_lro->width; j++)
		{
			double num = 0, den = 0;
			for (int y=i*seg_size; y<(i+1)*seg_size; y++)
				for (int x=j*seg_size; x<(j+1)*seg_size; x++)
				{
					int pos = y*img->width + x;
					num += g_x->data[pos]*g_x->data[pos] - g_y->data[pos]*g_y->data[pos];
					den += 2*g_x->data[pos]*g_y->data[pos];
				}

			_lro->dir[i*_lro->width + j] = 0.5 * (atan2(num, den) + PI) * 180.0 / PI;
			//printf("%d ", _lro->dir[i*_lro->width + j]);
		}
		//printf("\n");
	}

	*lro = _lro;

	return 0;
}


int lro_stdev(mat* img, int seg_size, int dir_count, ridge_orientation** lro)
{

	ridge_orientation* _lro = malloc(sizeof(ridge_orientation));
	_lro->segment_size = seg_size;
	_lro->height = img->height / seg_size;
	_lro->width = img->width / seg_size;
	_lro->dir = malloc(_lro->width*_lro->height*sizeof(int));
	int* dir_tmp = malloc(_lro->width*_lro->height*sizeof(int));
	_lro->rate = malloc(_lro->width*_lro->height*sizeof(int));

	int s = seg_size;
	int r = seg_size/2;

	int angel_min = 180/dir_count;

	for (int i=0; i<_lro->height; i++)
	{
		for (int j=0; j<_lro->width; j++)
		{
			int cx = j*s + r;
			int cy = i*s + r;

			int lx[s], ly[s];

			int min = 128*seg_size;
			int max = 0;
			int a_min = 0, a_max = 0;
			for (int i=0; i<dir_count; i++)
			{
				int angel = angel_min * i;
				get_line(cx, cy, r, angel, lx, ly);
				int mean = 0;
				for (int k=0; k<s; k++)
					mean += img->data[ly[k]*img->width + lx[k]];
				mean /= s;
				int stdev = 0;
				for (int k=0; k<s; k++)
					stdev += (img->data[ly[k]*img->width + lx[k]] - mean)*(img->data[ly[k]*img->width + lx[k]] - mean);
				stdev = sqrt(stdev/s);
				if (stdev < min)
				{
					min = stdev;
					a_min = angel;
				}
				if (stdev > max)
				{
					max = stdev;
					a_max = angel;
				}
			}
			_lro->dir[i*_lro->width + j] = a_min;// > 90?a_max-90:90-a_max;
			_lro->rate[i*_lro->width + j] = min;
			//printf("%d ", min);
		}
		//printf("\n");
	}

/*
	for (int k=0; k<10; k++)
		for (int i=1; i<_lro->height-1; i++)
		{
			for (int j=1; j<_lro->width-1; j++)
			{
				if (abs(_lro->dir[(i-1)*_lro->width + j] - _lro->dir[(i+1)*_lro->width + j]) <= 2*angel_min)
					_lro->dir[i*_lro->width + j] = (_lro->dir[(i-1)*_lro->width + j] + _lro->dir[(i+1)*_lro->width + j]) / 2;
				else if (abs(_lro->dir[(i)*_lro->width + j-1] - _lro->dir[(i)*_lro->width + j+1]) <= 2*angel_min)
					_lro->dir[i*_lro->width + j] = (_lro->dir[(i)*_lro->width + j-1] + _lro->dir[(i)*_lro->width + j+1]) / 2;
			}
		}
*/
/*
	for (int i=1; i<_lro->height-1; i++)
	{
		for (int j=1; j<_lro->width-1; j++)
		{
			_lro->dir[i*_lro->width + j] = angle_mean(
					angle_mean(dir_tmp[(i-1)*_lro->width + j], dir_tmp[(i+1)*_lro->width + j]),
					angle_mean(dir_tmp[i*_lro->width + j-1], dir_tmp[i*_lro->width + j+1])
					);
		}
	}
*/

	for (int k=0; k<3; k++)
	for (int i=1; i<_lro->height-1; i++)
	{
		for (int j=1; j<_lro->width-1; j++)
		{
			_lro->dir[i*_lro->width + j] = angle_mean(
					angle_mean(_lro->dir[(i-1)*_lro->width + j], _lro->dir[(i+1)*_lro->width + j]),
					angle_mean(_lro->dir[i*_lro->width + j-1], _lro->dir[i*_lro->width + j+1])
					);
		}
	}

	*lro = _lro;

	return 0;
}

int lro_stdev2(mat* img, int seg_size, int dir_count, ridge_orientation** lro)
{

	ridge_orientation* _lro = malloc(sizeof(ridge_orientation));
	_lro->segment_size = seg_size;
	_lro->height = img->height / seg_size;
	_lro->width = img->width / seg_size;
	_lro->dir = malloc(_lro->width*_lro->height*sizeof(int));
	_lro->rate = malloc(_lro->width*_lro->height*sizeof(int));

	int s = seg_size;
	int r = seg_size/2;

	int angel_min = 180/dir_count;

	for (int i=0; i<_lro->height; i++)
	{
		for (int j=0; j<_lro->width; j++)
		{
			int cx = j*s + r;
			int cy = i*s + r;

			int lx[s], ly[s];

			int stdevs[dir_count];
			for (int i=0; i<dir_count; i++)
			{
				int angel = angel_min * i;
				get_line(cx, cy, r, angel, lx, ly);
				int mean = 0;
				for (int k=0; k<s; k++)
					mean += img->data[ly[k]*img->width + lx[k]];
				mean /= s;
				int stdev = 0;
				for (int k=0; k<s; k++)
					stdev += (img->data[ly[k]*img->width + lx[k]] - mean)*(img->data[ly[k]*img->width + lx[k]] - mean);
				stdev = sqrt(stdev/s);

				stdevs[i] = stdev;
			}

			int diff_max = 0;
			int a = 0;
			for (int i=0; i<dir_count; i++)
			{
				int angel = angel_min * i;
				int diff = abs(stdevs[i]-stdevs[(i+dir_count/2)%dir_count]);
				if (diff > diff_max)
				{
					diff_max = diff;
					a = angel;
				}
			}
			_lro->dir[i*_lro->width + j] = a;// > 90?a_max-90:90-a_max;
			_lro->rate[i*_lro->width + j] = diff_max;
			//printf("%d ", min);
		}
		//printf("\n");
	}

	*lro = _lro;

	return 0;
}
