/**
 * @file f2j.c
 * @author Andrew Cannon
 * @date December 2011
 *
 * @brief Read FITS files and convert them into JPEG 2000.
 *
 * This file contains most of the custom source code for this
 * program.
 *
 * Note that this program assumes that int/float/etc widths are those on the
 * x86/x86_64 platforms.
 */

#include "f2j.h"

/**
 * Macro to perform the FITS read operation and then a specified transformation on
 * data from a FITS file.
 *
 * @param type primitive type, such as float or double, specifying the type of the array
 * which will contain data read from the FITS file.
 * @param fitstype CFITSIO constant, such as TFLOAT, specifying the type of the data
 * array that will contain the data read from the FITS file.
 * @param transformFunction Name of the  function used to transform raw input data from the
 * FITS file into output data.
 */
#define READ_AND_TRANSFORM(type,fitstype,transformFunction) { \
	type *imageArray = (type *) malloc(sizeof(type)*info->width*info->height);\
	\
	fits_read_pix(fptr,fitstype,fpixel,info->width*info->height,NULL,imageArray,NULL,status);\
	\
	if (*status != 0) {\
		fprintf(stderr,"Error reading frame %ld of image.\n",frame);\
		return 1;\
	}\
	int transformResult = transformFunction(imageArray,imageStruct->comps[0].data,transform,info->width*info->height,info->width);\
	\
	if (transformResult != 0) {\
		fprintf(stderr,"Specified transform could not be performed.\n");\
		return 1;\
	}\
	free(imageArray);\
}

/**
 * Displays usage information for f2j.  Exits with EXIT_FAILURE when finished.
 */
void displayHelp() {
	exit(EXIT_FAILURE);
}

/**
 * Function for transforming a raw array of data from a FITS file (in the form of
 * a long long int array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * @param rawData long long int array read from a FITS file using CFITSIO
 * @param imageData int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * @param transform transform to perform on each datum of rawData to get imageData.
 * @param len - length of rawData & imageData arrays.
 * @param width width of image.
 *
 * @return 0 if the transform could be performed successfully, 1 otherwise.
 */
int longLongImgTransform(long long int *rawData, int *imageData, transform transform, size_t len, size_t width) {
	fprintf(stderr,"This data type is not currently supported.\n");
	return 1;
}

/**
 * Function for transforming a raw array of data from a FITS file (in the form of
 * an int array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * @param rawData int array read from a FITS file using CFITSIO
 * @param imageData int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * @param transform transform to perform on each datum of rawData to get imageData.
 * @param len length of rawData & imageData arrays.
 * @param width width of image.
 *
 * @return 0 if the transform could be performed successfully, 1 otherwise.
 */
int intImgTransform(int *rawData, int *imageData, transform transform, size_t len, size_t width) {
	fprintf(stderr,"This data type is not currently supported.\n");
	return 1;
}

/**
 * Function for transforming a raw array of data from a FITS file (in the form of
 * an unsigned int array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * @param rawData unsigned int array read from a FITS file using CFITSIO
 * @param imageData int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * @param transform transform to perform on each datum of rawData to get imageData.
 * @param len length of rawData & imageData arrays.
 * @param width width of image.
 *
 * @return 0 if the transform could be performed successfully, 1 otherwise.
 */
int uIntImgTransform(unsigned int *rawData, int *imageData, transform transform, size_t len, size_t width) {
	fprintf(stderr,"This data type is not currently supported.\n");
	return 1;
}

/**
 * Function for transforming a raw array of data from a FITS file (in the form of
 * a short array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * Very basic parameter checking is performed, but the responsibility for checking
 * parameters are valid and meaningful is largely left to the calling function.
 *
 * @param rawData short array read from a FITS file using CFITSIO
 * @param imageData int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * @param transform transform to perform on each datum of rawData to get imageData.
 * @param len length of rawData & imageData arrays.
 * @param width width of image.
 *
 * @return 0 if the transform could be performed successfully, 1 otherwise.
 */
int shortImgTransform(short *rawData, int *imageData, transform transform, size_t len, size_t width) {
	if (rawData == NULL || imageData == NULL || len < 1) {
		fprintf(stderr,"Data arrays to shortImgTransform cannot be null or empty.\n");
		return 1;
	}

	// Loop variables
	size_t ii;

	if (transform == RAW) {
		// Variables that enable us to flip the image vertically as we read it in.
		size_t index = len-width;
		size_t dif = 0;

		// Shift scales (from signed to unsigned) then do a 1-1 mapping.
		for (ii=0; ii<len; ii++) {
			imageData[ii] = (int) rawData[index] + 32768;

			// Update index for vertical flipping.
			index++;
			dif++;

			if (dif >= width) {
				dif = 0;
				index -= 2*width;
			}
		}

		return 0;
	}
	else if (transform == NEGATIVE_RAW) {
		// Variables that enable us to flip the image vertically as we read it in.
		size_t index = len-width;
		size_t dif = 0;

		// As for linear, but subtract from 65535
		for (ii=0; ii<len; ii++) {
			imageData[ii] = 32767 - (int) rawData[index];

			// Update index for vertical flipping.
			index++;
			dif++;

			if (dif >= width) {
				dif = 0;
				index -= 2*width;
			}
		}

		return 0;
	}

	fprintf(stderr,"This transform is not currently supported for this data type.\n");
	return 1;
}

