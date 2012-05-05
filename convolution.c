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

#include <stdlib.h>
#include <math.h>

int convolve2D(int* in, int* out, int dataSizeX, int dataSizeY,
                float* kernel, int kernelSizeX, int kernelSizeY)
{
    int i, j, m, n;
    int *inPtr, *inPtr2, *outPtr;
    float *kPtr;
    int kCenterX, kCenterY;
    int rowMin, rowMax;                             // to check boundary of input array
    int colMin, colMax;                             //
    float sum;                                      // temp accumulation buffer

    // check validity of params
    if(!in || !out || !kernel) return 1;
    if(dataSizeX <= 0 || kernelSizeX <= 0) return 1;

    // find center position of kernel (half of kernel size)
    kCenterX = kernelSizeX >> 1;
    kCenterY = kernelSizeY >> 1;

    // init working  pointers
    inPtr = inPtr2 = &in[dataSizeX * kCenterY + kCenterX];  // note that  it is shifted (kCenterX, kCenterY),
    outPtr = out;
    kPtr = kernel;

    // start convolution
    for(i= 0; i < dataSizeY; ++i)                   // number of rows
    {
        // compute the range of convolution, the current row of kernel should be between these
        rowMax = i + kCenterY;
        rowMin = i - dataSizeY + kCenterY;

        for(j = 0; j < dataSizeX; ++j)              // number of columns
        {
            // compute the range of convolution, the current column of kernel should be between these
            colMax = j + kCenterX;
            colMin = j - dataSizeX + kCenterX;

            sum = 0;                                // set to 0 before accumulate

            // flip the kernel and traverse all the kernel values
            // multiply each kernel value with underlying input data
            for(m = 0; m < kernelSizeY; ++m)        // kernel rows
            {
                // check if the index is out of bound of input array
                if(m <= rowMax && m > rowMin)
                {
                    for(n = 0; n < kernelSizeX; ++n)
                    {
                        // check the boundary of array
                        if(n <= colMax && n > colMin)
                            sum += *(inPtr - n) * *kPtr;

                        ++kPtr;                     // next kernel
                    }
                }
                else
                    kPtr += kernelSizeX;            // out of bound, move to next row of kernel

                inPtr -= dataSizeX;                 // move input data 1 raw up
            }

            // convert integer number
            if(sum >= 0) *outPtr = (int)(sum + 0.5f);
            else *outPtr = (int)(sum - 0.5f);

            kPtr = kernel;                          // reset kernel to (0,0)
            inPtr = ++inPtr2;                       // next input
            ++outPtr;                               // next output
        }
    }

    return 0;
}

