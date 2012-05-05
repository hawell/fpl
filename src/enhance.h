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

#ifndef ENHANCE_H_
#define ENHANCE_H_

#include "image.h"

void normalize(mat* m, int min_val, int max_val);

void histogram_equalization(image* img);

void clahe(image* img, int x_count, int y_count, float clip_limit);

void normalization_enhance(image* img, int m0, int v0);

void wiener_filter(image* img, int size);

void remove_background(image* img, int size);


#endif /* ENHANCE_H_ */