/**
 * Function for transforming a raw array of data from a FITS file (in the form of
 * an unsigned short array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * Very basic parameter checking is performed, but the responsibility for checking
 * parameters are valid and meaningful is largely left to the calling function.
 *
 * @param rawData unsigned short array read from a FITS file using CFITSIO
 * @param imageData int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * @param transform transform to perform on each datum of rawData to get imageData.
 * @param len length of rawData & imageData arrays.
 * @param width width of image.
 *
 * @return 0 if the transform could be performed successfully, 1 otherwise.
 */
int uShortImgTransform(unsigned short *rawData, int *imageData, transform transform, size_t len, size_t width) {
	if (rawData == NULL || imageData == NULL || len < 1) {
		fprintf(stderr,"Data arrays to uShortImgTransform cannot be null or empty.\n");
		return 1;
	}

	// Loop variables
	size_t ii;

	if (transform == RAW) {
		// Variables that enable us to flip the image vertically as we read it in.
		size_t index = len-width;
		size_t dif = 0;

		// Simple raw copying.
		for (ii=0; ii<len; ii++) {
			imageData[ii] = (int) rawData[index];

			// Update index for vertical flipping.
			index++;
			dif++;

			if (dif >= width) {
				dif = 0;
				index -= 2*width;
			}
		}

		return 0;
	}
	else if (transform == NEGATIVE_RAW) {
		// Variables that enable us to flip the image vertically as we read it in.
		size_t index = len-width;
		size_t dif = 0;

		// As for linear, but subtract from 65535
		for (ii=0; ii<len; ii++) {
			imageData[ii] = 65535 - (int) rawData[index];

			// Update index for vertical flipping.
			index++;
			dif++;

			if (dif >= width) {
				dif = 0;
				index -= 2*width;
			}
		}

		return 0;
	}

	fprintf(stderr,"This transform is not currently supported for this data type.\n");
	return 1;
}

/**
 * Function for transforming a raw array of data from a FITS file (in the form of
 * an unsigned char array) into grayscale image intensities (between 0 and 255 inclusive).
 *
 * Very basic parameter checking is performed, but the responsibility for checking
 * parameters are valid and meaningful is largely left to the calling function.
 *
 * @param rawData unsigned char array read from a FITS file using CFITSIO
 * @param imageData int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * @param transform transform to perform on each datum of rawData to get imageData.
 * @param len length of rawData & imageData arrays.
 * @param width width of image.
 *
 * @return 0 if the transform could be performed successfully, 1 otherwise.
 */
int byteImgTransform(unsigned char *rawData, int *imageData, transform transform, size_t len, size_t width) {
	if (rawData == NULL || imageData == NULL || len < 1) {
		fprintf(stderr,"Data arrays to byteImgTransform cannot be null or empty.\n");
		return 1;
	}

	// Loop variables
	size_t ii;

	if (transform == RAW) {
		// Variables that enable us to flip the image vertically as we read it in.
		size_t index = len-width;
		size_t dif = 0;

		// Simple raw transform
		for (ii=0; ii<len; ii++) {
			imageData[ii] = (int) rawData[index];

			// Update index for vertical flipping.
			index++;
			dif++;

			if (dif >= width) {
				dif = 0;
				index -= 2*width;
			}
		}

		return 0;
	}
	else if (transform == NEGATIVE_RAW) {
		// Variables that enable us to flip the image vertically as we read it in.
		size_t index = len-width;
		size_t dif = 0;

		// Invert raw transform
		for (ii=0; ii<len; ii++) {
			imageData[ii] = 255 - (int) rawData[index];

			// Update index for vertical flipping.
			index++;
			dif++;

			if (dif >= width) {
				dif = 0;
				index -= 2*width;
			}
		}

		return 0;
	}

	fprintf(stderr,"This transform is not currently supported for this data type.\n");
	return 1;
}

/**
 * Function for transforming a raw array of data from a FITS file (in the form of
 * a char array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * Very basic parameter checking is performed, but the responsibility for checking
 * parameters are valid and meaningful is largely left to the calling function.
 *
 * @param rawData signed char array read from a FITS file using CFITSIO
 * @param imageData int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * @param transform transform to perform on each datum of rawData to get imageData.
 * @param len length of rawData & imageData arrays.
 * @param width width of image.
 *
 * @return 0 if the transform could be performed successfully, 1 otherwise.
 */
