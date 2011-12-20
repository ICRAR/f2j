/*
 * f2j.c
 *
 * Read FITS files and convert them into JPEG 2000.
 *
 * Note that this program assumes that int/float/etc widths are those on the
 * x86/x86_64 platforms.
 *
 *  Created on: Dec 9, 2011
 *      Author: Andrew Cannon
 */

#include "f2j.h"

// Macro to perform the FITS read operation and then a specified transformation on
// data from a FITS file.
//
// type is a primitive type, such as float or double
// fitstype is the constant, such as TFLOAT, corresponding to type
// transformFunction is the function used to transform raw input data from the FITS file
// into output data.
#define READ_AND_TRANSFORM(type,fitstype,transformFunction) { \
	type imageArray[info->width*info->height];\
	\
	fits_read_pix(fptr,fitstype,fpixel,info->width*info->height,NULL,imageArray,NULL,status);\
	\
	if (*status != 0) {\
		fprintf(stderr,"Error reading frame %d of image.\n",frame);\
		return 1;\
	}\
	int transformResult = transformFunction(imageArray,imageStruct->comps[0].data,transform,info->width*info->height);\
	\
	if (transformResult != 0) {\
		fprintf(stderr,"Specified transform could not be performed.\n");\
		return 1;\
	}\
}

/*
 * Function for transforming a raw array of data from a FITS file (in the form of
 * a long long int array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * rawData - long long int array read from a FITS file using CFITSIO
 * imageData - int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * transform - transform to perform on each datum of rawData to get imageData.
 * len - length of rawData & imageData arrays.
 *
 * Returns 0 if the transform could be performed successfully, 1 otherwise.
 */
int longLongImgTransform(long long int *rawData, int *imageData, transform transform, int len) {
	fprintf(stderr,"This data type is not currently supported.\n");
	return 1;
}

/*
 * Function for transforming a raw array of data from a FITS file (in the form of
 * an int array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * rawData - int array read from a FITS file using CFITSIO
 * imageData - int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * transform - transform to perform on each datum of rawData to get imageData.
 * len - length of rawData & imageData arrays.
 *
 * Returns 0 if the transform could be performed successfully, 1 otherwise.
 */
int intImgTransform(int *rawData, int *imageData, transform transform, int len) {
	fprintf(stderr,"This data type is not currently supported.\n");
	return 1;
}

/*
 * Function for transforming a raw array of data from a FITS file (in the form of
 * an unsigned int array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * rawData - unsigned int array read from a FITS file using CFITSIO
 * imageData - int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * transform - transform to perform on each datum of rawData to get imageData.
 * len - length of rawData & imageData arrays.
 *
 * Returns 0 if the transform could be performed successfully, 1 otherwise.
 */
int uIntImgTransform(unsigned int *rawData, int *imageData, transform transform, int len) {
	fprintf(stderr,"This data type is not currently supported.\n");
	return 1;
}

/*
 * Function for transforming a raw array of data from a FITS file (in the form of
 * a short array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * rawData - short array read from a FITS file using CFITSIO
 * imageData - int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * transform - transform to perform on each datum of rawData to get imageData.
 * len - length of rawData & imageData arrays.
 *
 * Returns 0 if the transform could be performed successfully, 1 otherwise.
 */
int shortImgTransform(short *rawData, int *imageData, transform transform, int len) {
	// Loop variables
	int ii;

	if (transform == LINEAR) {
		// Shift scales (from signed to unsigned) then do a 1-1 mapping.
		for (ii=0; ii<len; ii++) {
			imageData[ii] = (int) rawData[ii] + 32768;
		}

		return 0;
	}
	else if (transform == NEGATIVE_LINEAR) {
		// As for linear, but subtract from 65535
		for (ii=0; ii<len; ii++) {
			imageData[ii] = 32767 - (int) rawData[ii];
		}
	}
	else {
		fprintf(stderr,"This data type is not currently supported.\n");
		return 1;
	}
}

/*
 * Function for transforming a raw array of data from a FITS file (in the form of
 * an unsigned short array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * rawData - unsigned short array read from a FITS file using CFITSIO
 * imageData - int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * transform - transform to perform on each datum of rawData to get imageData.
 * len - length of rawData & imageData arrays.
 *
 * Returns 0 if the transform could be performed successfully, 1 otherwise.
 */
int uShortImgTransform(unsigned short *rawData, int *imageData, transform transform, int len) {
	fprintf(stderr,"This data type is not currently supported.\n");
	return 1;
}

