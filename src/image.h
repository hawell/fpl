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

#ifndef IMAGE_H_
#define IMAGE_H_

typedef struct {
	int width, height;
	int len;
	int max_val;
	int *data;
} mat;

mat* new_image(int w, int h, int m);
mat* clone_image(mat* img);

void free_image(mat* img);

int load_from_file(const char* file_name, mat** img);
int save_to_file(const char* file_name, mat* img);

int resize(mat* img, int width, int height);

#endif /* IMAGE_H_ */