int sByteImgTransform(signed char *rawData, int *imageData, transform transform, size_t len, size_t width) {
	if (rawData == NULL || imageData == NULL || len < 1) {
		fprintf(stderr,"Data arrays to sByteImgTransform cannot be null or empty.\n");
		return 1;
	}

	// Loop variables
	size_t ii;

	if (transform == RAW) {
		// Variables that enable us to flip the image vertically as we read it in.
		size_t index = len-width;
		size_t dif = 0;

		// Take raw data, shift it to be unsigned.
		for (ii=0; ii<len; ii++) {
			imageData[ii] = 128 + (int) rawData[index];

			// Update index for vertical flipping.
			index++;
			dif++;

			if (dif >= width) {
				dif = 0;
				index -= 2*width;
			}
		}

		return 0;
	}
	else if (transform == NEGATIVE_RAW) {
		// Variables that enable us to flip the image vertically as we read it in.
		size_t index = len-width;
		size_t dif = 0;

		// Invert raw transform.
		for (ii=0; ii<len; ii++) {
			imageData[ii] = 127 + (int) rawData[index];

			// Update index for vertical flipping.
			index++;
			dif++;

			if (dif >= width) {
				dif = 0;
				index -= 2*width;
			}
		}

		return 0;
	}

	fprintf(stderr,"This transform is not currently supported for this data type.\n");
	return 1;
}

/**
 * Function for transforming a raw array of data from a FITS file (in the form of
 * a double array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * Very basic parameter checking is performed, but the responsibility for checking that
 * parameters are valid and meaningful is largely left to the calling function.
 *
 * @param rawData double array read from a FITS file using CFITSIO
 * @param imageData int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * @param transform transform to perform on each datum of rawData to get imageData.
 * @param len length of rawData & imageData arrays.
 * @param datamin minimum value in rawData.
 * @param datamax maximum value in rawData.
 * @param width width of image.
 *
 * @return 0 if the transform could be performed successfully, 1 otherwise.
 */
int floatDoubleTransform(double *rawData, int *imageData, transform transform, size_t len, double datamin, double datamax, size_t width) {
	if (rawData == NULL || imageData == NULL || len < 1) {
		fprintf(stderr,"Data arrays in floatDoubleTransform cannot be null or empty.\n");
		return 1;
	}

	// Loop variables
	size_t ii;

	if (transform == LOG || transform == NEGATIVE_LOG) {
		double absMin = datamin;
		double zero = 0.0;

		if (datamin < 0.0) {
			absMin = -absMin;
			zero = 2*absMin;
		}
		else if (datamin <= 0.0) {
			absMin = 0.000001;
			zero = absMin;
		}

		double scale = 65535.0/log((datamax+zero)/absMin);

		// Variables that enable us to flip the image vertically as we read it in.
		size_t index = len-width;
		size_t dif = 0;

		for (ii=0; ii<len; ii++) {
			// Read the flipped image pixel.
			imageData[ii] = (int) (scale * log( (rawData[index] + zero) / absMin) );

			// Shouldn't get values outside this range, but just in case.
			if (imageData[ii] < 0) {
				imageData[ii] = 0;
			}
			else if (imageData[ii] > 65535) {
				imageData[ii] = 65535;
			}

			if (transform == NEGATIVE_LOG) {
				imageData[ii] = 65535 - imageData[ii];
			}

			// Update index for vertical flipping.
			index++;
			dif++;

			if (dif >= width) {
				dif = 0;
				index -= 2*width;
			}
		}

		return 0;
	}
	else if (transform == LINEAR || transform == NEGATIVE_LINEAR) {
		double absMin = datamin;
		double zero = 0.0;

		if (datamin < 0.0) {
			absMin = -absMin;
			zero = absMin;
		}

		double scale = 65535.0/(datamax+zero);

		// Variables that enable us to flip the image vertically as we read it in.
		size_t index = len-width;
		size_t dif = 0;

		for (ii=0; ii<len; ii++) {
			imageData[ii] = (int) (rawData[index] * scale);

			if (imageData[ii] < 0) {
				imageData[ii] = 0;
			}
			else if (imageData[ii] > 65535) {
				imageData[ii] = 65535;
			}

			if (transform == NEGATIVE_LINEAR) {
				imageData[ii] = 65535 - imageData[ii];
			}

			// Update index for vertical flipping.
			index++;
			dif++;

			if (dif >= width) {
				dif = 0;
				index -= 2*width;
			}
		}

		return 0;
	}

	fprintf(stderr,"This transform is not currently supported for this data type.\n");
	return 1;
}

