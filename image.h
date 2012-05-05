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

#define IMAGE_DATA_MAIN					0
#define IMAGE_DATA_GRADIENT				1
#define IMAGE_DATA_LRO					2

#define IMAGE_STATE_RAW					0
#define IMAGE_STATE_RAW_ENHANCED		1
#define IMAGE_STATE_BINARY				2
#define IMAGE_STATE_BINARY_ENHANCED		3
#define IMAGE_STATE_THINNED				4

#define IMAGE_FLAG_HAS_LRO				1
#define IMAGE_FLAG_HAS_LRF				2
#define IMAGE_FLAG_HAS_GRAD				4

typedef struct {
	int width, height;
	int *data;
} mat;

typedef struct {
	int width, height;
	int *data;
	int len;
	int max_val;

	unsigned int state;
	unsigned int flags;

	mat grad;

	int lro_radius;
	mat lro_dir;
	mat lro_rate;

	mat lrf;
} image;

image* new_image(int w, int h, int m);
image* clone_image(image* img);

void free_image(image* img);

int load_from_file(char* file_name, image** img);
int save_to_file(char* file_name, image* img, int img_data);
int save_data(char* file_name, int* data, int w, int h, int max_val);

#endif /* IMAGE_H_ */
