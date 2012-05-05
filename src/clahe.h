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

#ifndef CLAHE_H_
#define CLAHE_H_

#define BYTE_IMAGE

#ifdef BYTE_IMAGE
typedef unsigned char kz_pixel_t;
#define uiNR_OF_GREY (256)
#else
typedef unsigned short kz_pixel_t;
#define uiNR_OF_GREY (4096)
#endif

#define uiMAX_REG_X	16
#define uiMAX_REG_Y	16

int CLAHE(kz_pixel_t* pImage, unsigned int uiXRes, unsigned int uiYRes, kz_pixel_t Min,
	  kz_pixel_t Max, unsigned int uiNrX, unsigned int uiNrY,
	  unsigned int uiNrBins, float fCliplimit);


#endif /* CLAHE_H_ */