/**
 * Function to open a FITS data file, check that it is a valid data cube that this program
 * may interpret and record some basic information about it.
 *
 * Basic parameter checking is performed, but the responsibility for checking that parameters
 * are valid and meaningful is largely left to the client code.
 *
 * @param ffname FITS file to open.
 * @param fptr Pointer to a pointer to a fitsfile datatype for CFITSIO. Will be populated by this
 * program, allowing it to be used in subsequent IO operations.
 * @param info Data structure containing information about the data cube, including width, length
 * and height.
 * @param status Reference to status variable used by CFITSIO.  Must have been initialised to
 * 0 by the time that this function is called.
 *
 * @return 0 if the read was successful or 1 otherwise.
 */
int getFITSInfo(char *ffname, fitsfile **fptr, cube_info *info, int *status) {
	// Check parameters.
	if (ffname == NULL || fptr == NULL || info == NULL || status == NULL) {
		fprintf(stderr,"Parameters to getFITSInfo cannot be null.\n");
		return 1;
	}

	// Loop variables
	int ii;

	// Declare variables used for reading FITS files using CFITSIO.
	int naxis; // Number of dimensions of image
	int bitpix; // Type of image

	// Open specified file for read only access.
	fits_open_file(fptr,ffname, READONLY, status);

	if (*status != 0) {
		fprintf(stderr,"Unable to open FITS file: %s\n",ffname);
		return 1;
	}

	// Get image type.
	fits_get_img_type(*fptr,&bitpix,status);
	// Get number of dimensions.
	fits_get_img_dim(*fptr,&naxis,status);

	if (*status != 0) {
		fprintf(stderr,"Unable to get image type or dimensions of FITS file: %s\n",ffname);
		return 1;
	}

	if (naxis<2) {
		fprintf(stderr,"Image must have at least 2 dimensions.\n");
		return 1;
	}

	// Save information on image type.
	info->bitpix = bitpix;
	info->naxis = naxis;

	// Get length of each dimension.
	long *naxes = (long *) malloc(sizeof(long) * naxis);
	fits_get_img_size(*fptr,naxis,naxes,status);

	if (*status != 0) {
		fprintf(stderr,"Unable to get image resolution of FITS file: %s\n",ffname);
		free(naxes);
		return 1;
	}

	// Record image dimensions.
	info->width = naxes[0];
	info->height = naxes[1];

	// Check if we are dealing with a three (or greater) dimensional image.
	// We can only deal with data cubes or images (2 or 3 dimensions).  Sometimes,
	// naxis is >3, but all the higher dimensions are 1.  In this case, we can interpret
	// this as a datacube.  Check that this is the case.
	if (naxis > 2) {
		info->depth = naxes[2];

		// Check higher dimensions are length one.
		for (ii=3; ii<naxis; ii++) {
			if (naxes[ii]>1) {
				fprintf(stderr,"Dimension %d in file %s has a length greater than 1.\n",ii+1,ffname);
				free(naxes);
				return 1;
			}
		}
	}

	free(naxes);
	return 0;
}

/**
 * Function to read a FITS file and create an OpenJPEG opj_image_t image (structure) from the data
 * read.
 *
 * @param fptr pointer to a CFITSIO fitsfile structure corresponding to a particular FITS file.  Must
 * have been opened using CFITSIO by the time this function is called.
 * @param transform transform to be performed on raw data from FITS file to create grayscale image intensities
 * for our output image.  See f2j.h for possible values.
 * @param imageStruct Reference to an image structure.  This function will populate most of the data values,
 * however, memory must have been assigned for the image data array (in the first component) by the time
 * that this function is called.
 * @param frame Plane of data to read for a 3D data cube.  Must be a valid frame number from 1 to [total number
 * of frames] inclusive.  Arbitrary for a 2D image.
 * @param info Pointer to a cube_info structure containing data on the image being read.
 * @param status Pointer to CFITSIO status integer.  The value must have been initialised to 0 by the time
 * that this function is called.
 *
 * @return 0 if there were no errors, 1 otherwise.
 */