/*
 * Function for transforming a raw array of data from a FITS file (in the form of
 * an unsigned char array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * rawData - unsigned char array read from a FITS file using CFITSIO
 * imageData - int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * transform - transform to perform on each datum of rawData to get imageData.
 * len - length of rawData & imageData arrays.
 *
 * Returns 0 if the transform could be performed successfully, 1 otherwise.
 */
int byteImgTransform(unsigned char *rawData, int *imageData, transform transform, int len) {
	fprintf(stderr,"This data type is not currently supported.\n");
	return 1;
}

/*
 * Function for transforming a raw array of data from a FITS file (in the form of
 * a char array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * rawData - char array read from a FITS file using CFITSIO
 * imageData - int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * transform - transform to perform on each datum of rawData to get imageData.
 * len - length of rawData & imageData arrays.
 *
 * Returns 0 if the transform could be performed successfully, 1 otherwise.
 */
int sByteImgTransform(char *rawData, int *imageData, transform transform, int len) {
	fprintf(stderr,"This data type is not currently supported.\n");
	return 1;
}

/*
 * Function for transforming a raw array of data from a FITS file (in the form of
 * a double array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * rawData - double array read from a FITS file using CFITSIO
 * imageData - int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * transform - transform to perform on each datum of rawData to get imageData.
 * len - length of rawData & imageData arrays.
 *
 * Returns 0 if the transform could be performed successfully, 1 otherwise.
 */
int doubleImgTransform(double *rawData, int *imageData, transform transform, int len) {
	fprintf(stderr,"This data type is not currently supported.\n");
	return 1;
}

/*
 * Function for transforming a raw array of data from a FITS file (in the form of
 * a float array) into grayscale image intensities (between 0 and 2^16-1 inclusive).
 *
 * rawData - float array read from a FITS file using CFITSIO
 * imageData - int array, assumed to be the same length as rawData, to be populated
 * with grayscale image intensities.
 * transform - transform to perform on each datum of rawData to get imageData.
 * len - length of rawData & imageData arrays.
 *
 * Returns 0 if the transform could be performed successfully, 1 otherwise.
 */
int floatImgTransform(float *rawData, int *imageData, transform transform, int len) {
	if (rawData == NULL || imageData == NULL) {
		fprintf(stderr,"IO arrays cannot be null.\n");
		return 1;
	}

	// LOG transform
	if (transform == LOG) {
		fprintf(stderr,"Specified transform not supported at this time.\n");
		return 1;
	}
	// LINEAR transform
	else if (transform == LINEAR) {
		fprintf(stderr,"Specified transform not supported at this time.\n");
		return 1;
	}
	// Display error when a specified transform is not supported.
	else {
		fprintf(stderr,"Specified transform not supported.\n");
		return 1;
	}

	return 0;
}

/*
 * Function to open a FITS data file, check that it is a valid data cube that this program
 * may interpret and record some basic information about it.
 *
 * ffname: FITS file to open.
 * fptr: Pointer to a pointer to a fitsfile datatype for CFITSIO. Will be populated by this
 * program, allowing it to be used in subsequent IO operations.
 * info: Data structure containing information about the data cube, including width, length
 * and height.
 * status: Reference to status variable used by CFITSIO.  Must have been initialised to
 * 0 by the time that this function is called.
 *
 * Returns 0 if the read was successful or 1 otherwise.
 */
