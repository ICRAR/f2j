/**
 * @file f2j.h
 * @author Andrew Cannon
 * @date December 2011
 *
 * @brief Header file for f2j.
 *
 * Defines global functions and data structures.  Includes necessary library
 * header files.
 *
 * @section LICENSE
 *
 * This program is distributed under the <a href="http://www.gnu.org/copyleft/gpl.html">GNU General Public License</a> (GPL).
 *
 * <b>OpenJPEG</b>
 *
 * This program uses code from the <a href="http://www.openjpeg.org/">OpenJPEG library</a>, which carries the following BSD license.
 *
 * Copyright (c) 2002-2007, Communications and Remote Sensing Laboratory, Universite catholique de Louvain (UCL), Belgium<br/>
 * Copyright (c) 2002-2007, Professor Benoit Macq<br/>
 * Copyright (c) 2001-2003, David Janssens<br/>
 * Copyright (c) 2002-2003, Yannick Verschueren<br/>
 * Copyright (c) 2003-2007, Francois-Olivier Devaux and Antonin Descampe<br/>
 * Copyright (c) 2005, Herve Drolon, FreeImage Team<br/>
 * Copyright (c) 2006-2007, Parvatha Elangovan<br/>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:<br/>
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.<br/>
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
 *
 * <b>CFITSIO</b>
 *
 * The <a href="http://heasarc.gsfc.nasa.gov/fitsio/">CFITSIO library</a> is also
 * used by this program, which carries the following license.
 *
 * Copyright (Unpublished--all rights reserved under the copyright laws of
 * the United States), U.S. Government as represented by the Administrator
 * of the National Aeronautics and Space Administration.  No copyright is
 * claimed in the United States under Title 17, U.S. Code.
 *
 * Permission to freely use, copy, modify, and distribute this software
 * and its documentation without fee is hereby granted, provided that this
 * copyright notice and disclaimer of warranty appears in all copies.
 *
 * DISCLAIMER:
 *
 * THE SOFTWARE IS PROVIDED 'AS IS' WITHOUT ANY WARRANTY OF ANY KIND,
 * EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO,
 * ANY WARRANTY THAT THE SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
 * PURPOSE, AND FREEDOM FROM INFRINGEMENT, AND ANY WARRANTY THAT THE
 * DOCUMENTATION WILL CONFORM TO THE SOFTWARE, OR ANY WARRANTY THAT THE
 * SOFTWARE WILL BE ERROR FREE.  IN NO EVENT SHALL NASA BE LIABLE FOR ANY
 * DAMAGES, INCLUDING, BUT NOT LIMITED TO, DIRECT, INDIRECT, SPECIAL OR
 * CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN ANY WAY
 * CONNECTED WITH THIS SOFTWARE, WHETHER OR NOT BASED UPON WARRANTY,
 * CONTRACT, TORT , OR OTHERWISE, WHETHER OR NOT INJURY WAS SUSTAINED BY
 * PERSONS OR PROPERTY OR OTHERWISE, AND WHETHER OR NOT LOSS WAS SUSTAINED
 * FROM, OR AROSE OUT OF THE RESULTS OF, OR USE OF, THE SOFTWARE OR
 * SERVICES PROVIDED HEREUNDER.
 *
 * <b>GNU Scientific Library (GSL)</b>
 *
 * The <a href="http://www.gnu.org/software/gsl/">GNU Scientific Library</a> is
 * used by this program, which is distributed under the
 * <a href="http://www.gnu.org/copyleft/gpl.html">GNU General Public License</a> (GPL).
 */

#ifndef F2J_H_
#define F2J_H_

/**
 * Do we want to be able to add (Gaussian) noise to an image?  Delete this definition
 * if this is not desired.  Adding noise adds overhead to the FITS -> JPEG 2000 conversion
 * process and is a somewhat specialised function, so it can be removed for greater speed
 * if not desired.
 */
#define noise

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <openjpeg-1.99/openjpeg.h>

#ifdef noise
#include <time.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#endif

#include "fitsio.h"

/**
 * Structure for defining essential properties of a FITS datacube.
 */
typedef struct {
	long width /** Image width. */;
	long height /** Image height. */;
	long depth /** Image depth.  Arbitrary for 2D images. */;
	long stokes /** Number of stokes in image.  Arbitrary for 2D or 3D images. */;
	int naxis /** Number of dimensions of the data cube. */;
	int bitpix /** Image data type.  Same as BITPIX in CFITSIO. */;
} cube_info;

/**
 * Structure allowing parameters for quality benchmarking to be specified
 * by the user.  Currently, numerous different quality benchmarks can be
 * specified by the user at the command line and information on these is
 * recorded in here.
 *
 * The last three metrics are not really of interest in their own right, but
 * are integer intermediate building blocks of some of the other metrics that
 * might be useful if integer, rather than floating point, raw data is desired.
 *
 * The last field specifies whether a residual iamge should be written to a
 * file.  This can be used even if no other quality benchmarks are specified.
 */
typedef struct {
	bool meanSquaredError /** Mean squared error.  */;
	bool rootMeanSquaredError /** Root mean squared error.  */;
	bool peakSignalToNoiseRatio /** Peak signal to noise ratio. */;
	bool meanAbsoluteError /** Mean absolute error.  */;
	bool fidelity /** Fidelity.  */;
	bool maximumAbsoluteDistortion /** Maximum absolute distortion.  */;
	bool squaredError /** Squared error. */;
	bool absoluteError /** Absolute error. */;
	bool squaredIntensitySum /** Sum of squared uncompressed image intensities. */;

	bool performQualityBenchmarking /** Is at least one quality benchmark selected?  Intended to provide a quick check.  Client code must keep this up to date.  */;
	bool writeResidual /** Should the residual image be written to a file?  */;
} quality_benchmark_info;

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
	RAW /** Convert raw values to image intensities.  Only defined for FITS files containing short/byte data.  If raw data is signed, it will be shifted to be unsigned.  */,
	NEGATIVE_RAW /** Inverse image from raw transform. */,
	SQRT /** Square root scale. */,
	NEGATIVE_SQRT /** Inverse image from square root scale. */,
	SQUARED /** Squared scale. */,
	NEGATIVE_SQUARED /** Inverse image from squared scale. */,
	POWER /** Power scale. */,
	NEGATIVE_POWER /** Inverse image from power scale. */,
	DEFAULT /** Default transform to use if no transform is explicitly specified.  This will depend on the FITS data type.  */
} transform;

// External function declarations.
// f2j.c
extern void displayHelp();
int createJPEG2000Image(char *,OPJ_CODEC_FORMAT,opj_cparameters_t *,opj_image_t *);
// openjpeg.c
extern int parse_cmdline_encoder(int,char **,opj_cparameters_t *,transform *,bool *,long *,long *,quality_benchmark_info *,bool *, long *, long *
#ifdef noise
		, double *, bool *, unsigned long *, bool *, double *, bool *
#endif
);
void encode_help_display();
// benchmark.c
extern int performQualityBenchmarking(opj_image_t *,char *,quality_benchmark_info *,OPJ_CODEC_FORMAT);

#endif /* F2J_H_ */
