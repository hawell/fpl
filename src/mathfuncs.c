/*
 * mathfuncs.c
 *
 *  Created on: Nov 6, 2013
 *      Author: arash
 */

#include "mathfuncs.h"
#include "convolution.h"

#include <math.h>

#define MAX_FILTER_LINE_LEN	10
#define MAX_KERNEL_SIZE		100

typedef struct {
	float k[MAX_KERNEL_SIZE];
	int width, height;
} kernel_normal;

typedef struct {
	float column[MAX_FILTER_LINE_LEN];
	float row[MAX_FILTER_LINE_LEN];
	int column_count, row_count;
} kernel_seperable;

static kernel_seperable sobel3x3 = {
		{1, 2, 1},
		{-1, 0, 1},
		3,3
};

static kernel_seperable scharr3x3 = {
		{3, 10, 3},
		{-1, 0, 1},
		3,3
};

static kernel_seperable nrigo5x3 = {
		{1/4.0f, 2/4.0f, 1/4.0f},
		{-1/8.0f, -2/8.0f, 0, 2/8.0f, 1/8.0f},
		3,5
};

static kernel_seperable nrigo7x5 = {
		{1/16.0f, 4/16.0f, 6/16.0f, 4/16.0f, 1/16.0f},
		{-1/32.0f,-4/32.0f, -5/32.0f, 0, 5/32.0f, 4/32.0f, 1/32.0f},
		5,7
};

static kernel_seperable gsf3x3 = {
		{1/4.0f, 2/4.0f, 1/4.0f},
		{1/4.0f, 2/4.0f, 1/4.0f},
		3,3
};

static kernel_seperable gsf5x5 = {
		{1/16.0f, 4/16.0f, 6/16.0f, 4/16.0f, 1/16.0f},
		{1/16.0f, 4/16.0f, 6/16.0f, 4/16.0f, 1/16.0f},
		5,5
};

static kernel_seperable laplacian3x3 = {
		{1, -2, 1},
		{1, -2, 1},
		3,3
};

static kernel_seperable mean5x5 = {
		{1/5.0f, 1/5.0f, 1/5.0f, 1/5.0f, 1/5.0f},
		{1/5.0f, 1/5.0f, 1/5.0f, 1/5.0f, 1/5.0f},
		5,5
};

static kernel_normal gaussian_blur5x5 = {
	{
		2/159.0f, 4/159.0f, 5/159.0f, 4/159.0f, 2/159.0f,
		4/159.0f, 9/159.0f, 12/159.0f, 9/159.0f, 4/159.0f,
		5/159.0f, 12/159.0f, 15/159.0f, 12/159.0f, 5/159.0f,
		4/159.0f, 9/159.0f, 12/159.0f, 9/159.0f, 4/159.0f,
		2/159.0f, 4/159.0f, 5/159.0f, 4/159.0f, 2/159.0f
	},
	5,5
};

int convolve(mat* img_in, mat** out, int krnl, int dir)
{
	(*out) = new_image(img_in->width, img_in->height, img_in->max_val);

	if (krnl < KERNEL_SEPERABLE)
	{
		kernel_seperable* k;
		switch (krnl)
		{
			case KERNEL_SOBEL3x3:k = &sobel3x3;break;
			case KERNEL_SCHARR3x3:k = &scharr3x3;break;
			case KERNEL_NRIGO5x3:k = &nrigo5x3;break;
			case KERNEL_NRIGO7x5:k = &nrigo7x5;break;
			case KERNEL_GSF3x3:k = &gsf3x3;break;
			case KERNEL_GSF5x5:k = &gsf5x5;break;
			case KERNEL_LAPLACIAN3x3:k = &laplacian3x3;break;
			case KERNEL_MEAN5x5:k = &mean5x5;break;

			default:
			{
				// invalid kernel
				return 1;
			}
		}

		if (dir == DIRECTION_VERTICAL)
			convolve2DSeparable(img_in->data, (*out)->data, img_in->width, img_in->height,
					k->row, k->row_count, k->column, k->column_count);
		else
			convolve2DSeparable(img_in->data, (*out)->data, img_in->width, img_in->height,
					k->column, k->column_count, k->row, k->row_count);
	}
	else
	{
		kernel_normal* k;
		switch (krnl)
		{
			case KERNEL_GAUSSIANBLUR5x5:k = &gaussian_blur5x5;break;

			default:
			{
				// invalid kernel
				return 1;
			}
		}

		convolve2D(img_in->data, (*out)->data, img_in->width, img_in->height, k->k, k->width, k->height);
	}

	return 0;
}

