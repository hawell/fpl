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

#ifndef LINEAR_H_
#define LINEAR_H_

#define DFT_DIRECT	1
#define DFT_REVERSE	-1

#define PI	3.141592654

typedef struct {
	int len;
	float* data;
} vector;

void get_line(int cx, int cy, int radius, int a, int *x, int *y);

int dft(int dir, int m, float *x, float *y);
void dft1(int size, float* in, float* out_r, float* out_i);

#endif /* LINEAR_H_ */