int createImageFromFITS(fitsfile *fptr, transform transform, opj_image_t *imageStruct, long frame, cube_info *info, int *status) {
	// Check parameters.
	if (fptr == NULL || imageStruct == NULL || info == NULL || status == NULL) {
		fprintf(stderr,"Parameters to createImageFromFITS cannot be null.\n");
	}

	// Loop variables.
	int ii;
	size_t jj;

	// Check we have a valid frame if we are dealing with a data cube.  If we are dealing with a 2D FITS file,
	// the frame parameter is ignored.
	if (info->naxis > 2 && (frame<1 || frame>info->depth) ) {
		fprintf(stderr,"Specified frame must be between 1 and %ld.\n",info->depth);
	}

	// Write basic information about the image to be created.
	imageStruct->x0 = 0;
	imageStruct->x1 = info->width;
	imageStruct->y0 = 0;
	imageStruct->y1 = info->height;
	imageStruct->color_space = CLRSPC_GRAY; // Create a grayscale image.
	imageStruct->icc_profile_buf = NULL;
	imageStruct->icc_profile_len = 0;

	// Write basic information about the single image component.
	imageStruct->comps[0].bpp = 16; // Create a 16 bit grayscale image.
	imageStruct->comps[0].prec = 16;
	imageStruct->comps[0].dx = 1;
	imageStruct->comps[0].dy = 1;
	imageStruct->comps[0].factor = 0;
	imageStruct->comps[0].resno_decoded = 0;
	imageStruct->comps[0].w = info->width;
	imageStruct->comps[0].h = info->height;
	imageStruct->comps[0].sgnd = 0;
	imageStruct->comps[0].x0 = 0;
	imageStruct->comps[0].y0 = 0;

	// Create array used by CFITSIO to specify starting pixel to read from.
	long fpixel[info->naxis];

	// Start from the upper left corner of the image.
	fpixel[0] = 1;
	fpixel[1] = 1;

	// We need entries 2+ only if we are dealing with an image of > 2 dimensions.
	if (info->naxis>2) {
		fpixel[2] = frame;

		// For any dimension > 3, the width if always 1 if we are dealing with
		// a valid FITS file for this program.
		for (ii=3; ii<info->naxis; ii++) {
			fpixel[ii] = 1;
		}
	}

	// Different reading operations for each different image type.
	// 8 bit unsigned integer case
	if (info->bitpix == BYTE_IMG) {
		// Turn off scaling for this data stream if using raw data scales.
		if (transform == RAW || transform == NEGATIVE_RAW) {
			fits_set_bscale(fptr,1.0,0.0,status);
		}

		// We're only dealing with 8 bit data, so encode an 8 bit grayscale image.
		imageStruct->comps[0].bpp = 8;
		imageStruct->comps[0].prec = 8;

		READ_AND_TRANSFORM(unsigned char,TBYTE,byteImgTransform);
	}
	// 16 bit signed integer case
	else if (info->bitpix == SHORT_IMG) {
		// Turn off scaling for this data stream if using raw data scales.
		if (transform == RAW || transform == NEGATIVE_RAW) {
			fits_set_bscale(fptr,1.0,0.0,status);
		}

		READ_AND_TRANSFORM(short,TSHORT,shortImgTransform);
	}
	// 32 bit signed integer case
	else if (info->bitpix == LONG_IMG) {
		READ_AND_TRANSFORM(int,TLONG,intImgTransform);
	}
	// 64 bit signed integer case
	else if (info->bitpix == LONGLONG_IMG) {
		READ_AND_TRANSFORM(long long int,TLONGLONG,longLongImgTransform);
	}
	// 32/64 bit floating point case
	else if (info->bitpix == FLOAT_IMG || info->bitpix == DOUBLE_IMG) {
		// Do we need to find the max/min values?
		bool findMinMax = false;

		// Get min/max data values
		double datamax;
		double datamin;

		fits_read_key(fptr,TDOUBLE,"DATAMAX",&datamax,NULL,status);
		fits_read_key(fptr,TDOUBLE,"DATAMIN",&datamin,NULL,status);

		// Check if the DATAMAX/DATAMIN keywords were found in the header.  If they weren't,
		// we'll need to find them ourselves.
		if (*status != 0) {
			*status = 0;
			findMinMax = true;
		}

		double *imageArray = (double *) malloc(sizeof(double) * info->width * info->height);

		if (imageArray == NULL) {
			fprintf(stderr,"Unable to allocate memory to read FITS data.\n");
			return 1;
		}

		fits_read_pix(fptr,TDOUBLE,fpixel,info->width*info->height,NULL,imageArray,NULL,status);

		if (*status != 0) {
			fprintf(stderr,"Error reading frame %ld of image.\n",frame);
			return 1;
		}

		// Need to find min/max values if they weren't defined in the header.
		if (findMinMax) {
			// Small assumption here: that we have at least 1 pixel - does not seem unreasonable!
			datamax = imageArray[0];
			datamin = imageArray[0];

			// Search through array to find max/min values.
			for (jj=1; jj<info->width*info->height; jj++) {
				if (imageArray[jj] > datamax) {
					datamax = imageArray[jj];
				}

				if (imageArray[jj] < datamin) {
					datamin = imageArray[jj];
				}
			}
		}

		int transformResult = floatDoubleTransform(imageArray,imageStruct->comps[0].data,transform,info->width*info->height,datamin,datamax,info->width);

		if (transformResult != 0) {
			fprintf(stderr,"Specified transform could not be performed.\n");
			return 1;
		}

		free(imageArray);
	}
	// Signed char (8 bit integer) case
	else if (info->bitpix == SBYTE_IMG) {
		// Turn off scaling for this data stream if using raw data scales.
		if (transform == RAW || transform == NEGATIVE_RAW) {
			fits_set_bscale(fptr,1.0,0.0,status);
		}

		// We're only dealing with 8 bit data, so encode an 8 bit grayscale image.
		imageStruct->comps[0].bpp = 8;
		imageStruct->comps[0].prec = 8;

		READ_AND_TRANSFORM(signed char,TSBYTE,sByteImgTransform);
	}
	// Unsigned short (16 bit integer) case
	else if (info->bitpix == USHORT_IMG) {
		// Turn off scaling for this data stream if using raw data scales.
		if (transform == RAW || transform == NEGATIVE_RAW) {
			fits_set_bscale(fptr,1.0,0.0,status);
		}

		READ_AND_TRANSFORM(unsigned short,TUSHORT,uShortImgTransform);
	}
	// Unsigned 32 bit integer case
	else if (info->bitpix == ULONG_IMG) {
		READ_AND_TRANSFORM(unsigned int,TULONG,uIntImgTransform);
	}
	else {
		fprintf(stderr,"Unsupported FITS image type: %d\n",info->bitpix);
		return 1;
	}

	return 0;
}

