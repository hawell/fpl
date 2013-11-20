/*
 * fingerprint.h
 *
 *  Created on: Nov 6, 2013
 *      Author: arash
 */

#ifndef FINGERPRINT_H_
#define FINGERPRINT_H_

#include "image.h"
#include "lro.h"

void ROI(mat* img, mat* grad, int size);

void thin(mat* img);

/* Directional Median Filter */
void DMF(mat* img, ridge_orientation* lro);

/* Lined Directional Median Filter */
void LDMF(mat* img, ridge_orientation* lro, int radius);

/* Directional Gabor Filter */
void DGF(mat* img);


#endif /* FINGERPRINT_H_ */
