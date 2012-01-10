/**
 * @file benchmark.c
 * @author Andrew Cannon
 * @date January 2012
 *
 * @brief Functions for performing benchmarking on JPEG 2000 compression.
 *
 * Currently includes quality benchmarks.  Other benchmarks may be added.
 */

// Some code modified from OpenJPEG.
// OpenJPEG code is provided under the following BSD license.
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

#include "f2j.h"

void error_callback(const char *msg, void *client_data) {
}

void warning_callback(const char *msg, void *client_data) {
}

void info_callback(const char *msg, void *client_data) {
}

/**
 * Read a JPEG 2000 image from a file, decompress it and create an OpenJPEG
 * image structure from it.
 *
 * Modified version of code in j2k_to_image.c from OpenJPEG.  Some generality is
 * sacrificed for simplicity, as particular codebranches in j2k_to_image.c will never
 * be called, given the way that our program encodes J2K images.
 *
 * Very basic parameter checking is performed, but it is largely left to the
 * calling code to ensure parameters are valid and meaningful.
 *
 * @param imageFile Name of JPEG 2000 image file to decompress.
 * @param image Reference to a pointer to OpenJPEG image structure which will be populated
 * with data from the decompressed J2K image.  The reference should initially point to a
 * null pointers.
 * @param codec JPEG 2000 codec (JP2 or J2K) used.
 *
 * @return 0 if the file was read successfully, 1 otherwise.
 */
int readJ2K(char *imageFile, opj_image_t **image, OPJ_CODEC_FORMAT codec) {
	if (imageFile == NULL) {
		fprintf(stderr,"Filename provided to readJ2K cannot be null.\n");
		return 1;
	}

	if (*image != NULL) {
		fprintf(stderr,"Image structure pointer provided to read J2K must be null.\n");
	}

	FILE *fsrc = NULL;
	opj_dparameters_t parameters;			/* decompression parameters */
	opj_event_mgr_t event_mgr;				/* event manager */
	opj_stream_t *cio = NULL;				/* Stream */
	opj_codec_t* dinfo = NULL;				/* Handle to a decompressor */

	/* configure the event callbacks (not required) */
	memset(&event_mgr, 0, sizeof(opj_event_mgr_t));
	event_mgr.error_handler = error_callback;
	event_mgr.warning_handler = warning_callback;
	event_mgr.info_handler = info_callback;

	/* set decoding parameters to default values */
	opj_set_default_decoder_parameters(&parameters);

	/* read the input file and put it in memory */
	/* ---------------------------------------- */
	fsrc = fopen(imageFile, "rb");
	if (!fsrc) {
		fprintf(stderr, "ERROR -> failed to open %s for reading\n",imageFile);
		return 1;
	}

	cio = opj_stream_create_default_file_stream(fsrc,1);
	if (!cio){
		fclose(fsrc);
		fprintf(stderr, "ERROR -> failed to create the stream from the file\n");
		return 1;
	}

	/* decode the JPEG2000 stream */
	/* ---------------------- */
	dinfo = opj_create_decompress_v2(codec);

	/* Setup the decoder decoding parameters using user parameters */
	if ( !opj_setup_decoder_v2(dinfo, &parameters, &event_mgr) ){
		fprintf(stderr, "ERROR -> j2k_dump: failed to setup the decoder\n");
		opj_stream_destroy(cio);
		fclose(fsrc);
		opj_destroy_codec(dinfo);
		return 1;
	}

	/* Read the main header of the codestream and if necessary the JP2 boxes*/
	if(! opj_read_header(cio, dinfo, image)){
		fprintf(stderr, "ERROR -> j2k_to_image: failed to read the header\n");
		opj_stream_destroy(cio);
		fclose(fsrc);
		opj_destroy_codec(dinfo);
		opj_image_destroy(*image);
		return 1;
	}

	/* Get the decoded image */
	if (!(opj_decode_v2(dinfo, cio, *image) && opj_end_decompress(dinfo,	cio))) {
		fprintf(stderr,"ERROR -> j2k_to_image: failed to decode image!\n");
		opj_destroy_codec(dinfo);
		opj_stream_destroy(cio);
		opj_image_destroy(*image);
		fclose(fsrc);
		return 1;
	}

	/* Close the byte stream */
	opj_stream_destroy(cio);
	fclose(fsrc);

	/* free remaining structures */
	if (dinfo) {
		opj_destroy_codec(dinfo);
	}

	return 0;
}

/**
 * Function to perform image quality benchmarking between a raw uncompressed image and a compressed JPEG 2000 file.
 *
 * Very basic parameter checking is performed, but it is largely left to the client code to verify that parameters
 * are meaningful.
 *
 * @param image Reference to OpenJPEG image structure representing uncompressed version of image.
 * @param compressedFile file name of compressed JPEG 2000 image.
 * @param parameters Reference to quality_benchmark_info structure specifying what quality benchmarks should be performed.
 * Currently a dummy variable, but will allow additional options to be specified in the future.
 * @param codec Codec (such as JP2/JPT/J2K) of compressed image file.
 *
 * @return 0 if the benchmarking was performed successfully, 1 otherwise.
 */