int getFITSInfo(char *ffname, fitsfile **fptr, cube_info *info, int *status) {
	// Check parameters.
	if (ffname == NULL || fptr == NULL || info == NULL) {
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

/*
 * Function to read a FITS file and create an OpenJPEG opj_image_t image (structure) from the data
 * read.
 *
 * Parameter:
 * fptr - pointer to a CFITSIO fitsfile structure corresponding to a particular FITS file.  Must
 * have been opened using CFITSIO by the time this function is called.
 * transform - transform to be performed on raw data from FITS file to create grayscale image intensities
 * for our output image.  See f2j.h for possible values.
 * imageStruct - Reference to an image structure.  This function will populate most of the data values,
 * however, memory must have been assigned for the image data array (in the first component) by the time
 * that this function is called.
 * frame - Plane of data to read for a 3D data cube.
 * info - Pointer to a cube_info structure (see f2j.h) containing data on the image being read.
 * status - Pointer to CFITSIO status integer.  The value must have been initialised to 0 by the time
 * that this function is called.
 *
 * Returns 0 if there were no errors, 1 otherwise.
 */
int createImageFromFITS(fitsfile *fptr, transform transform, opj_image_t *imageStruct, int frame, cube_info *info, int *status) {
	// Check parameters.
	if (imageStruct == NULL || info == NULL || status == NULL) {
		fprintf(stderr,"Parameters to createImageFromFITS cannot be null.\n");
	}

	// Loop variables.
	int ii;

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
		READ_AND_TRANSFORM(unsigned char,TBYTE,byteImgTransform);
	}
	// 16 bit signed integer case
	else if (info->bitpix == SHORT_IMG) {
		if (*status != 0) {
								fprintf(stderr,"NPrior error reading frame %d of image.\n",frame);
								return 1;
							}
		// Turn off scaling for this data stream.
		fits_set_bscale(fptr,1.0,0.0,status);


		if (*status != 0) {
						fprintf(stderr,"FPrior error reading frame %d of image.\n",frame);
						return 1;
					}

			short imageArray[info->width*info->height];

			fits_read_pix(fptr,TSHORT,fpixel,info->width*info->height,NULL,imageArray,NULL,status);

			if (*status != 0) {
				fprintf(stderr,"XError reading frame %d of image.\n",frame);
				return 1;
			}
			int transformResult = shortImgTransform(imageArray,imageStruct->comps[0].data,transform,info->width*info->height);

			if (transformResult != 0) {
				fprintf(stderr,"Specified transform could not be performed.\n");
				return 1;
			}

		//XREAD_AND_TRANSFORM(short,TSHORT,shortImgTransform);
	}
	// 32 bit signed integer case
	else if (info->bitpix == LONG_IMG) {
		READ_AND_TRANSFORM(int,TLONG,intImgTransform);
	}
	// 64 bit signed integer case
	else if (info->bitpix == LONGLONG_IMG) {
		READ_AND_TRANSFORM(long long int,TLONGLONG,longLongImgTransform);
	}
	// 32 bit floating point case
	else if (info->bitpix == FLOAT_IMG) {
		READ_AND_TRANSFORM(float,TFLOAT,floatImgTransform);
	}
	// 64 bit floating point case
	else if (info->bitpix == DOUBLE_IMG) {
		READ_AND_TRANSFORM(double,TDOUBLE,doubleImgTransform);
	}
	// Signed char (8 bit integer) case
	else if (info->bitpix == SBYTE_IMG) {
		READ_AND_TRANSFORM(char,TSBYTE,sByteImgTransform);
	}
	// Unsigned short (16 bit integer) case
	else if (info->bitpix == USHORT_IMG) {
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

int main(int argc, char *argv[]) {
	// FITS file to read.  Eventually, we will take this as a parameter.
	char *ffname = "//Users//acannon//Downloads//FITS//H100_abcde_luther_chop.fits";

	// Transform (if any) to perform on raw data.  Eventually, we'll take this as a parameter.
	transform transform = NEGATIVE_LINEAR;

	// Should the intermediate image be written to a (losslessly compressed) image?
	// May need to compare lossy & lossless compression.  Eventually, we'll take this
	// as a parameter.
	bool writeUncompressed = false;

	// Declare variables for reading FITS files needed by CFITSIO.
	fitsfile *fptr;
	int status = 0;

	// Loop variables
	int ii;

	// Information on the data cube
	cube_info info;

	// Read basic information on FITS file.
	int result = getFITSInfo(ffname,&fptr,&info,&status);

	// Display error if FITS file could not be opened.
	if (result != 0) {
		fprintf(stderr,"FITS file %s cannot be opened or is invalid.\n",ffname);
		fits_close_file(fptr,&status);
		exit(EXIT_FAILURE);
	}

	// Read each frame of the FITS file and compress it to JPEG 2000.
	// 2 dimensional image case
	if (info.naxis == 2) {
		// Create frame structure.
		opj_image_t frame;
		opj_image_comp_t components[1];
		frame.comps = components;
		frame.numcomps = 1;

		// Create component structure.
		int image_data[info.width*info.height];
		frame.comps[0].data = image_data;

		// Create image
		result = createImageFromFITS(fptr,transform,&frame,1,&info,&status);

		if (result != 0) {
			fprintf(stderr,"Unable to create image from FITS file %s.\n",ffname);
			fits_close_file(fptr,&status);
			exit(EXIT_FAILURE);
		}

		if (writeUncompressed) {
			// Write uncompressed image to file.
		}
	}
	else {
		for (ii=1; ii<=info.depth; ii++) {
			// Initialise an OpenJPEG image structure with a single component with data storage
			// initialised to the width and height of the image.
			// If this code was being run in serial, we could save time by initialising the image structure
			// outside of the loop, to prevent a new memory allocation being performed every time the loop
			// ran.  However, if this code is to be parallelised, we want a separate memory allocation for
			// each frame of the image, to allow this process to be run in parallel.

			// Create frame structure.
			opj_image_t frame;
			opj_image_comp_t components[1];
			frame.comps = components;
			frame.numcomps = 1;

			// Create component structure.
			int image_data[info.width*info.height];
			frame.comps[0].data = image_data;

			// Could potentially specify other opj_image_t/opj_image_comp_t values here, but for flexibility,
			// they will be set in createImageFromFITS.  We don't want to get into the minutae of writing
			// image data at this point.

			// Create image
			result = createImageFromFITS(fptr,transform,&frame,ii,&info,&status);

			if (result != 0) {
				fprintf(stderr,"Unable to create image from frame %d of FITS file %s.\n",ii,ffname);
				exit(EXIT_FAILURE);
			}

			if (writeUncompressed) {
				// Write uncompressed image to file.
			}

			// Write compressed image to file.  This code is based on that in image_to_j2k.c in the
			// OpenJPEG library.

			// Permissions for writing output JPEG 2000 file.  Currently write as binary file.
			char *writePermissions = "wb";

			// Output file will be input file name (minus FITS extension) + _ + frame number + .JP2.
			// An additional 20 characters is sufficient for the additional data.
			int oflen = strlen(ffname) + 20;

			char outputFileName[oflen];
			char fileStub[oflen];

			// Copy input file name to intermediary stub.
			strcpy(fileStub,ffname);

			// Get the last dot
			char *dotPosition = strrchr(fileStub,'.');

			// Overwrite it with an underscore.
			*dotPosition = '_';
			*(dotPosition+1) = '\0';

			sprintf(outputFileName,"%s%d.jp2",fileStub,ii);

			// Codec format to use.  Eventually, probably take this as a user parameter.
			// Currently set to JPEG2000 file format.  Possible values available at:
			// http://www.openjpeg.org/libdoc/openjpeg_8h.html#a1d857738cef754699ffb79ddff48efbf
			OPJ_CODEC_FORMAT codec = CODEC_JP2;

			// Structure to hold compression parameters.
			opj_cparameters_t parameters;
			// Initialise to default values.
			opj_set_default_encoder_parameters(&parameters);

			// Set the right values for lossless encoding (based on examples in image_to_j2k.c)
			parameters.tcp_mct = 0;

			if (parameters.tcp_numlayers == 0) {
				parameters.tcp_rates[0] = 0;
				parameters.tcp_numlayers++;
				parameters.cp_disto_alloc = 1;
			}

			// Eventually, we'll alter the compression parameters at this point.

			// Get compressor handle using the specified codec.
			opj_cinfo_t *cinfo = opj_create_compress(codec);

			// Event manager object for error/warning/debug messages.
			opj_event_mgr_t event_mgr;

			// Catch events
			opj_set_event_mgr((opj_common_ptr)cinfo,&event_mgr,stderr);

			// IO stream for compression.
			opj_cio_t *cio = NULL;

			// Pointer for file to write to.
			FILE *f = NULL;

			// Setup encoder with the current frame and the specified parameters.
			opj_setup_encoder(cinfo,&parameters,&frame);

			// Open IO stream for compression.
			cio = opj_cio_open((opj_common_ptr)cinfo,NULL,0);

			// Was compression successful?
			bool compSuccess = opj_encode(cinfo,cio,&frame,NULL);

			// Exit unsuccessfully if compression unsuccessful.
			if (!compSuccess) {
				fprintf(stderr,"Unable to compress frame %d of file %s.\n",ii,ffname);
				opj_cio_close(cio);
				fits_close_file(fptr,&status);
				exit(EXIT_FAILURE);
			}

			// Get length of codestream.
			int codestream_length = cio_tell(cio);

			// Open FILE handle.
			f = fopen(outputFileName,writePermissions);

			// Check that file was opened successfully.
			if (!f) {
				fprintf(stderr,"Unable to open output file: %s for writing.\n",outputFileName);
				opj_cio_close(cio);
				fits_close_file(fptr,&status);
				exit(EXIT_FAILURE);
			}

			// Write data to file.
			fwrite(cio->buffer,1,codestream_length,f);

			// Close file handle.
			fclose(f);

			// Close the IO stream.
			opj_cio_close(cio);

			// Free compression structures.
			opj_destroy_compress(cinfo);
		}
	}

	// Close FITS file.
	fits_close_file(fptr, &status);

	exit(EXIT_SUCCESS);
}
