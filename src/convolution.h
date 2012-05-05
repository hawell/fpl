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

#ifndef CONVOLUTION_H_
#define CONVOLUTION_H_

int convolve2D(int* in, int* out, int dataSizeX, int dataSizeY,
                float* kernel, int kernelSizeX, int kernelSizeY);

int convolve2DSeparable(int* in, int* out, int dataSizeX, int dataSizeY,
                         float* kernelX, int kSizeX, float* kernelY, int kSizeY);

#endif /* CONVOLUTION_H_ */