void get_line(int cx, int cy, int radius, int a, int *x, int *y)
{
	float m = tan(PI*a / 180.0f);


	x[radius] = cx;
	y[radius] = cy;

	if (a==0 || a==180)
	{
		for (int i=1; i<=radius; i++)
		{
			x[radius+i] = cx+i;
			x[radius-i] = cx-i;
			y[radius+i] = cy;
			y[radius-i] = cy;
		}
	}
	else if (a==90)
	{
		for (int i=1; i<=radius; i++)
		{
			x[radius+i] = cx;
			x[radius-i] = cx;
			y[radius+i] = cy+i;
			y[radius-i] = cy-i;
		}
	}
	else if (a<45 || a>135)
	{
		float yy = cy + m;
		for (int i=1; i<=radius; i++)
		{
			x[radius+i] = cx+i;
			y[radius+i] = (int)(yy + 0.5);
			yy += m;
		}
		yy = cy - m;
		for (int i=1; i<=radius; i++)
		{
			x[radius-i] = cx-i;
			y[radius-i] = (int)(yy + 0.5);
			yy -= m;
		}
	}
	else
	{
		m = 1.0f/m;
		float xx = cx + m;
		for (int i=1; i<=radius; i++)
		{
			x[radius+i] = (int)(xx+0.5);
			y[radius+i] = cy+i;
			xx += m;
		}
		xx = cx - m;
		for (int i=1; i<=radius; i++)
		{
			x[radius-i] = (int)(xx+0.5);
			y[radius-i] = cy-i;
			xx -= m;
		}
	}

}

static const unsigned char BitReverseTable256[] =
{
  0x00, 0x80, 0x40, 0xC0, 0x20, 0xA0, 0x60, 0xE0, 0x10, 0x90, 0x50, 0xD0, 0x30, 0xB0, 0x70, 0xF0,
  0x08, 0x88, 0x48, 0xC8, 0x28, 0xA8, 0x68, 0xE8, 0x18, 0x98, 0x58, 0xD8, 0x38, 0xB8, 0x78, 0xF8,
  0x04, 0x84, 0x44, 0xC4, 0x24, 0xA4, 0x64, 0xE4, 0x14, 0x94, 0x54, 0xD4, 0x34, 0xB4, 0x74, 0xF4,
  0x0C, 0x8C, 0x4C, 0xCC, 0x2C, 0xAC, 0x6C, 0xEC, 0x1C, 0x9C, 0x5C, 0xDC, 0x3C, 0xBC, 0x7C, 0xFC,
  0x02, 0x82, 0x42, 0xC2, 0x22, 0xA2, 0x62, 0xE2, 0x12, 0x92, 0x52, 0xD2, 0x32, 0xB2, 0x72, 0xF2,
  0x0A, 0x8A, 0x4A, 0xCA, 0x2A, 0xAA, 0x6A, 0xEA, 0x1A, 0x9A, 0x5A, 0xDA, 0x3A, 0xBA, 0x7A, 0xFA,
  0x06, 0x86, 0x46, 0xC6, 0x26, 0xA6, 0x66, 0xE6, 0x16, 0x96, 0x56, 0xD6, 0x36, 0xB6, 0x76, 0xF6,
  0x0E, 0x8E, 0x4E, 0xCE, 0x2E, 0xAE, 0x6E, 0xEE, 0x1E, 0x9E, 0x5E, 0xDE, 0x3E, 0xBE, 0x7E, 0xFE,
  0x01, 0x81, 0x41, 0xC1, 0x21, 0xA1, 0x61, 0xE1, 0x11, 0x91, 0x51, 0xD1, 0x31, 0xB1, 0x71, 0xF1,
  0x09, 0x89, 0x49, 0xC9, 0x29, 0xA9, 0x69, 0xE9, 0x19, 0x99, 0x59, 0xD9, 0x39, 0xB9, 0x79, 0xF9,
  0x05, 0x85, 0x45, 0xC5, 0x25, 0xA5, 0x65, 0xE5, 0x15, 0x95, 0x55, 0xD5, 0x35, 0xB5, 0x75, 0xF5,
  0x0D, 0x8D, 0x4D, 0xCD, 0x2D, 0xAD, 0x6D, 0xED, 0x1D, 0x9D, 0x5D, 0xDD, 0x3D, 0xBD, 0x7D, 0xFD,
  0x03, 0x83, 0x43, 0xC3, 0x23, 0xA3, 0x63, 0xE3, 0x13, 0x93, 0x53, 0xD3, 0x33, 0xB3, 0x73, 0xF3,
  0x0B, 0x8B, 0x4B, 0xCB, 0x2B, 0xAB, 0x6B, 0xEB, 0x1B, 0x9B, 0x5B, 0xDB, 0x3B, 0xBB, 0x7B, 0xFB,
  0x07, 0x87, 0x47, 0xC7, 0x27, 0xA7, 0x67, 0xE7, 0x17, 0x97, 0x57, 0xD7, 0x37, 0xB7, 0x77, 0xF7,
  0x0F, 0x8F, 0x4F, 0xCF, 0x2F, 0xAF, 0x6F, 0xEF, 0x1F, 0x9F, 0x5F, 0xDF, 0x3F, 0xBF, 0x7F, 0xFF
};

