/**
 * @file f2j.h
 * @author Andrew Cannon
 * @date December 2011
 *
 * @brief Header file for f2j.
 *
 * Defines global functions and data structures.  Includes library
 * header files.
 */

// This file uses code from the OpenJPEG library, which carries the following license.
/*
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium
 * Copyright (c) 2002-2007, Professor Benoit Macq
 * Copyright (c) 2001-2003, David Janssens
 * Copyright (c) 2002-2003, Yannick Verschueren
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe
 * Copyright (c) 2005, Herve Drolon, FreeImage Team
 * Copyright (c) 2006-2007, Parvatha Elangovan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef F2J_H_
#define F2J_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "fitsio.h"
#include "openjpeg.h"

/**
 * Structure for defining essential properties of a FITS datacube.
 */
typedef struct {
	long width /** Image width. */;
	long height /** Image height. */;
	long depth /** Image depth.  Arbitrary for 2D images. */;
	int naxis /** Number of dimensions of the data cube. */;
	int bitpix /** Image data type.  Same as BITPIX in CFITSIO. */;
} cube_info;

/**
 * Enumerated type defining the transformations that may be performed
 * on raw FITS data to convert each datum into a 16 bit grayscale
 * (integer) intensity.
 *
 * Not all transforms will be defined for all FITS image types.
 */
typedef enum {
	LOG /** Logarithmic scale. */,
	NEGATIVE_LOG /** Inverse image from logarithmic scale. */,
	LINEAR /** Linear scale. */,
	NEGATIVE_LINEAR /** Inverse image from linear scale. */,
	RAW /** Convert raw values to image intensities.  Only defined for FITS files containing short/byte data.  If raw daa is signed, it will be shifted to be unsigned.  */,
	NEGATIVE_RAW /** Inverse image from raw transform. */
} transform;

// External function declarations.
// f2j.c
extern void displayHelp();
// openjpeg.c
extern int parse_cmdline_encoder(int,char **,opj_cparameters_t *,transform *,bool *);

#endif /* F2J_H_ */