int performQualityBenchmarking(opj_image_t *image, char *compressedFile, quality_benchmark_info *parameters, OPJ_CODEC_FORMAT codec) {
	if (image == NULL || compressedFile == NULL) {
		fprintf(stderr,"Compressed and uncompressed images cannot be null.\n");
		return 1;
	}

	// Decompress JPEG 2000 image file into an OpenJPEG image structure.
	opj_image_t *compressedImage = NULL;
	int readResult = readJ2K(compressedFile,&compressedImage,codec);

	if (readResult != 0) {
		fprintf(stderr,"Unable to read JPEG file: %s\n",compressedFile);
		return 1;
	}

	// Specify whether two images are comparable on a pixel by pixel basis.  For this to be true,
	// they need to have the same dimensions and the same number of components.
	// By default true, otherwise we set this to be false when performing sanity checking below.
	bool pixelsComparable = true;

	// Loop variables
	int ii,jj;

	// Now compare the two images.  Start with some basic sanity checking.
	if (compressedImage->color_space != image->color_space) {
		fprintf(stdout,"COLOR_SPACE of compressed image does not match uncompressed image for file: %s\n",compressedFile);
	}

	if (compressedImage->icc_profile_len != image->icc_profile_len) {
		fprintf(stdout,"ICC_PROFILE_LEN of compressed image does not match uncompressed image for file: %s\n",compressedFile);
	}

	if (compressedImage->x0 != image->x0) {
		fprintf(stdout,"X0 of compressed image does not match uncompressed image for file: %s\n",compressedFile);
	}

	if (compressedImage->x1 != image->x1) {
		fprintf(stdout,"X1 of compressed image does not match uncompressed image for file: %s\n",compressedFile);
		pixelsComparable = false; // Width not equal.
	}

	if (compressedImage->y0 != image->y0) {
		fprintf(stdout,"Y0 of compressed image does not match uncompressed image for file: %s\n",compressedFile);
	}

	if (compressedImage->y1 != image->y1) {
		fprintf(stdout,"Y1 of compressed image does not match uncompressed image for file: %s\n",compressedFile);
		pixelsComparable = false; // Height not equal.
	}

	if (compressedImage->numcomps != image->numcomps) {
		fprintf(stdout,"Number of components in compressed and uncompressed images are not the same for file: %s\n",compressedFile);
		pixelsComparable = false; // Different numbers of components.
	}

	if (pixelsComparable) {
		// Perform pixel by pixel comparison, component by component.  We should have only 1 component, but wrap the code in
		// a loop in case we eventually have to deal with more.
		for (ii=0; ii<image->numcomps; ii++) {
			// Get short references to both compressed & uncompressed components.
			opj_image_comp_t compUC = image->comps[ii];
			opj_image_comp_t compC = compressedImage->comps[ii];

			// Perform sanity check comparison on component dimensions.
			if (compUC.w != compC.w || compUC.h != compC.h) {
				fprintf(stdout,"Component %d has different dimensions in uncompressed and compressed images for file: %s\n",ii,compressedFile);
				continue;
			}

			if (compUC.sgnd != compC.sgnd) {
				fprintf(stdout,"Component %d is differently signed in compressed and uncompressed images for file: %s\n",ii,compressedFile);
			}

			// Number of pixels in image.
			int pixels = compUC.w * compUC.h;

			// Squared error - initially 0.  Use a 64 bit integer.  (Hopefully this will not overflow!)
			unsigned long long int squaredError = 0;

			// Was pixel by pixel comparison successful?
			bool comparisonSuccessful = true;

			// Perform pixel by pixel comparison.
			for (jj=0; jj<pixels; jj++) {
				unsigned long long int oldSquareError = squaredError;

				squaredError += (compUC.data[jj]-compC.data[jj])*(compUC.data[jj]-compC.data[jj]);

				// Check for overflow.  We can never 'wrap around' completely, so we can check if the new
				// value is less than the old value.
				if (oldSquareError > squaredError) {
					comparisonSuccessful = false;
					fprintf(stdout,"Overflow occurred in pixel by pixel comparison for component %d of file %d %s\n",ii,jj,compressedFile);
				}
			}

			// Print out MSE info if there were no errors.
			if (comparisonSuccessful) {
				double mse = ((double) squaredError) / ((double) pixels);

				if (squaredError == 0) {
					// Don't calculate PSNR if squaredError is 0
					fprintf(stdout,"%s %llu %d %f NO-PSNR\n",compressedFile,squaredError,pixels,mse);
				}
				else {
					// Calculate PSNR

					// Maximum pixel value (for PSNR)
					int maxPixValue = 1;

					if (compC.prec == 16) {
						// Common value: 2^16-1
						maxPixValue = 65535;
					}
					else if (compC.prec == 8) {
						// Common value: 2^8-1
						maxPixValue = 255;
					}
					else {
						// 2^prec-1
						int prec = compC.prec;

						// 2^prec
						while (prec>0) {
							maxPixValue *= 2;
							prec--;
						}

						// -1
						maxPixValue -= 1;
					}

					// Calculate PSNR
					double psnr = 10.0 * log10( ( ((double)maxPixValue) * ((double)maxPixValue) ) / mse );

					fprintf(stdout,"%s %llu %d %f %f\n",compressedFile,squaredError,pixels,mse,psnr);
				}
			}
		}
	}
	else {
		// Unable to perform pixel by pixel comparison.
		fprintf(stdout,"Unable to perform pixel by pixel comparison on image %s\n",compressedFile);
	}

	// If benchmarking was successful, free image data structure.
	opj_image_destroy(compressedImage);

	return 0;
}
