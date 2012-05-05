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

void gradient(image* img, int kernel);

int ridge_orientation(image* img, int radius);

void get_background(image* img, int size, image** bg);

void binarize(image* img, int size);
void binarize2(image* img, int threshold);

void thin(image* img);



#endif /* PROCESS_H_ */