/**
 * Encodes a specified image to a specified JPEG 2000 file.
 *
 * Basic parameter checking is performed, but the responsibility for ensuring parameters are
 * valid and meaningful is largely left to the calling function.
 *
 * @param outfile Name of JPEG 2000 image to create.  This file will be overwritten if it already
 * exists.
 * @param codec specified codec to use.  See <a href="http://www.openjpeg.org/libdoc/openjpeg_8h.html#a1d857738cef754699ffb79ddff48efbf">OpenJPEG documentation</a>
 * for legal values.
 * @param parameters compression parameters to use.
 * @param frame image to compress.
 *
 * @return 0 if compression was successful, 1 otherwise.
 */
int createJPEG2000Image(char *outfile, OPJ_CODEC_FORMAT codec, opj_cparameters_t *parameters, opj_image_t *frame) {
	if (outfile == NULL || parameters == NULL || frame == NULL) {
		fprintf(stderr,"Parameters to createJPEG2000Image cannot be null.\n");
		return 1;
	}

	// Write compressed image to file.  This code is based on that in image_to_j2k.c in the
	// OpenJPEG library.

	// Permissions for writing output JPEG 2000 file.  Currently write as binary file.
	char *writePermissions = "wb";

	// Get compressor handle using the specified codec.
	opj_cinfo_t *cinfo = opj_create_compress(codec);

	// Event manager object for error/warning/debug messages.
	opj_event_mgr_t event_mgr;
	memset(&event_mgr,0,sizeof(opj_event_mgr_t));

	// Catch events
	opj_set_event_mgr((opj_common_ptr)cinfo,&event_mgr,stderr);

	// IO stream for compression.
	opj_cio_t *cio = NULL;

	// Pointer for file to write to.
	FILE *f = NULL;

	// Setup encoder with the current frame and the specified parameters.
	opj_setup_encoder(cinfo,parameters,frame);

	// Open IO stream for compression.
	cio = opj_cio_open((opj_common_ptr)cinfo,NULL,0);

	// Was compression successful?
	opj_bool compSuccess;

	// Codestream information (needed for JPIP)
	opj_codestream_info_t cstr_info;

	// Perform compression and check if it was successful
	if (codec == CODEC_JP2 && parameters->jpip_on) {
		// See if we need to encode JPIP index information.
		compSuccess = opj_encode_with_info(cinfo,cio,frame,&cstr_info);
	}
	else {
		// Otherwise, encode without index information.
		compSuccess = opj_encode(cinfo,cio,frame,NULL);
	}

	// Exit unsuccessfully if compression unsuccessful.
	if (!compSuccess) {
		fprintf(stderr,"Unable to compress file %s.\n",outfile);
		opj_cio_close(cio);
		opj_destroy_compress(cinfo);
		return 1;
	}

	// Get length of codestream.
	int codestream_length = cio_tell(cio);

	// Open FILE handle.
	f = fopen(outfile,writePermissions);

	// Check that file was opened successfully.
	if (!f) {
		fprintf(stderr,"Unable to open output file: %s for writing.\n",outfile);
		opj_cio_close(cio);
		opj_destroy_compress(cinfo);
		if (codec == CODEC_JP2 && parameters->jpip_on) {
			opj_destroy_cstr_info(&cstr_info);
		}
		return 1;
	}

	// Write data to file.
	fwrite(cio->buffer,1,codestream_length,f);

	// Close file handle.
	fclose(f);

	// Close the IO stream.
	opj_cio_close(cio);

	// Free compression structures.
	opj_destroy_compress(cinfo);
	if (codec == CODEC_JP2 && parameters->jpip_on) {
		opj_destroy_cstr_info(&cstr_info);
	}

	return 0;
}

