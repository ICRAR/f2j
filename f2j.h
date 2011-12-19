/*
 * f2j.h
 *
 * Header file for FITS -> JPEG 2000 converter.
 *
 *  Created on: Dec 19, 2011
 *      Author: acannon
 */

#ifndef F2J_H_
#define F2J_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fitsio.h"
#include "openjpeg.h"

// Structure for defining essential properties of a FITS datacube.
typedef struct {
	long width;
	long height;
	long depth;
	int naxis; // Number of dimensions of the data cube.
	int bitpix; // Image data type.
} cube_info;

// Enumerated type defining the transformations that may be performed
// on raw FITS data to convert each datum into a 16 bit grayscale
// (integer) intensity.
typedef enum {
	LOG,LINEAR,RAW
} transform;

#endif /* F2J_H_ */