unsigned int bit_reverse(unsigned int v, int num_bits)
{
	unsigned int c; // c will get v reversed

	c =	(BitReverseTable256[v & 0xff] << 24) |
		(BitReverseTable256[(v >> 8) & 0xff] << 16) |
		(BitReverseTable256[(v >> 16) & 0xff] << 8) |
		(BitReverseTable256[(v >> 24) & 0xff]);
	c >>= 32 - num_bits;

	return c;
}

unsigned int num_bits(unsigned int x)
{
	int bits = 0;
	while (x)
	{
		bits++;
		x >>= 1;
	}
	return --bits;
}

void dft(short int dir, long m, double *x, double *y)
{
	long n,i,i1,j,k,i2,l,l1,l2;
	double c1,c2,tx,ty,t1,t2,u1,u2,z;

	/* Calculate the number of points */
	n = 1;
	for (i=0;i<m;i++)
	  n *= 2;

	/* Do the bit reversal */
	i2 = n >> 1;
	j = 0;
	for (i=0;i<n-1;i++) {
	  if (i < j) {
		 tx = x[i];
		 ty = y[i];
		 x[i] = x[j];
		 y[i] = y[j];
		 x[j] = tx;
		 y[j] = ty;
	  }
	  k = i2;
	  while (k <= j) {
		 j -= k;
		 k >>= 1;
	  }
	  j += k;
	}

	/* Compute the FFT */
	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for (l=0;l<m;l++) {
	  l1 = l2;
	  l2 <<= 1;
	  u1 = 1.0;
	  u2 = 0.0;
	  for (j=0;j<l1;j++) {
		 for (i=j;i<n;i+=l2) {
			i1 = i + l1;
			t1 = u1 * x[i1] - u2 * y[i1];
			t2 = u1 * y[i1] + u2 * x[i1];
			x[i1] = x[i] - t1;
			y[i1] = y[i] - t2;
			x[i] += t1;
			y[i] += t2;
		 }
		 z =  u1 * c1 - u2 * c2;
		 u2 = u1 * c2 + u2 * c1;
		 u1 = z;
	  }
	  c2 = sqrt((1.0 - c1) / 2.0);
	  if (dir == 1)
		 c2 = -c2;
	  c1 = sqrt((1.0 + c1) / 2.0);
	}

	/* Scaling for forward transform */
	if (dir == 1) {
	  for (i=0;i<n;i++) {
		 x[i] /= n;
		 y[i] /= n;
	  }
	}

}

