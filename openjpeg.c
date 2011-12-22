/**
 * @file openjpeg.c
 * @author Andrew Cannon
 * @date December 2011
 *
 * @brief Functions in f2j that are largely taken from the OpenJPEG library.
 *
 * They are modified here for our purposes.
 */

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

/* ------------------------------------------------------------------------------------ */

#include "f2j.h"

#include "getopt.h"
#include "format_defs.h"

/**
 * Taken from directly image_to_j2k.c in OpenJPEG.  See <a href="http://www.openjpeg.org/libdoc/index.html">OpenJPEG documentation</a>
 * for more information.
 */
OPJ_PROG_ORDER give_progression(char progression[4]) {
	if(strncmp(progression, "LRCP", 4) == 0) {
		return LRCP;
	}
	if(strncmp(progression, "RLCP", 4) == 0) {
		return RLCP;
	}
	if(strncmp(progression, "RPCL", 4) == 0) {
		return RPCL;
	}
	if(strncmp(progression, "PCRL", 4) == 0) {
		return PCRL;
	}
	if(strncmp(progression, "CPRL", 4) == 0) {
		return CPRL;
	}

	return PROG_UNKNOWN;
}

/**
 * Command line parser.  Takes command line parameters, parses them and encodes an
 * opj_cparameters_t object with the specified compression parameters.  Extracted, with
 * minor modifications, from image_to_j2k.c.  Uses most of the command line prefixes from
 * image_to_j2k.c in OpenJPEG.  Also defines 3 additional command line parameters, A,
 * specifying how the raw data in the FITS file should be transformed, L specifying whether
 * a lossless version of the image should be encoded in addition to the lossy version and
 * m, allowing the user to specify only a single frame in a data cube to be processed.
 *
 * @param argc Number of command line arguments.
 * @param argv Command line arguments.
 * @param parameters Compression parameters structure.  Assumed to have been populated
 * with default values by the time this function is called.  This function will configure
 * parameters based on the command line options specified.
 * @param transform Reference to the transform to be performed on the raw data.  This will
 * be updated if a transform is specified on the command line.
 * @param writeUncompressed Reference to boolean specifying if a lossless version of image
 * should be written.  This will be set to true if the L parameter is present on the command
 * line.
 *
 * @return 0 if parsing was successful, 1 otherwise.
 */