int convolve2DSeparable(int* in, int* out, int dataSizeX, int dataSizeY,
                         float* kernelX, int kSizeX, float* kernelY, int kSizeY)
{
    int i, j, k, m, n;
    float *tmp, *sum;                               // intermediate data buffer
    int *inPtr, *outPtr;                            // working pointers
    float *tmpPtr, *tmpPtr2;                        // working pointers
    int kCenter, kOffset, endIndex;                 // kernel indice

    // check validity of params
    if(!in || !out || !kernelX || !kernelY) return 1;
    if(dataSizeX <= 0 || kSizeX <= 0) return 1;

    // allocate temp storage to keep intermediate result
    tmp = malloc(dataSizeX * dataSizeY * sizeof(float));
    if(!tmp) return 1;  // memory allocation error

    // store accumulated sum
    sum = malloc(dataSizeX*sizeof(float));
    if(!sum) return 1;  // memory allocation error

    // covolve horizontal direction ///////////////////////

    // find center position of kernel (half of kernel size)
    kCenter = kSizeX >> 1;                          // center index of kernel array
    endIndex = dataSizeX - kCenter;                 // index for full kernel convolution

    // init working pointers
    inPtr = in;
    tmpPtr = tmp;                                   // store intermediate results from 1D horizontal convolution

    // start horizontal convolution (x-direction)
    for(i=0; i < dataSizeY; ++i)                    // number of rows
    {

        kOffset = 0;                                // starting index of partial kernel varies for each sample

        // COLUMN FROM index=0 TO index=kCenter-1
        for(j=0; j < kCenter; ++j)
        {
            *tmpPtr = 0;                            // init to 0 before accumulation

            for(k = kCenter + kOffset, m = 0; k >= 0; --k, ++m) // convolve with partial of kernel
            {
                *tmpPtr += *(inPtr + m) * kernelX[k];
            }
            ++tmpPtr;                               // next output
            ++kOffset;                              // increase starting index of kernel
        }

        // COLUMN FROM index=kCenter TO index=(dataSizeX-kCenter-1)
        for(j = kCenter; j < endIndex; ++j)
        {
            *tmpPtr = 0;                            // init to 0 before accumulate

            for(k = kSizeX-1, m = 0; k >= 0; --k, ++m)  // full kernel
            {
                *tmpPtr += *(inPtr + m) * kernelX[k];
            }
            ++inPtr;                                // next input
            ++tmpPtr;                               // next output
        }

        kOffset = 1;                                // ending index of partial kernel varies for each sample

        // COLUMN FROM index=(dataSizeX-kCenter) TO index=(dataSizeX-1)
        for(j = endIndex; j < dataSizeX; ++j)
        {
            *tmpPtr = 0;                            // init to 0 before accumulation

            for(k = kSizeX-1, m=0; k >= kOffset; --k, ++m)   // convolve with partial of kernel
            {
                *tmpPtr += *(inPtr + m) * kernelX[k];
            }
            ++inPtr;                                // next input
            ++tmpPtr;                               // next output
            ++kOffset;                              // increase ending index of partial kernel
        }

        inPtr += kCenter;                           // next row
    }
    // END OF HORIZONTAL CONVOLUTION //////////////////////

    // start vertical direction ///////////////////////////

    // find center position of kernel (half of kernel size)
    kCenter = kSizeY >> 1;                          // center index of vertical kernel
    endIndex = dataSizeY - kCenter;                 // index where full kernel convolution should stop

    // set working pointers
    tmpPtr = tmpPtr2 = tmp;
    outPtr = out;

    // clear out array before accumulation
    for(i = 0; i < dataSizeX; ++i)
        sum[i] = 0;

    // start to convolve vertical direction (y-direction)

    // ROW FROM index=0 TO index=(kCenter-1)
    kOffset = 0;                                    // starting index of partial kernel varies for each sample
    for(i=0; i < kCenter; ++i)
    {
        for(k = kCenter + kOffset; k >= 0; --k)     // convolve with partial kernel
        {
            for(j=0; j < dataSizeX; ++j)
            {
                sum[j] += *tmpPtr * kernelY[k];
                ++tmpPtr;
            }
        }

        for(n = 0; n < dataSizeX; ++n)              // convert and copy from sum to out
        {
            if(sum[n] >= 0)
                *outPtr = (int)(sum[n] + 0.5f);     // store final result to output array
            else
                *outPtr = (int)(sum[n] - 0.5f);     // store final result to output array

            sum[n] = 0;                             // reset to zero for next summing
            ++outPtr;                               // next element of output
        }

        tmpPtr = tmpPtr2;                           // reset input pointer
        ++kOffset;                                  // increase starting index of kernel
    }

    // ROW FROM index=kCenter TO index=(dataSizeY-kCenter-1)
    for(i = kCenter; i < endIndex; ++i)
    {
        for(k = kSizeY -1; k >= 0; --k)             // convolve with full kernel
        {
            for(j = 0; j < dataSizeX; ++j)
            {
                sum[j] += *tmpPtr * kernelY[k];
                ++tmpPtr;
            }
        }

        for(n = 0; n < dataSizeX; ++n)              // convert and copy from sum to out
        {
            if(sum[n] >= 0)
                *outPtr = (int)(sum[n] + 0.5f);     // store final result to output array
            else
                *outPtr = (int)(sum[n] - 0.5f);     // store final result to output array
            sum[n] = 0;                             // reset to 0 before next summing
            ++outPtr;                               // next output
        }

        // move to next row
        tmpPtr2 += dataSizeX;
        tmpPtr = tmpPtr2;
    }

    // ROW FROM index=(dataSizeY-kCenter) TO index=(dataSizeY-1)
    kOffset = 1;                                    // ending index of partial kernel varies for each sample
    for(i=endIndex; i < dataSizeY; ++i)
    {
        for(k = kSizeY-1; k >= kOffset; --k)        // convolve with partial kernel
        {
            for(j=0; j < dataSizeX; ++j)
            {
                sum[j] += *tmpPtr * kernelY[k];
                ++tmpPtr;
            }
        }

        for(n = 0; n < dataSizeX; ++n)              // convert and copy from sum to out
        {
            if(sum[n] >= 0)
                *outPtr = (int)(sum[n] + 0.5f);     // store final result to output array
            else
                *outPtr = (int)(sum[n] - 0.5f);     // store final result to output array
            sum[n] = 0;                             // reset before next summing
            ++outPtr;                               // next output
        }

        // move to next row
        tmpPtr2 += dataSizeX;
        tmpPtr = tmpPtr2;                           // next input
        ++kOffset;                                  // increase ending index of kernel
    }
    // END OF VERTICAL CONVOLUTION ////////////////////////

    // deallocate temp buffers
    free(tmp);
    free(sum);
    return 0;
}
