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

#ifndef PROCESS_H_
#define PROCESS_H_

#include "image.h"

#define STREL_DISK	0

void strel(int el, int size, mat** se);

void dilate(mat* img, mat* strel);
void erode(mat* img, mat* strel);

void gradient_x(mat* img_in, mat** img_out, int kernel);
void gradient_y(mat* img_in, mat** img_out, int kernel);
void gradient(mat* img_in, mat** img_out, int kernel);

void sharpen(mat* img_in, mat** img_out);

void negative(mat* img);

void binarize(mat* img, int size);
void binarize2(mat* img, int threshold);

void normalize(mat* m, int min_val, int max_val);

void histogram_equalization(mat* img);

void clahe(mat* img, int x_count, int y_count, float clip_limit);

void normalization_enhance(mat* img, int m0, int v0);

void wiener_filter(mat* img, int size);

void remove_background(mat* img, int size);

#endif /* PROCESS_H_ */