int parse_cmdline_encoder(int argc, char **argv, opj_cparameters_t *parameters, transform *transform, bool *writeUncompressed) {
	int i, j,totlen;
	option_t long_option[]={
		{"ImgDir",REQ_ARG, NULL ,'z'},
		{"TP",REQ_ARG, NULL ,'v'},
		{"SOP",NO_ARG, NULL ,'S'},
		{"EPH",NO_ARG, NULL ,'E'},
		{"OutFor",REQ_ARG, NULL ,'O'},
		{"POC",REQ_ARG, NULL ,'P'},
		{"ROI",REQ_ARG, NULL ,'R'},
	};

	/* parse the command line */
	const char optlist[] = "i:o:r:q:n:b:c:t:p:s:SEM:R:d:T:If:P:C:F:L:A:m:"
#ifdef USE_JPWL
		"W:"
#endif /* USE_JPWL */
		"h";

	totlen=sizeof(long_option);

	// Use JP2 if no output format is specified.
	parameters->cod_format = CODEC_JP2;

	while (1) {
    int c = getopt_long(argc, argv, optlist,long_option,totlen);
		if (c == -1)
			break;
		switch (c) {
			/* Not in OpenJPEG version - should a lossless version be written? */
			case 'L':
			{
				*writeUncompressed = true;
			}
			break;

			/* What transform should be performed on the raw FITS data? */
			case 'A':
			{
				// This should be synced with the possibilities in f2j.h.
				char *transformString = optarg;

				if (strcasecmp(transformString,"LOG") == 0) {
					*transform = LOG;
				}
				else if (strcasecmp(transformString,"NEGATIVE_LOG") == 0) {
					*transform = NEGATIVE_LOG;
				}
				else if (strcasecmp(transformString,"LINEAR") == 0) {
					*transform = LINEAR;
				}
				else if (strcasecmp(transformString,"NEGATIVE_LINEAR") == 0) {
					*transform = NEGATIVE_LINEAR;
				}
				else if (strcasecmp(transformString,"RAW") == 0) {
					*transform = RAW;
				}
				else if (strcasecmp(transformString,"NEGATIVE_RAW") == 0) {
					*transform = NEGATIVE_RAW;
				}
				else {
					fprintf(stderr,"Unknown transform specified: %s.  Using default instead.\n",optarg);
				}
			}
 			break;

			/* Largely as in OpenJPEG below */
			case 'i':			/* input file */
			{
				char *infile = optarg;
				strncpy(parameters->infile, infile, sizeof(parameters->infile)-1);
			}
			break;

				/* ----------------------------------------------------- */

			case 'o':			/* output format - either J2K or JP2 - differs from image_to_j2k.c */
			{
				char *outfile = optarg;

				if (strcasecmp(outfile,"JP2") == 0) {
					parameters->cod_format = CODEC_JP2;
				}
				else if (strcasecmp(outfile,"J2K") == 0) {
					parameters->cod_format = CODEC_J2K;
				}
				else {
					// Use JP2 if format is not specified.
					parameters->cod_format = CODEC_JP2;
				}
			}
			break;

				/* ----------------------------------------------------- */


			case 'r':			/* rates rates/distorsion */
			{
				char *s = optarg;
				parameters->tcp_numlayers = 0;
				while (sscanf(s, "%f", &parameters->tcp_rates[parameters->tcp_numlayers]) == 1) {
					parameters->tcp_numlayers++;
					while (*s && *s != ',') {
						s++;
					}
					if (!*s)
						break;
					s++;
				}
				parameters->cp_disto_alloc = 1;
			}
			break;

				/* ----------------------------------------------------- */

			case 'q':			/* add fixed_quality */
			{
				char *s = optarg;
				while (sscanf(s, "%f", &parameters->tcp_distoratio[parameters->tcp_numlayers]) == 1) {
					parameters->tcp_numlayers++;
					while (*s && *s != ',') {
						s++;
					}
					if (!*s)
						break;
					s++;
				}
				parameters->cp_fixed_quality = 1;
			}
			break;

				/* dda */
				/* ----------------------------------------------------- */

			case 'f':			/* mod fixed_quality (before : -q) */
			{
				int *row = NULL, *col = NULL;
				int numlayers = 0, numresolution = 0, matrix_width = 0;

				char *s = optarg;
				sscanf(s, "%d", &numlayers);
				s++;
				if (numlayers > 9)
					s++;

				parameters->tcp_numlayers = numlayers;
				numresolution = parameters->numresolution;
				matrix_width = numresolution * 3;
				parameters->cp_matrice = (int *) malloc(numlayers * matrix_width * sizeof(int));
				s = s + 2;

				for (i = 0; i < numlayers; i++) {
					row = &parameters->cp_matrice[i * matrix_width];
					col = row;
					parameters->tcp_rates[i] = 1;
					sscanf(s, "%d,", &col[0]);
					s += 2;
					if (col[0] > 9)
						s++;
					col[1] = 0;
					col[2] = 0;
					for (j = 1; j < numresolution; j++) {
						col += 3;
						sscanf(s, "%d,%d,%d", &col[0], &col[1], &col[2]);
						s += 6;
						if (col[0] > 9)
							s++;
						if (col[1] > 9)
							s++;
						if (col[2] > 9)
							s++;
					}
					if (i < numlayers - 1)
						s++;
				}
				parameters->cp_fixed_alloc = 1;
			}
			break;

				/* ----------------------------------------------------- */

			case 't':			/* tiles */
			{
				sscanf(optarg, "%d,%d", &parameters->cp_tdx, &parameters->cp_tdy);
				parameters->tile_size_on = true;
			}
			break;

				/* ----------------------------------------------------- */

			case 'n':			/* resolution */
			{
				sscanf(optarg, "%d", &parameters->numresolution);
			}
			break;

				/* ----------------------------------------------------- */
			case 'c':			/* precinct dimension */
			{
				char sep;
				int res_spec = 0;

				char *s = optarg;
				do {
					sep = 0;
					sscanf(s, "[%d,%d]%c", &parameters->prcw_init[res_spec],
                                 &parameters->prch_init[res_spec], &sep);
					parameters->csty |= 0x01;
					res_spec++;
					s = strpbrk(s, "]") + 2;
				}
				while (sep == ',');
				parameters->res_spec = res_spec;
			}
			break;

				/* ----------------------------------------------------- */

			case 'b':			/* code-block dimension */
			{
				int cblockw_init = 0, cblockh_init = 0;
				sscanf(optarg, "%d,%d", &cblockw_init, &cblockh_init);
				if (cblockw_init * cblockh_init > 4096 || cblockw_init > 1024
					|| cblockw_init < 4 || cblockh_init > 1024 || cblockh_init < 4) {
					fprintf(stderr,
						"!! Size of code_block error (option -b) !!\n\nRestriction :\n"
            "    * width*height<=4096\n    * 4<=width,height<= 1024\n\n");
					return 1;
				}
				parameters->cblockw_init = cblockw_init;
				parameters->cblockh_init = cblockh_init;
			}
			break;

				/* ----------------------------------------------------- */

			case 'p':			/* progression order */
			{
				char progression[4];

				strncpy(progression, optarg, 4);
				parameters->prog_order = give_progression(progression);
				if (parameters->prog_order == -1) {
					fprintf(stderr, "Unrecognized progression order "
            "[LRCP, RLCP, RPCL, PCRL, CPRL] !!\n");
					return 1;
				}
			}
			break;

				/* ----------------------------------------------------- */

			case 's':			/* subsampling factor */
			{
				if (sscanf(optarg, "%d,%d", &parameters->subsampling_dx,
                                    &parameters->subsampling_dy) != 2) {
					fprintf(stderr,	"'-s' sub-sampling argument error !  [-s dx,dy]\n");
					return 1;
				}
			}
			break;

				/* ----------------------------------------------------- */

			case 'd':			/* coordonnate of the reference grid */
			{
				if (sscanf(optarg, "%d,%d", &parameters->image_offset_x0,
                                    &parameters->image_offset_y0) != 2) {
					fprintf(stderr,	"-d 'coordonnate of the reference grid' argument "
            "error !! [-d x0,y0]\n");
					return 1;
				}
			}
			break;

				/* ----------------------------------------------------- */

			case 'h':			/* display an help description */
				displayHelp();
				return 1;

				/* ----------------------------------------------------- */

			case 'P':			/* POC */
			{
				int numpocs = 0;		/* number of progression order change (POC) default 0 */
				opj_poc_t *POC = NULL;	/* POC : used in case of Progression order change */

				char *s = optarg;
				POC = parameters->POC;

				while (sscanf(s, "T%d=%d,%d,%d,%d,%d,%4s", &POC[numpocs].tile,
					&POC[numpocs].resno0, &POC[numpocs].compno0,
					&POC[numpocs].layno1, &POC[numpocs].resno1,
					&POC[numpocs].compno1, POC[numpocs].progorder) == 7) {
					POC[numpocs].prg1 = give_progression(POC[numpocs].progorder);
					numpocs++;
					while (*s && *s != '/') {
						s++;
					}
					if (!*s) {
						break;
					}
					s++;
				}
				parameters->numpocs = numpocs;
			}
			break;

				/* ------------------------------------------------------ */

			case 'S':			/* SOP marker */
			{
				parameters->csty |= 0x02;
			}
			break;

				/* ------------------------------------------------------ */

			case 'E':			/* EPH marker */
			{
				parameters->csty |= 0x04;
			}
			break;

				/* ------------------------------------------------------ */

			case 'M':			/* Mode switch pas tous au point !! */
			{
				int value = 0;
				if (sscanf(optarg, "%d", &value) == 1) {
					for (i = 0; i <= 5; i++) {
						int cache = value & (1 << i);
						if (cache)
							parameters->mode |= (1 << i);
					}
				}
			}
			break;

				/* ------------------------------------------------------ */

			case 'R':			/* ROI */
			{
				if (sscanf(optarg, "c=%d,U=%d", &parameters->roi_compno,
                                           &parameters->roi_shift) != 2) {
					fprintf(stderr, "ROI error !! [-ROI c='compno',U='shift']\n");
					return 1;
				}
			}
			break;

				/* ------------------------------------------------------ */

			case 'T':			/* Tile offset */
			{
				if (sscanf(optarg, "%d,%d", &parameters->cp_tx0, &parameters->cp_ty0) != 2) {
					fprintf(stderr, "-T 'tile offset' argument error !! [-T X0,Y0]");
					return 1;
				}
			}
			break;

				/* ------------------------------------------------------ */

			case 'C':			/* add a comment */
			{
				parameters->cp_comment = (char*)malloc(strlen(optarg) + 1);
				if(parameters->cp_comment) {
					strcpy(parameters->cp_comment, optarg);
				}
			}
			break;


				/* ------------------------------------------------------ */

			case 'I':			/* reversible or not */
			{
				parameters->irreversible = 1;
			}
			break;

			/* ------------------------------------------------------ */

			case 'v':			/* Tile part generation*/
			{
				parameters->tp_flag = optarg[0];
				parameters->tp_on = 1;
			}
			break;

				/* ------------------------------------------------------ */

			// Removed Cinema 2K & 4K - not needed.

/* UniPG>> */
#ifdef USE_JPWL
				/* ------------------------------------------------------ */

			case 'W':			/* JPWL capabilities switched on */
			{
				char *token = NULL;
				int hprot, pprot, sens, addr, size, range;

				/* we need to enable indexing */
				if (!indexfilename || !*indexfilename) {
					strncpy(indexfilename, JPWL_PRIVATEINDEX_NAME, OPJ_PATH_LEN);
				}

				/* search for different protection methods */

				/* break the option in comma points and parse the result */
				token = strtok(optarg, ",");
				while(token != NULL) {

					/* search header error protection method */
					if (*token == 'h') {

						static int tile = 0, tilespec = 0, lasttileno = 0;

						hprot = 1; /* predefined method */

						if(sscanf(token, "h=%d", &hprot) == 1) {
							/* Main header, specified */
							if (!((hprot == 0) || (hprot == 1) || (hprot == 16) || (hprot == 32) ||
								((hprot >= 37) && (hprot <= 128)))) {
								fprintf(stderr, "ERROR -> invalid main header protection method h = %d\n", hprot);
								return 1;
							}
							parameters->jpwl_hprot_MH = hprot;

						} else if(sscanf(token, "h%d=%d", &tile, &hprot) == 2) {
							/* Tile part header, specified */
							if (!((hprot == 0) || (hprot == 1) || (hprot == 16) || (hprot == 32) ||
								((hprot >= 37) && (hprot <= 128)))) {
								fprintf(stderr, "ERROR -> invalid tile part header protection method h = %d\n", hprot);
								return 1;
							}
							if (tile < 0) {
								fprintf(stderr, "ERROR -> invalid tile part number on protection method t = %d\n", tile);
								return 1;
							}
							if (tilespec < JPWL_MAX_NO_TILESPECS) {
								parameters->jpwl_hprot_TPH_tileno[tilespec] = lasttileno = tile;
								parameters->jpwl_hprot_TPH[tilespec++] = hprot;
							}

						} else if(sscanf(token, "h%d", &tile) == 1) {
							/* Tile part header, unspecified */
							if (tile < 0) {
								fprintf(stderr, "ERROR -> invalid tile part number on protection method t = %d\n", tile);
								return 1;
							}
							if (tilespec < JPWL_MAX_NO_TILESPECS) {
								parameters->jpwl_hprot_TPH_tileno[tilespec] = lasttileno = tile;
								parameters->jpwl_hprot_TPH[tilespec++] = hprot;
							}


						} else if (!strcmp(token, "h")) {
							/* Main header, unspecified */
							parameters->jpwl_hprot_MH = hprot;

						} else {
							fprintf(stderr, "ERROR -> invalid protection method selection = %s\n", token);
							return 1;
						};

					}

					/* search packet error protection method */
					if (*token == 'p') {

						static int pack = 0, tile = 0, packspec = 0;

						pprot = 1; /* predefined method */

						if (sscanf(token, "p=%d", &pprot) == 1) {
							/* Method for all tiles and all packets */
							if (!((pprot == 0) || (pprot == 1) || (pprot == 16) || (pprot == 32) ||
								((pprot >= 37) && (pprot <= 128)))) {
								fprintf(stderr, "ERROR -> invalid default packet protection method p = %d\n", pprot);
								return 1;
							}
							parameters->jpwl_pprot_tileno[0] = 0;
							parameters->jpwl_pprot_packno[0] = 0;
							parameters->jpwl_pprot[0] = pprot;

						} else if (sscanf(token, "p%d=%d", &tile, &pprot) == 2) {
							/* method specified from that tile on */
							if (!((pprot == 0) || (pprot == 1) || (pprot == 16) || (pprot == 32) ||
								((pprot >= 37) && (pprot <= 128)))) {
								fprintf(stderr, "ERROR -> invalid packet protection method p = %d\n", pprot);
								return 1;
							}
							if (tile < 0) {
								fprintf(stderr, "ERROR -> invalid tile part number on protection method p = %d\n", tile);
								return 1;
							}
							if (packspec < JPWL_MAX_NO_PACKSPECS) {
								parameters->jpwl_pprot_tileno[packspec] = tile;
								parameters->jpwl_pprot_packno[packspec] = 0;
								parameters->jpwl_pprot[packspec++] = pprot;
							}

						} else if (sscanf(token, "p%d:%d=%d", &tile, &pack, &pprot) == 3) {
							/* method fully specified from that tile and that packet on */
							if (!((pprot == 0) || (pprot == 1) || (pprot == 16) || (pprot == 32) ||
								((pprot >= 37) && (pprot <= 128)))) {
								fprintf(stderr, "ERROR -> invalid packet protection method p = %d\n", pprot);
								return 1;
							}
							if (tile < 0) {
								fprintf(stderr, "ERROR -> invalid tile part number on protection method p = %d\n", tile);
								return 1;
							}
							if (pack < 0) {
								fprintf(stderr, "ERROR -> invalid packet number on protection method p = %d\n", pack);
								return 1;
							}
							if (packspec < JPWL_MAX_NO_PACKSPECS) {
								parameters->jpwl_pprot_tileno[packspec] = tile;
								parameters->jpwl_pprot_packno[packspec] = pack;
								parameters->jpwl_pprot[packspec++] = pprot;
							}

						} else if (sscanf(token, "p%d:%d", &tile, &pack) == 2) {
							/* default method from that tile and that packet on */
							if (!((pprot == 0) || (pprot == 1) || (pprot == 16) || (pprot == 32) ||
								((pprot >= 37) && (pprot <= 128)))) {
								fprintf(stderr, "ERROR -> invalid packet protection method p = %d\n", pprot);
								return 1;
							}
							if (tile < 0) {
								fprintf(stderr, "ERROR -> invalid tile part number on protection method p = %d\n", tile);
								return 1;
							}
							if (pack < 0) {
								fprintf(stderr, "ERROR -> invalid packet number on protection method p = %d\n", pack);
								return 1;
							}
							if (packspec < JPWL_MAX_NO_PACKSPECS) {
								parameters->jpwl_pprot_tileno[packspec] = tile;
								parameters->jpwl_pprot_packno[packspec] = pack;
								parameters->jpwl_pprot[packspec++] = pprot;
							}

						} else if (sscanf(token, "p%d", &tile) == 1) {
							/* default from a tile on */
							if (tile < 0) {
								fprintf(stderr, "ERROR -> invalid tile part number on protection method p = %d\n", tile);
								return 1;
							}
							if (packspec < JPWL_MAX_NO_PACKSPECS) {
								parameters->jpwl_pprot_tileno[packspec] = tile;
								parameters->jpwl_pprot_packno[packspec] = 0;
								parameters->jpwl_pprot[packspec++] = pprot;
							}


						} else if (!strcmp(token, "p")) {
							/* all default */
							parameters->jpwl_pprot_tileno[0] = 0;
							parameters->jpwl_pprot_packno[0] = 0;
							parameters->jpwl_pprot[0] = pprot;

						} else {
							fprintf(stderr, "ERROR -> invalid protection method selection = %s\n", token);
							return 1;
						};

					}

					/* search sensitivity method */
					if (*token == 's') {

						static int tile = 0, tilespec = 0, lasttileno = 0;

						sens = 0; /* predefined: relative error */

						if(sscanf(token, "s=%d", &sens) == 1) {
							/* Main header, specified */
							if ((sens < -1) || (sens > 7)) {
								fprintf(stderr, "ERROR -> invalid main header sensitivity method s = %d\n", sens);
								return 1;
							}
							parameters->jpwl_sens_MH = sens;

						} else if(sscanf(token, "s%d=%d", &tile, &sens) == 2) {
							/* Tile part header, specified */
							if ((sens < -1) || (sens > 7)) {
								fprintf(stderr, "ERROR -> invalid tile part header sensitivity method s = %d\n", sens);
								return 1;
							}
							if (tile < 0) {
								fprintf(stderr, "ERROR -> invalid tile part number on sensitivity method t = %d\n", tile);
								return 1;
							}
							if (tilespec < JPWL_MAX_NO_TILESPECS) {
								parameters->jpwl_sens_TPH_tileno[tilespec] = lasttileno = tile;
								parameters->jpwl_sens_TPH[tilespec++] = sens;
							}

						} else if(sscanf(token, "s%d", &tile) == 1) {
							/* Tile part header, unspecified */
							if (tile < 0) {
								fprintf(stderr, "ERROR -> invalid tile part number on sensitivity method t = %d\n", tile);
								return 1;
							}
							if (tilespec < JPWL_MAX_NO_TILESPECS) {
								parameters->jpwl_sens_TPH_tileno[tilespec] = lasttileno = tile;
								parameters->jpwl_sens_TPH[tilespec++] = hprot;
							}

						} else if (!strcmp(token, "s")) {
							/* Main header, unspecified */
							parameters->jpwl_sens_MH = sens;

						} else {
							fprintf(stderr, "ERROR -> invalid sensitivity method selection = %s\n", token);
							return 1;
						};

						parameters->jpwl_sens_size = 2; /* 2 bytes for default size */
					}

					/* search addressing size */
					if (*token == 'a') {


						addr = 0; /* predefined: auto */

						if(sscanf(token, "a=%d", &addr) == 1) {
							/* Specified */
							if ((addr != 0) && (addr != 2) && (addr != 4)) {
								fprintf(stderr, "ERROR -> invalid addressing size a = %d\n", addr);
								return 1;
							}
							parameters->jpwl_sens_addr = addr;

						} else if (!strcmp(token, "a")) {
							/* default */
							parameters->jpwl_sens_addr = addr; /* auto for default size */

						} else {
							fprintf(stderr, "ERROR -> invalid addressing selection = %s\n", token);
							return 1;
						};

					}

					/* search sensitivity size */
					if (*token == 'z') {


						size = 1; /* predefined: 1 byte */

						if(sscanf(token, "z=%d", &size) == 1) {
							/* Specified */
							if ((size != 0) && (size != 1) && (size != 2)) {
								fprintf(stderr, "ERROR -> invalid sensitivity size z = %d\n", size);
								return 1;
							}
							parameters->jpwl_sens_size = size;

						} else if (!strcmp(token, "a")) {
							/* default */
							parameters->jpwl_sens_size = size; /* 1 for default size */

						} else {
							fprintf(stderr, "ERROR -> invalid size selection = %s\n", token);
							return 1;
						};

					}

					/* search range method */
					if (*token == 'g') {


						range = 0; /* predefined: 0 (packet) */

						if(sscanf(token, "g=%d", &range) == 1) {
							/* Specified */
							if ((range < 0) || (range > 3)) {
								fprintf(stderr, "ERROR -> invalid sensitivity range method g = %d\n", range);
								return 1;
							}
							parameters->jpwl_sens_range = range;

						} else if (!strcmp(token, "g")) {
							/* default */
							parameters->jpwl_sens_range = range;

						} else {
							fprintf(stderr, "ERROR -> invalid range selection = %s\n", token);
							return 1;
						};

					}

					/* next token or bust */
					token = strtok(NULL, ",");
				};


				/* some info */
				fprintf(stdout, "Info: JPWL capabilities enabled\n");
				parameters->jpwl_epc_on = true;

			}
			break;
#endif /* USE_JPWL */
/* <<UniPG */

				/* ------------------------------------------------------ */

			default:
				fprintf(stderr, "ERROR -> Command line not valid\n");
				return 1;
		}
	}

	/* check for possible errors */
	if (parameters->cp_cinema){
		if(parameters->tcp_numlayers > 1){
			parameters->cp_rsiz = STD_RSIZ;
     	fprintf(stdout,"Warning: DC profiles do not allow more than one quality layer. The codestream created will not be compliant with the DC profile\n");
		}
	}

	// CHECK THIS POINT
	// We don't need an outfile (we use a default encoding format), only an input file
	if((parameters->infile[0] == 0)) {
		fprintf(stderr, "No input file specified - Example: %s -i image.fits\n",argv[0]);
		fprintf(stderr, "    Try: %s -h\n",argv[0]);
		return 1;
	}

	if ((parameters->cp_disto_alloc || parameters->cp_fixed_alloc || parameters->cp_fixed_quality)
		&& (!(parameters->cp_disto_alloc ^ parameters->cp_fixed_alloc ^ parameters->cp_fixed_quality))) {
		fprintf(stderr, "Error: options -r -q and -f cannot be used together !!\n");
		return 1;
	}				/* mod fixed_quality */

	/* if no rate entered, lossless by default */
	if (parameters->tcp_numlayers == 0) {
		parameters->tcp_rates[0] = 0;	/* MOD antonin : losslessbug */
		parameters->tcp_numlayers++;
		parameters->cp_disto_alloc = 1;

		// No need to write lossless version twice.
		*writeUncompressed = false;
	}

	if((parameters->cp_tx0 > parameters->image_offset_x0) || (parameters->cp_ty0 > parameters->image_offset_y0)) {
		fprintf(stderr,
			"Error: Tile offset dimension is unnappropriate --> TX0(%d)<=IMG_X0(%d) TYO(%d)<=IMG_Y0(%d) \n",
			parameters->cp_tx0, parameters->image_offset_x0, parameters->cp_ty0, parameters->image_offset_y0);
		return 1;
	}

	for (i = 0; i < parameters->numpocs; i++) {
		if (parameters->POC[i].prg == -1) {
			fprintf(stderr,
				"Unrecognized progression order in option -P (POC n %d) [LRCP, RLCP, RPCL, PCRL, CPRL] !!\n",
				i + 1);
		}
	}

	return 0;
}