/**
 * Function to read a frame from a FITS data cube, create a grayscale image from it, then encode it as a JPEG 2000
 * image using lossy or lossless compression.
 *
 * The image is compressed using the parameters defined in parameters.  If writeUncompressed is true, a losslessly
 * compressed image will also be created.  If the parameters information passed to the function also describes lossless
 * compression, the two images will be the same, making the duplicate image redundant.  It is the responsibility of
 * client code to check this.
 *
 * This function performs basic checking that the parameters are not null, but in general, it is up to the client
 * code to ensure that parameters are valid and meaningful.
 *
 * @param info Reference to cube_info structure containing information on the data cube.
 * @param fptr Pointer to a fitsfile structure.  Assumed to be initialised by this point.
 * @param transform transform to perform when converting frame to image.
 * @param frameNumber Number of frame in 3D data cube.  Arbitrary for 2D images.
 * @param status Reference to status integer for CFITSIO.  Assumed to be initialised to 0 by this point.
 * @param outFileStub File name stub for JPEG 2000 image to be written.  Files will be STUB.jp2/j2k and STUB_LOSSLESS.jp2
 * (if writeUncompressed is true).
 * @param writeUncompressed Should a copy of the image be encoded using lossless compression.  May want to
 * do this to compare lossless VS lossy compression on an image.
 * @param parameters Compression parameters.
 *
 * @return 0 if all operations were successful, 1 otherwise.
 */
int setupCompression(cube_info *info, fitsfile *fptr, transform transform, long frameNumber, int *status, char *outFileStub,
		bool writeUncompressed, opj_cparameters_t *parameters) {
	// Check parameters
	if (info == NULL || fptr == NULL || status == NULL || outFileStub == NULL || parameters == NULL) {
		fprintf(stderr,"Parameters to setupCompression cannot be null.\n");
		return 1;
	}

	// Initialise an OpenJPEG image structure with a single component with data storage
	// initialised to the width and height of the image.

	// Create frame structure.
	opj_image_t frame;
	frame.comps = (opj_image_comp_t *) malloc(sizeof(opj_image_comp_t));
	frame.numcomps = 1;

	// Create component structure.
	frame.comps[0].data = (int *) malloc(sizeof(int)*info->width*info->height);

	// Could potentially specify other opj_image_t/opj_image_comp_t values here, but for flexibility,
	// they will be set in createImageFromFITS.  We don't want to get into the minutae of writing
	// image data at this point.

	// Create image
	int result = createImageFromFITS(fptr,transform,&frame,frameNumber,info,status);

	if (result != 0) {
		fprintf(stderr,"Unable to create image from frame %ld of FITS file.\n",frameNumber);
		free(frame.comps[0].data);
		free(frame.comps);
		return 1;
	}

	size_t stublen = strlen(outFileStub);

	if (writeUncompressed) {
		// Write uncompressed image to file.

		// Lossless compression codec.
		OPJ_CODEC_FORMAT losslessCodec = CODEC_JP2;

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

		// Create filename string for losslessly compressed file.
		// Name is STUB_LOSSLESS.jp2
		char losslessFile[stublen + 14];

		sprintf(losslessFile,"%s_LOSSLESS.jp2",outFileStub);

		// Perform JPEG 2000 compression.
		result = createJPEG2000Image(losslessFile,losslessCodec,&lossless,&frame);

		// Exit unsuccessfully if compression unsuccessful.
		if (result != 0) {
			fprintf(stderr,"Unable to compress frame %ld of FITS file.\n",frameNumber);
			free(frame.comps[0].data);
			free(frame.comps);
			return 1;
		}
	}

	// Write compressed image to file using specified compression parameters.

	// Get file name string.
	// Name is STUB.jp2
	char compressedFile[stublen + 5];

	if (parameters->cod_format == CODEC_JP2) {
		sprintf(compressedFile,"%s.jp2",outFileStub);
	}
	else {
		sprintf(compressedFile,"%s.j2k",outFileStub);
	}

	// Perform JPEG 2000 compression.
	result = createJPEG2000Image(compressedFile,parameters->cod_format,parameters,&frame);

	// Exit unsuccessfully if compression unsuccessful.
	if (result != 0) {
		fprintf(stderr,"Unable to compress frame %ld of FITS file.\n",frameNumber);
		free(frame.comps[0].data);
		free(frame.comps);
		return 1;
	}

	free(frame.comps[0].data);
	free(frame.comps);

	return 0;
}

/**
 * Main function run from the command line.
 */
