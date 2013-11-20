/*
 * fingerprint.c
 *
 *  Created on: Nov 6, 2013
 *      Author: arash
 */

#include "fingerprint.h"

#include <stdlib.h>
#include <stdio.h>

void ROI(mat* img, mat* grad, int size)
{
	for (int i=0; i<img->height/size; i++)
	{
		for (int j=0; j<img->width/size; j++)
		{
			int sum = 0;
			for (int y=i*size; y<(i+1)*size; y++)
				for (int x=j*size; x<(j+1)*size; x++)
					sum += grad->data[y*img->width + x];
			printf("%d ", sum);
			if (sum < 12000)
			{
				for (int y=i*size; y<(i+1)*size; y++)
					for (int x=j*size; x<(j+1)*size; x++)
						img->data[y*img->width + x] = 0;
			}
		}
		printf("\n");
	}
}

void thin(mat* img)
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

}

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
		{5, 3, 15, 1, d0},
		{5, 6, 30, 1, d1},
		{7, 5, 35, 1, d2},
		{5, 5, 25, 1, d3},
		{3, 5, 15, 1, d4},
		{3, 7, 21, 1, d5},
		{7, 5, 35, 1, d6},
		{7, 3, 21, 1, d7}
};

void DMF(mat* img, ridge_orientation* lro)
{
	int *d = malloc(img->len * sizeof(int));


	for (int i=0; i<img->height; i++)
		for (int j=0; j<img->width; j++)
		{
			mat* f;
			int dir = lro->dir[i/(lro->segment_size)*lro->width + j/(lro->segment_size)];
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

void LDMF(mat* img, ridge_orientation* lro, int radius)
{
	int x[radius*2 + 1],y[radius*2 + 1];
	int *d = malloc(img->len*sizeof(int));

	for (int i=0; i<img->height; i++)
		for (int j=0; j<img->width; j++)
		{
			int a = lro->dir[i/(lro->segment_size)*lro->width + j/(lro->segment_size)];
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

void DGF(mat* img)
{
	/* Method Stub */
	/* TODO: implement Directional Gabor Filter */
}
