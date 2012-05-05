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

#include "linear.h"

#include <math.h>

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

int dft(int dir, int m, float *x, float *y)
{
   long n,i,i1,j,k,i2,l,l1,l2;
   float c1,c2,tx,ty,t1,t2,u1,u2,z;

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

   return 0;
}

void dft1(int size, float* in, float* out_r, float* out_i)
{
	double pi2 = -2.0 * PI;
	double a,ca,sa;
	double invs = 1.0 / size;
	for(int y=0; y<size; y++)
	{
		out_r[y] = out_i[y] = 0;
		for(int x=0; x<size; x++)
		{
			a = pi2 * y * x * invs;
			ca = cos(a);
			sa = sin(a);
			out_r[y] += in[x] * ca;
			out_i[y] += in[x] * sa;
		}
		out_r[y] *= invs;
		out_i[y] *= invs;
	}
}