void dft2d(short int dir, long nx, long ny, double *x, double *y)
{
	long n, i, i1, j, k, i2, l, l1, l2;
	double c1, c2, tx, ty, t1, t2, u1, u2, z;

	// Calculate the number of points
	int columns = 1;
	for (i = 0; i < nx; i++)
		columns *= 2;
	int rows = 1;
	for (i=0; i<ny; i++)
		rows *=2;

	// Do the bit reversal
	n = columns;
	i2 = n >> 1;
	j = 0;
	for (i = 0; i < n - 1; i++)
	{
		if (i < j)
		{
			for (int row=0; row<rows; row++)
			{
				tx = x[row*columns + i];
				ty = y[row*columns + i];
				x[row*columns + i] = x[row*columns + j];
				y[row*columns + i] = y[row*columns + j];
				x[row*columns + j] = tx;
				y[row*columns + j] = ty;
			}
		}
		k = i2;
		while (k <= j)
		{
			j -= k;
			k >>= 1;
		}
		j += k;
	}


	// Compute the FFT
	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for (l = 0; l < nx; l++)
	{
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for (j = 0; j < l1; j++)
		{
			for (i = j; i < n; i += l2)
			{
				for (int row=0; row<rows; row++)
				{
					i1 = i + l1;
					t1 = u1 * x[row*columns + i1] - u2 * y[row*columns + i1];
					t2 = u1 * y[row*columns + i1] + u2 * x[row*columns + i1];
					x[row*columns + i1] = x[row*columns + i] - t1;
					y[row*columns + i1] = y[row*columns + i] - t2;
					x[row*columns + i] += t1;
					y[row*columns + i] += t2;
				}
			}
			z = u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		if (dir == 1)
			c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}

	// Do the bit reversal
	n = rows;
	i2 = n >> 1;
	j = 0;
	for (i = 0; i < n - 1; i++)
	{
		if (i < j)
		{
			for (int col=0; col<columns; col++)
			{
				tx = x[i*columns + col];
				ty = y[i*columns + col];
				x[i*columns + col] = x[j*columns + col];
				y[i*columns + col] = y[j*columns + col];
				x[j*columns + col] = tx;
				y[j*columns + col] = ty;
			}
		}
		k = i2;
		while (k <= j)
		{
			j -= k;
			k >>= 1;
		}
		j += k;
	}

	// Compute the FFT
	c1 = -1.0;
	c2 = 0.0;
	l2 = 1;
	for (l = 0; l < ny; l++)
	{
		l1 = l2;
		l2 <<= 1;
		u1 = 1.0;
		u2 = 0.0;
		for (j = 0; j < l1; j++)
		{
			for (i = j; i < n; i += l2)
			{
				for (int col=0; col<columns; col++)
				{
					i1 = i + l1;
					t1 = u1 * x[i1*columns + col] - u2 * y[i1*columns + col];
					t2 = u1 * y[i1*columns + col] + u2 * x[i1*columns + col];
					x[i1*columns + col] = x[i*columns + col] - t1;
					y[i1*columns + col] = y[i*columns + col] - t2;
					x[i*columns + col] += t1;
					y[i*columns + col] += t2;
				}
			}
			z = u1 * c1 - u2 * c2;
			u2 = u1 * c2 + u2 * c1;
			u1 = z;
		}
		c2 = sqrt((1.0 - c1) / 2.0);
		if (dir == 1)
			c2 = -c2;
		c1 = sqrt((1.0 + c1) / 2.0);
	}

	if (dir == 1)
	{
		int scale = rows*columns;
		for (int pos=0; pos<scale; pos++)
			{
				x[pos] /= scale;
				y[pos] /= scale;
			}
	}
}