int main(int argc, char *argv[]) {
	// Transform (if any) to perform on raw data.  This is a default value.  May be changed
	// when parsing user input from the command line.
	transform transform = LOG;

	// Should a lossless version of image be written?  By default, no.  May be changed
	// when parsing user input from the command line.
	bool writeUncompressed = false;

	// Structure to hold compression parameters.
	opj_cparameters_t parameters;

	// Initialise to default values.
	opj_set_default_encoder_parameters(&parameters);

	// Start frame - first frame of 3D data cube to read.  Ignored for 2D images.
	long startFrame = -1;

	// End frame - last frame of 3D data cube to read.  Ignored for 2D images.
	long endFrame = -1;

	// Parse command line parameters.
	int result = parse_cmdline_encoder(argc,argv,&parameters,&transform,&writeUncompressed,&startFrame,&endFrame);

	if (result != 0) {
		fprintf(stderr,"Error parsing command parameters.\n");
		displayHelp();
	}

	// image_to_j2k.c sets this to 1 if the image to be encoded has 3 components, or 0
	// otherwise.  We always set it to 0, as we are always encoding 1 component (grayscale)
	// images.
	parameters.tcp_mct = 0;

	// FITS file to read.
	char *ffname = parameters.infile;

	// Declare variables for reading FITS files needed by CFITSIO.
	fitsfile *fptr;
	int status = 0;

	// Loop variables
	long ii;

	// Information on the data cube
	cube_info info;

	// Read basic information on FITS file.
	result = getFITSInfo(ffname,&fptr,&info,&status);

	// Display error if FITS file could not be opened.
	if (result != 0) {
		fprintf(stderr,"FITS file %s cannot be opened or is invalid.\n",ffname);
		fits_close_file(fptr,&status);
		exit(EXIT_FAILURE);
	}

	// Eventually, we'll alter the compression parameters at this point.

	// Input file length
	size_t ilen = strlen(ffname);

	// Read each frame of the FITS file and compress it to JPEG 2000.
	// 2 dimensional image case
	if (info.naxis == 2) {
		// Output file will be input file name (minus FITS extension) + .JP2/J2K.
		// An additional 10 characters is sufficient for the additional data.
		char outFileStub[ilen+10];

		// Copy input file name to intermediary string.
		strcpy(outFileStub,ffname);

		// Get the last dot
		char *dotPosition = strrchr(outFileStub,'.');

		// Terminate the string at this point.
		*dotPosition = '\0';

		// Setup and perform compression.
		result = setupCompression(&info,fptr,transform,1,&status,outFileStub,writeUncompressed,&parameters);

		// Exit unsuccessfully if compression unsuccessful.
		if (result != 0) {
			fprintf(stderr,"Unable to compress frame %ld of file %s.\n",ii,ffname);
			fits_close_file(fptr,&status);
			exit(EXIT_FAILURE);
		}
	}
	else {
		// Valid start and end frames specified
		if (1<=startFrame && startFrame<=endFrame && endFrame<=info.depth) {
			// Do nothing, parameters already set.
		}
		// Valid start frame only - just read this one
		else if (1<=startFrame && startFrame<=info.depth) {
			endFrame = startFrame;
		}
		// If both specified start and end frames are invalid, read all frames.
		else {
			startFrame = 1;
			endFrame = info.depth;
		}

		for (ii=startFrame; ii<=endFrame; ii++) {
			// Setup and perform compression for this frame.  Each time the loop runs, memory for a new
			// image structure is allocatged as part of the setupCompression function.
			// If this code was being run in serial, we could save time by initialising the image structure
			// outside of the loop, to prevent a new memory allocation being performed every time the loop
			// ran.  However, if this code is to be parallelised, we want a separate memory allocation for
			// each frame of the image, to allow this process to be run in parallel.

			// Output file will be input file name (minus FITS extension) + _ + frame number + .JP2.
			// An additional 20 characters is sufficient for the additional data.
			size_t oflen = ilen + 20;

			char intermediate[oflen];
			char outFileStub[oflen];

			// Copy input file name to intermediary string.
			strcpy(intermediate,ffname);

			// Get the last dot
			char *dotPosition = strrchr(intermediate,'.');

			// Overwrite it with an underscore.
			*dotPosition = '_';
			*(dotPosition+1) = '\0';

			sprintf(outFileStub,"%s%ld",intermediate,ii);

			// Setup and perform compression.
			result = setupCompression(&info,fptr,transform,ii,&status,outFileStub,writeUncompressed,&parameters);

			// Exit unsuccessfully if compression unsuccessful.
			if (result != 0) {
				fprintf(stderr,"Unable to compress frame %ld of file %s.\n",ii,ffname);
				fits_close_file(fptr,&status);
				exit(EXIT_FAILURE);
			}
		}
	}

	// Close FITS file.
	fits_close_file(fptr, &status);

	exit(EXIT_SUCCESS);
}
