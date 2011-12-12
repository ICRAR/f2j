/*
 * f2j.c
 *
 * Early prototype to read FITS file and write to JPEG 2000.
 *
 *  Created on: Dec 9, 2011
 *      Author: Andrew Cannon
 */

#include <stdio.h>
#include <stdlib.h>
#include "fitsio.h"

int main(int argc, char *argv[]) {
	// Declare variables
	fitsfile *fptr;
	int status = 0;
	int naxis;
	int i;

	// Open file for read only access
	fits_open_file(&fptr,"//Users//acannon//Downloads//FITS//H100_abcde_luther_chop.fits", READONLY, &status);

	// Code for reading in image type (currently unused).
	/*
	fits_get_img_type(fptr,&bitpix,&status);

	switch (bitpix) {
		case BYTE_IMG: fprintf(stdout,"BYTE_IMG\n"); break;
		case SHORT_IMG: fprintf(stdout,"SHORT_IMG\n"); break;
		case LONG_IMG: fprintf(stdout,"LONG_IMG\n"); break;
		case FLOAT_IMG: fprintf(stdout,"FLOAT_IMG\n"); break;
		case DOUBLE_IMG: fprintf(stdout,"DOUBLE_IMG\n"); break;
		default: fprintf(stdout,"Unknown type\n"); break;
	}*/

	// Get image dimension and resolution
	fits_get_img_dim(fptr,&naxis,&status);

	long naxes[3];

	fits_get_img_size(fptr,3,naxes,&status);

	short int blank;

	// Code for reading BSCALE/BZERO values as floats.  Unnecessary.
/*	float bsd;
	float bzd;

	fits_read_key(fptr,TFLOAT,"BSCALE",(void*)&bsd,NULL,&status);
	fits_read_key(fptr,TFLOAT,"BZERO",(void*)&bzd,NULL,&status);*/

	fits_read_key(fptr,TSHORT,"BLANK",(void*)&blank,NULL,&status);

	float ia[naxes[0]*naxes[1]];

	int anynul;

	long fpixel[] = {1L,1L,1L};

	// Read image
	fits_read_pix(fptr,TFLOAT,fpixel,naxes[0]*naxes[1],(void*)&blank,ia,&anynul,&status);

	for (i=0; i<naxes[0]*naxes[1]; i++) {
		if (i%naxes[0] == 0) {
			fprintf(stdout,"\n");
		}

		fprintf(stdout," %+6f ",ia[i]);
	}

	// Code for reading in BSCALE/BZERO values as text.  Unnecessary as the library handles reading the variables.
/*	char bs[BUFSIZ];
	char bz[BUFSIZ];

	fits_read_card(fptr,"BSCALE",bs,&status);
	fits_read_card(fptr,"BZERO",bz,&status);

	fprintf(stdout,"BSCALE: %f, BZERO: %f, BLANK: %d\n",bsd,bzd,blank);
*/
	exit(EXIT_SUCCESS);
}
