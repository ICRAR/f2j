/**
 * @file benchmark.c
 * @author Andrew Cannon
 * @date January 2012
 *
 * @brief Functions relating to performing benchmarking on JPEG 2000 compression.
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

/**
 * Dummy callback function.
 */
void error_callback(const char *msg, void *client_data) {
}

/**
 * Dummy callback function.
 */
void warning_callback(const char *msg, void *client_data) {
}

/**
 * Dummy callback function.
 */
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
		return 1;
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
 * Function to perform image quality benchmarking between a raw uncompressed image and a compressed JPEG 2000 file,
 * possibly writing a residual image.
 *
 * Very basic parameter checking is performed, but it is largely left to the client code to verify that parameters
 * are meaningful.
 *
 * @param image Reference to OpenJPEG image structure representing uncompressed version of image.
 * @param compressedFile File name of compressed JPEG 2000 image.
 * @param parameters Reference to quality_benchmark_info structure specifying what quality benchmarks should be performed.
 * Currently allows specific benchmarks to be specified by the user.
 * @param codec Codec (such as JP2/JPT/J2K) of compressed image file.
 *
 * @return 0 if the benchmarking was performed successfully, 1 otherwise.
 */
int performQualityBenchmarking(opj_image_t *image, char *compressedFile, quality_benchmark_info *parameters, OPJ_CODEC_FORMAT codec) {
	if (image == NULL || compressedFile == NULL || parameters == NULL) {
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
	size_t kk;

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
		// Create residual image structure.
		opj_image_t residualImage;

		// Should a residual image be written?  By default yes, may change if there is an error doing comparisons.
		bool canWriteResidual = true;

		if (parameters->writeResidual) {
			residualImage.comps = (opj_image_comp_t *) malloc(sizeof(opj_image_comp_t) * image->numcomps);
			if (!residualImage.comps) {
				fprintf(stderr,"Unable to allocate memory for residual image components of file %s",compressedFile);
				opj_image_destroy(compressedImage);
				return 1;
			}

			residualImage.color_space = image->color_space;
			residualImage.icc_profile_buf = image->icc_profile_buf;
			residualImage.icc_profile_len = image->icc_profile_len;
			residualImage.numcomps = image->numcomps;
			residualImage.x0 = image->x0;
			residualImage.x1 = image->x1;
			residualImage.y0 = image->y0;
			residualImage.y1 = image->y1;
		}

		// Perform pixel by pixel comparison, component by component.  We should have only 1 component, but wrap the code in
		// a loop in case we eventually have to deal with more.
		for (ii=0; ii<image->numcomps; ii++) {
			// Get short name references to both compressed & uncompressed components.
			opj_image_comp_t compUC = image->comps[ii];
			opj_image_comp_t compC = compressedImage->comps[ii];

			// Number of pixels in image.
			size_t pixels = ((size_t) compUC.w) * ((size_t) compUC.h);

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
				int prec = compUC.prec;

				// 2^prec
				while (prec>0) {
					maxPixValue *= 2;
					prec--;
				}

				// -1
				maxPixValue -= 1;
			}

			if (parameters->writeResidual) {
				// Allocate memory for residual image data.
				residualImage.comps[ii].data = (int *) malloc(sizeof(int) * pixels);
				if (!residualImage.comps[ii].data) {
					// Free memory for other components.
					for (jj=0; jj<ii; jj++) {
						free(residualImage.comps[jj].data);
					}
					free(residualImage.comps);

					fprintf(stderr,"Unable to allocate memory for residual image component %d of file %s\n",ii,compressedFile);
					opj_image_destroy(compressedImage);
					return 1;
				}

				// Copy component information from uncompressed image.
				residualImage.comps[ii].bpp = compUC.bpp;
				residualImage.comps[ii].dx = compUC.dx;
				residualImage.comps[ii].dy = compUC.dy;
				residualImage.comps[ii].factor = compUC.factor;
				residualImage.comps[ii].h = compUC.h;
				residualImage.comps[ii].prec = compUC.prec;
				residualImage.comps[ii].resno_decoded = compUC.resno_decoded;
				residualImage.comps[ii].w = compUC.w;
				residualImage.comps[ii].x0 = compUC.x0;
				residualImage.comps[ii].y0 = compUC.y0;

				// We want a signed image for the image, so we can simply use the raw difference values.
				residualImage.comps[ii].sgnd = 1;
			}

			// Residual bounds
			int resMax = (maxPixValue + 1) / 2 - 1;
			int resMin = -resMax - 1;

			// Perform sanity check comparison on component dimensions.
			if (compUC.w != compC.w || compUC.h != compC.h) {
				fprintf(stdout,"Component %d has different dimensions in uncompressed and compressed images for file: %s\n",ii,compressedFile);
				canWriteResidual = false;
				continue;
			}

			if (compUC.sgnd != compC.sgnd) {
				fprintf(stdout,"Component %d is differently signed in compressed and uncompressed images for file: %s\n",ii,compressedFile);
				canWriteResidual = false;
				continue;
			}

			// Squared error - initially 0.  Use a 64 bit integer.  (Hopefully this will not overflow!)
			unsigned long long int squaredError = 0;

			// Absolute error - initially 0.
			unsigned long long int absoluteError = 0;

			// Pixel intensities (for fidelity metric) - initially 0.
			unsigned long long int intensitySquareSum = 0;

			// Maximum absolute error - initially 0.
			int maxAbsoluteError = 0;

			// Was pixel by pixel comparison successful?
			bool comparisonSuccessful = true;

			// Perform pixel by pixel comparison.
			for (kk=0; kk<pixels; kk++) {
				// Get long long int values of pixels (in some cases, we get the intensity square overflowing otherwise).
				long long int uv = (long long int) compUC.data[kk];
				long long int cv = (long long int) compC.data[kk];

				if (abs(uv-cv) > maxAbsoluteError) {
					maxAbsoluteError = abs(uv-cv);
				}

				// Sometimes we get overflow values

				unsigned long long int oldSquareError = squaredError;
				unsigned long long int oldAbsoluteError = absoluteError;
				unsigned long long int oldIntensitySquareSum = intensitySquareSum;

				squaredError += (uv-cv)*(uv-cv);
				absoluteError += abs(uv-cv);
				intensitySquareSum += uv*uv;

				// Check for overflow.  We can never 'wrap around' completely, so we can check if the new
				// value is less than the old value.

				// We only need to take action on an overflow if it affects one of the quality benchmarks we have
				// been asked to performed.
				if (oldSquareError > squaredError) {
					if (parameters->squaredError || parameters->meanSquaredError || parameters->peakSignalToNoiseRatio || parameters->rootMeanSquaredError || parameters->fidelity) {
						comparisonSuccessful = false;
						fprintf(stdout,"Overflow occurred in MSE pixel by pixel comparison for component %d of file %s\n",ii,compressedFile);
						break;
					}
				}

				if (oldAbsoluteError > absoluteError) {
					if (parameters->absoluteError || parameters->meanAbsoluteError) {
						comparisonSuccessful = false;
						fprintf(stdout,"Overflow occurred in MAE pixel by pixel comparison for component %d of file %s\n",ii,compressedFile);
						break;
					}
				}

				if (oldIntensitySquareSum > intensitySquareSum) {
					if (parameters->squaredIntensitySum || parameters->fidelity) {
						comparisonSuccessful = false;
						fprintf(stdout,"Overflow occurred in fidelity pixel by pixel comparison for component %d of file %s\n",ii,compressedFile);
						break;
					}
				}

				if (parameters->writeResidual) {
					residualImage.comps[ii].data[kk] = uv-cv;

					if (residualImage.comps[ii].data[kk] < resMin) {
						fprintf(stderr,"Overflow calculating residual image of file %s - pixel %zd set to %d\n",compressedFile,kk,resMin);
						residualImage.comps[ii].data[kk] = resMin;
					}
					else if (residualImage.comps[ii].data[kk] > resMax) {
						fprintf(stderr,"Overflow calculating residual image of file %s - pixel %zd set to %d\n",compressedFile,kk,resMax);
						residualImage.comps[ii].data[kk] = resMax;
					}
				}
			}

			// Print out quality benchmarks if all relevant computations were successful.
			if (comparisonSuccessful && parameters->performQualityBenchmarking) {
				// Construct string specifying what the output string consists of:
				fprintf(stdout,"[Compressed File Name] [Pixels]");

				if (parameters->squaredError) {
					fprintf(stdout," [SE]");
				}
				if (parameters->meanSquaredError) {
					fprintf(stdout," [MSE]");
				}
				if (parameters->rootMeanSquaredError) {
					fprintf(stdout," [RMSE]");
				}
				if (parameters->peakSignalToNoiseRatio) {
					fprintf(stdout," [PSNR]");
				}
				if (parameters->absoluteError) {
					fprintf(stdout," [AE]");
				}
				if (parameters->meanAbsoluteError) {
					fprintf(stdout," [MAE]");
				}
				if (parameters->squaredIntensitySum) {
					fprintf(stdout," [SI]");
				}
				if (parameters->fidelity) {
					fprintf(stdout, " [FID]");
				}
				if (parameters->maximumAbsoluteDistortion) {
					fprintf(stdout," [MAD]");
				}
				fprintf(stdout,"\n");

				// Calculate metrics to be printed out.
				double mse = ((double) squaredError) / ((double) pixels);

				fprintf(stdout,"%s %zd",compressedFile,pixels);

				if (parameters->squaredError) {
					fprintf(stdout," %llu",squaredError);
				}
				if (parameters->meanSquaredError) {
					fprintf(stdout," %f",mse);
				}
				if (parameters->rootMeanSquaredError) {
					double rmse = sqrt(mse);
					fprintf(stdout," %f",rmse);
				}
				if (parameters->peakSignalToNoiseRatio) {
					if (squaredError == 0) {
						fprintf(stdout," NO-PSNR");
					}
					else {
						double psnr = 10.0 * log10( ( ((double)maxPixValue) * ((double)maxPixValue) ) / mse );
						fprintf(stdout," %f",psnr);
					}
				}
				if (parameters->absoluteError) {
					fprintf(stdout," %llu",absoluteError);
				}
				if (parameters->meanAbsoluteError) {
					double mae = ((double) absoluteError) / ((double) pixels);
					fprintf(stdout," %f",mae);
				}
				if (parameters->squaredIntensitySum) {
					fprintf(stdout," %llu",intensitySquareSum);
				}
				if (parameters->fidelity) {
					double fidelity = 1.0 - ((double) squaredError) / ((double) intensitySquareSum);
					fprintf(stdout, " %f",fidelity);
				}
				if (parameters->maximumAbsoluteDistortion) {
					fprintf(stdout," %d",maxAbsoluteError);
				}
				fprintf(stdout,"\n");
			}
		}

		if (parameters->writeResidual && canWriteResidual) {
			// Write residual image to file - we'll use lossless JP2 to store it.

			// Lossless compression parameters.
			opj_cparameters_t lossless;
			// Initialise to default values.
			opj_set_default_encoder_parameters(&lossless);

			// Set the right values for lossless encoding (based on examples in image_to_j2k.c)
			lossless.tcp_mct = 0;

			if (lossless.tcp_numlayers == 0) {
				lossless.tcp_rates[0] = 0;
				lossless.tcp_numlayers++;
				lossless.cp_disto_alloc = 1;
			}

			// Create filename string for residual image.
			char *lastDot = strrchr(compressedFile,'.');
			*lastDot = '\0';

			// Name is compressed file name (minus extension) + _RESIDUAL.jp2 - add enough space for terminating \n
			char residualFile[strlen(lastDot) + 15];

			sprintf(residualFile,"%s_RESIDUAL.jp2",compressedFile);

			// Restore end of file name of compressed file.
			*lastDot = '.';

			// Perform JPEG 2000 compression.
			int result = createJPEG2000Image(residualFile,CODEC_JP2,&lossless,&residualImage);

			// Exit unsuccessfully if compression unsuccessful.
			if (result != 0) {
				// Free memory for residual image.
				for (ii=0; ii<residualImage.numcomps; ii++) {
					free(residualImage.comps[ii].data);
				}
				free(residualImage.comps);

				fprintf(stderr,"Unable to compress residual image of file %s\n",compressedFile);
				opj_image_destroy(compressedImage);
				return 1;
			}
		}

		if (parameters->writeResidual) {
			// Free memory for residual image.
			for (ii=0; ii<residualImage.numcomps; ii++) {
				free(residualImage.comps[ii].data);
			}
			free(residualImage.comps);
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
