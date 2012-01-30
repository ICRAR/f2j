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

#include "opj_getopt.h"
#include "opj_getopt.c" /* Slightly unorthordox to import a source rather than header file,
but we need the code, and it's better not to place an unmodified source file in our project
if we can import it from elsewhere.  */
#include "format_defs.h"

/**
 * Display usage information for image_to_j2k command line parameters that are used
 * by f2j.  Largely taken from image_to_j2k.c.
 *
 * This function provides specific information on JPEG 2000 compression options.  The
 * displayHelp() in f2j.c provides general program information beyond JPEG 2000 compression
 * options.
 */
void encode_help_display() {
/* UniPG>> */
	fprintf(stdout,"List of parameters for the JPEG 2000 "
#ifdef USE_JPWL
		"+ JPWL "
#endif /* USE_JPWL */
		"encoder:\n");
/* <<UniPG */
	fprintf(stdout,"\n");
	fprintf(stdout,"REMARKS:\n");
	fprintf(stdout,"---------\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"The markers written to the main_header are : SOC SIZ COD QCD COM.\n");
	fprintf(stdout,"COD and QCD never appear in the tile_header.\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"By default:\n");
	fprintf(stdout,"------------\n");
	fprintf(stdout,"\n");
	fprintf(stdout," * Lossless\n");
	fprintf(stdout," * 1 tile\n");
	fprintf(stdout," * Size of precinct : 2^15 x 2^15 (means 1 precinct)\n");
	fprintf(stdout," * Size of code-block : 64 x 64\n");
	fprintf(stdout," * Number of resolutions: 6\n");
	fprintf(stdout," * No SOP marker in the codestream\n");
	fprintf(stdout," * No EPH marker in the codestream\n");
	fprintf(stdout," * No sub-sampling in x or y direction\n");
	fprintf(stdout," * No mode switch activated\n");
	fprintf(stdout," * Progression order: LRCP\n");
	fprintf(stdout," * No ROI upshifted\n");
	fprintf(stdout," * No offset of the origin of the image\n");
	fprintf(stdout," * No offset of the origin of the tiles\n");
	fprintf(stdout," * Reversible DWT 5-3\n");
	fprintf(stdout," * No JPIP index information written to the image\n"); // Addition relative to image_to_j2k.c.
/* UniPG>> */
#ifdef USE_JPWL
	fprintf(stdout," * No JPWL protection\n");
#endif /* USE_JPWL */
/* <<UniPG */
	fprintf(stdout,"\n");
	fprintf(stdout,"JPEG 2000 Compression Parameters (all optional):\n");
	fprintf(stdout,"------------------------------------------------\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-r           : different compression ratios for successive layers (-r 20,10,5)\n ");
	fprintf(stdout,"	         - The rate specified for each quality level is the desired \n");
	fprintf(stdout,"	           compression factor.\n");
	fprintf(stdout,"		   Example: -r 20,10,1 means quality 1: compress 20x, \n");
	fprintf(stdout,"		     quality 2: compress 10x and quality 3: compress lossless\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"               (options -r and -q cannot be used together)\n ");
	fprintf(stdout,"\n");

	fprintf(stdout,"-q           : different psnr for successive layers (-q 30,40,50) \n ");

	fprintf(stdout,"               (options -r and -q cannot be used together)\n ");

	fprintf(stdout,"\n");
	fprintf(stdout,"-n           : number of resolutions (-n 3) \n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-b           : size of code block (-b 32,32) \n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-c           : size of precinct (-c 128,128) \n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-t           : size of tile (-t 512,512) \n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-p           : progression order (-p LRCP) [LRCP, RLCP, RPCL, PCRL, CPRL] \n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-s           : subsampling factor (-s 2,2) [-s X,Y] \n");
	fprintf(stdout,"	     Remark: subsampling bigger than 2 can produce error\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-POC         : Progression order change (-POC T1=0,0,1,5,3,CPRL/T1=5,0,1,6,3,CPRL) \n");
	fprintf(stdout,"      Example: T1=0,0,1,5,3,CPRL \n");
	fprintf(stdout,"			 : Ttilenumber=Resolution num start,Component num start,Layer num end,Resolution num end,Component num end,Progression order\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-SOP         : write SOP marker before each packet \n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-EPH         : write EPH marker after each header packet \n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-M           : mode switch (-M 3) [1=BYPASS(LAZY) 2=RESET 4=RESTART(TERMALL)\n");
	fprintf(stdout,"                 8=VSC 16=ERTERM(SEGTERM) 32=SEGMARK(SEGSYM)] \n");
	fprintf(stdout,"                 Indicate multiple modes by adding their values. \n");
	fprintf(stdout,"                 ex: RESTART(4) + RESET(2) + SEGMARK(32) = -M 38\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-TP          : devide packets of every tile into tile-parts (-TP R) [R, L, C]\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-ROI         : c=%%d,U=%%d : quantization indices upshifted \n");
	fprintf(stdout,"               for component c=%%d [%%d = 0,1,2]\n");
	fprintf(stdout,"               with a value of U=%%d [0 <= %%d <= 37] (i.e. -ROI c=0,U=25) \n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-d           : offset of the origin of the image (-d 150,300) \n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-T           : offset of the origin of the tiles (-T 100,75) \n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-I           : use the irreversible DWT 9-7 (-I) \n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-F           : characteristics of the raw input image\n");
	fprintf(stdout,"               -F rawWidth,rawHeight,rawComp,rawBitDepth,s/u (Signed/Unsigned)\n");
	fprintf(stdout,"               Example: -i lena.raw -o lena.j2k -F 512,512,3,8,u\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"-jpip        : write jpip codestream index box in JP2 output file\n");
	fprintf(stdout,"               NOTICE: currently supports only RPCL order\n");
	fprintf(stdout,"\n");
/* UniPG>> */
#ifdef USE_JPWL
	fprintf(stdout,"-W           : adoption of JPWL (Part 11) capabilities (-W params)\n");
	fprintf(stdout,"               The parameters can be written and repeated in any order:\n");
	fprintf(stdout,"               [h<tilepart><=type>,s<tilepart><=method>,a=<addr>,...\n");
	fprintf(stdout,"                ...,z=<size>,g=<range>,p<tilepart:pack><=type>]\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"                 h selects the header error protection (EPB): 'type' can be\n");
	fprintf(stdout,"                   [0=none 1,absent=predefined 16=CRC-16 32=CRC-32 37-128=RS]\n");
	fprintf(stdout,"                   if 'tilepart' is absent, it is for main and tile headers\n");
	fprintf(stdout,"                   if 'tilepart' is present, it applies from that tile\n");
	fprintf(stdout,"                     onwards, up to the next h<> spec, or to the last tilepart\n");
	fprintf(stdout,"                     in the codestream (max. %d specs)\n", JPWL_MAX_NO_TILESPECS);
	fprintf(stdout,"\n");
	fprintf(stdout,"                 p selects the packet error protection (EEP/UEP with EPBs)\n");
	fprintf(stdout,"                  to be applied to raw data: 'type' can be\n");
	fprintf(stdout,"                   [0=none 1,absent=predefined 16=CRC-16 32=CRC-32 37-128=RS]\n");
	fprintf(stdout,"                   if 'tilepart:pack' is absent, it is from tile 0, packet 0\n");
	fprintf(stdout,"                   if 'tilepart:pack' is present, it applies from that tile\n");
	fprintf(stdout,"                     and that packet onwards, up to the next packet spec\n");
	fprintf(stdout,"                     or to the last packet in the last tilepart in the stream\n");
	fprintf(stdout,"                     (max. %d specs)\n", JPWL_MAX_NO_PACKSPECS);
	fprintf(stdout,"\n");
	fprintf(stdout,"                 s enables sensitivity data insertion (ESD): 'method' can be\n");
	fprintf(stdout,"                   [-1=NO ESD 0=RELATIVE ERROR 1=MSE 2=MSE REDUCTION 3=PSNR\n");
	fprintf(stdout,"                    4=PSNR INCREMENT 5=MAXERR 6=TSE 7=RESERVED]\n");
	fprintf(stdout,"                   if 'tilepart' is absent, it is for main header only\n");
	fprintf(stdout,"                   if 'tilepart' is present, it applies from that tile\n");
	fprintf(stdout,"                     onwards, up to the next s<> spec, or to the last tilepart\n");
	fprintf(stdout,"                     in the codestream (max. %d specs)\n", JPWL_MAX_NO_TILESPECS);
	fprintf(stdout,"\n");
	fprintf(stdout,"                 g determines the addressing mode: <range> can be\n");
	fprintf(stdout,"                   [0=PACKET 1=BYTE RANGE 2=PACKET RANGE]\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"                 a determines the size of data addressing: <addr> can be\n");
	fprintf(stdout,"                   2/4 bytes (small/large codestreams). If not set, auto-mode\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"                 z determines the size of sensitivity values: <size> can be\n");
	fprintf(stdout,"                   1/2 bytes, for the transformed pseudo-floating point value\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"                 ex.:\n");
	fprintf(stdout,"                   h,h0=64,h3=16,h5=32,p0=78,p0:24=56,p1,p3:0=0,p3:20=32,s=0,\n");
	fprintf(stdout,"                     s0=6,s3=-1,a=0,g=1,z=1\n");
	fprintf(stdout,"                 means\n");
	fprintf(stdout,"                   predefined EPB in MH, rs(64,32) from TPH 0 to TPH 2,\n");
	fprintf(stdout,"                   CRC-16 in TPH 3 and TPH 4, CRC-32 in remaining TPHs,\n");
	fprintf(stdout,"                   UEP rs(78,32) for packets 0 to 23 of tile 0,\n");
	fprintf(stdout,"                   UEP rs(56,32) for packs. 24 to the last of tilepart 0,\n");
	fprintf(stdout,"                   UEP rs default for packets of tilepart 1,\n");
	fprintf(stdout,"                   no UEP for packets 0 to 19 of tilepart 3,\n");
	fprintf(stdout,"                   UEP CRC-32 for packs. 20 of tilepart 3 to last tilepart,\n");
	fprintf(stdout,"                   relative sensitivity ESD for MH,\n");
	fprintf(stdout,"                   TSE ESD from TPH 0 to TPH 2, byte range with automatic\n");
	fprintf(stdout,"                   size of addresses and 1 byte for each sensitivity value\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"                 ex.:\n");
	fprintf(stdout,"                       h,s,p\n");
	fprintf(stdout,"                 means\n");
	fprintf(stdout,"                   default protection to headers (MH and TPHs) as well as\n");
	fprintf(stdout,"                   data packets, one ESD in MH\n");
	fprintf(stdout,"\n");
	fprintf(stdout,"                 N.B.: use the following recommendations when specifying\n");
	fprintf(stdout,"                       the JPWL parameters list\n");
	fprintf(stdout,"                   - when you use UEP, always pair the 'p' option with 'h'\n");
	fprintf(stdout,"                 \n");
#endif /* USE_JPWL */
/* <<UniPG */
}

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
 * image_to_j2k.c in OpenJPEG.  Also defines additional command line parameters detailed
 * below.
 *
 * @param argc Number of command line arguments.
 * @param argv Command line arguments.
 * @param parameters Compression parameters structure.  Assumed to have been populated
 * with default values by the time this function is called.  This function will configure
 * parameters based on the command line options specified.
 * @param transform Reference to the transform to be performed on the raw data.  This will
 * be updated if a transform is specified on the command line using the parameter A.
 * @param writeUncompressed Reference to boolean specifying if a lossless version of image
 * should be written.  This will be set to true if the LL parameter is present on the command
 * line.
 * @param startFrame First frame of data cube to read.  Ignored for 2D images.  Will only be
 * modified if the x parameter is present.  If only x is specified (and not y), the single x
 * value will be interpreted as a single frame to read.
 * @param endFrame Last frame of data cube to read.  Ignored for 2D images.  Will only be
 * modified if the y parameter is present.
 * @param benchmarkQualityParameters Reference to quality_benchmark_info structure specifying what,
 * if any quality benchmark tests to be performed.  Assumed to be initialised to 'no benchmarks'
 * before this function is called.  If the QB command line option is present, all benchmarks will
 * be performed.  This will override any other quality parameters.  In the absence of this parameter,
 * individual benchmarks may be turned on by specifying QB_FID for fidelity, QB_PSNR for peak signal
 * to noise ratio, QB_MAD for maximum absolute distortion, QB_MSE for mean square error, QB_RMSE for
 * root mean square error, QB_MAE for mean absolute error, QB_SE for squared error, QB_AE for absolute
 * error and QB_SI for sum of uncompressed squared image intensities.  QB_RES specifies if a residual
 * image should be written.
 * @param performCompressionBenchmarking Reference to boolean specifying if compression benchmarking
 * should be performed on the images being compressed.  This will be set to true if the CB parameter
 * is present on the command line.
 * @param firstStoke First stoke of data volume to read.  Ignored for 2D or 3D images.  Will only be
 * modified if the S1 parameter is present.  If only S1 is specified (and not S2), the single S1
 * value will be interpreted as a single stoke to read.
 * @param lastStoke Last stoke of data volume to read.  Ignored for 2D or 3D images.  Will only be
 * modified if the S2 parameter is present.
 *
 * @return 0 if parsing was successful, 1 otherwise.
 */
int parse_cmdline_encoder(int argc, char **argv, opj_cparameters_t *parameters, transform *transform, bool *writeUncompressed,
		long *startFrame, long *endFrame, quality_benchmark_info *benchmarkQualityParameters, bool *performCompressionBenchmarking,
		long *firstStoke, long *lastStoke) {
	int i,j,totlen,c;
	opj_option_t long_option[]={
		{"ImgDir",REQ_ARG, NULL ,'z'},
		{"TP",REQ_ARG, NULL ,'u'},
		{"SOP",NO_ARG, NULL ,'S'},
		{"EPH",NO_ARG, NULL ,'E'},
		{"OutFor",REQ_ARG, NULL ,'O'},
		{"POC",REQ_ARG, NULL ,'P'},
		{"ROI",REQ_ARG, NULL ,'R'},
		{"jpip",NO_ARG, NULL, 'J'},
		{"S1",REQ_ARG, NULL ,'a'},
		{"S2",REQ_ARG, NULL ,'e'},
		{"QB",NO_ARG, NULL, 'K'},
		{"QB_NI",NO_ARG, NULL, 'N'},
		{"QB_FID",NO_ARG, NULL, 'B'},
		{"QB_PSNR",NO_ARG, NULL, 'D'},
		{"QB_MAD",NO_ARG, NULL, 'G'},
		{"QB_MSE",NO_ARG, NULL, 'H'},
		{"QB_RMSE",NO_ARG, NULL, 'L'},
		{"QB_MAE",NO_ARG, NULL, 'U'},
		{"QB_SE",NO_ARG, NULL, 'V'},
		{"QB_AE",NO_ARG, NULL, 'Y'},
		{"QB_SI",NO_ARG, NULL, 'X'},
		{"QB_RES",NO_ARG, NULL, 'Z'},
		{"suffix",REQ_ARG, NULL, 'O'},
		{"CB",NO_ARG,NULL,'g'},
		{"LL",NO_ARG, NULL,'l'}
	};

	/* parse the command line */
	const char optlist[] = "Z:B:D:G:H:L:U:V:Y:X:N:i:o:r:q:n:b:c:t:l:p:s:SEM:R:d:T:If:P:C:F:A:m:x:y:u:K:J:a:e"
#ifdef USE_JPWL
		"W:"
#endif /* USE_JPWL */
		"h";

	totlen=sizeof(long_option);

	// Use JP2 if no output format is specified.
	parameters->cod_format = CODEC_JP2;

	do {
    c = opj_getopt_long(argc, argv, optlist,long_option,totlen);
		if (c == -1)
			break;
		switch (c) {
			/* Not in OpenJPEG version - should a lossless version be written? */
			case 'l':
			{
				*writeUncompressed = true;
			}
			break;

			/* Suffix to be appended to the end of written filenames. */
			case 'O':
			{
				char *suffix = opj_optarg;

				// Copy string.
				strcpy(parameters->outfile,suffix);
			}
			break;

			/* Should all quality benchmark tests be performed? */
			case 'K':
			{
				// Main benchmarks
				benchmarkQualityParameters->fidelity = true;
				benchmarkQualityParameters->maximumAbsoluteDistortion = true;
				benchmarkQualityParameters->meanAbsoluteError = true;
				benchmarkQualityParameters->meanSquaredError = true;
				benchmarkQualityParameters->peakSignalToNoiseRatio = true;
				benchmarkQualityParameters->rootMeanSquaredError = true;
				benchmarkQualityParameters->performQualityBenchmarking = true;

				// Intermediate values
				benchmarkQualityParameters->squaredError = true;
				benchmarkQualityParameters->absoluteError = true;
				benchmarkQualityParameters->squaredIntensitySum = true;
			}
			break;

			/* Should all quality benchmark tests be performed - but no intermediate results shown?
			 * This is the same as the above case, but without the intermediate values printed.
			 */
			case 'N':
			{
				benchmarkQualityParameters->fidelity = true;
				benchmarkQualityParameters->maximumAbsoluteDistortion = true;
				benchmarkQualityParameters->meanAbsoluteError = true;
				benchmarkQualityParameters->meanSquaredError = true;
				benchmarkQualityParameters->peakSignalToNoiseRatio = true;
				benchmarkQualityParameters->rootMeanSquaredError = true;
				benchmarkQualityParameters->performQualityBenchmarking = true;
			}
			break;

			/* Fidelity benchmarking? */
			case 'B':
			{
				benchmarkQualityParameters->fidelity = true;
				benchmarkQualityParameters->performQualityBenchmarking = true;
			}
			break;

			/* PSNR benchmarking? */
			case 'D':
			{
				benchmarkQualityParameters->peakSignalToNoiseRatio = true;
				benchmarkQualityParameters->performQualityBenchmarking = true;
			}
			break;

			/* Maximum absolute distortion? */
			case 'G':
			{
				benchmarkQualityParameters->maximumAbsoluteDistortion = true;
				benchmarkQualityParameters->performQualityBenchmarking = true;
			}
			break;

			/* Mean squared error? */
			case 'H':
			{
				benchmarkQualityParameters->meanSquaredError = true;
				benchmarkQualityParameters->performQualityBenchmarking = true;
			}
			break;

			/* Root mean squared error? */
			case 'L':
			{
				benchmarkQualityParameters->rootMeanSquaredError = true;
				benchmarkQualityParameters->performQualityBenchmarking = true;
			}
			break;

			/* Mean absolute error? */
			case 'U':
			{
				benchmarkQualityParameters->meanAbsoluteError = true;
				benchmarkQualityParameters->performQualityBenchmarking = true;
			}
			break;

			/* Squared error? */
			case 'V':
			{
				benchmarkQualityParameters->squaredError = true;
				benchmarkQualityParameters->performQualityBenchmarking = true;
			}
			break;

			/* Absolute error? */
			case 'Y':
			{
				benchmarkQualityParameters->absoluteError = true;
				benchmarkQualityParameters->performQualityBenchmarking = true;
			}
			break;

			/* Squared uncompressed image intensity sum? */
			case 'X':
			{
				benchmarkQualityParameters->squaredIntensitySum = true;
				benchmarkQualityParameters->performQualityBenchmarking = true;
			}
			break;

			/* Should a residual image be written? */
			case 'Z':
			{
				benchmarkQualityParameters->writeResidual = true;
			}
			break;

			/* Should compression benchmarking be performed? */
			case 'g':
			{
				*performCompressionBenchmarking = true;
			}
			break;

			/* What is the first stoke of the data volume to read? */
			case 'a':
			{
				char *end;
				*firstStoke = strtol(opj_optarg,&end,10);
			}
			break;

			/*What is the last stoke of the data volume to read? */
			case 'e':
			{
				char *end;
				*lastStoke = strtol(opj_optarg,&end,10);
			}
			break;

			/* What is the first frame of the data cube to read? */
			case 'x':
			{
				char *end;
				*startFrame = strtol(opj_optarg,&end,10);
			}
			break;

			/* What is the last frame of the data cube to read? */
			case 'y':
			{
				char *end;
				*endFrame = strtol(opj_optarg,&end,10);
			}
			break;

			/* What transform should be performed on the raw FITS data? */
			case 'A':
			{
				// This should be synced with the possibilities in f2j.h.
				char *transformString = opj_optarg;

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
					fprintf(stderr,"Unknown transform specified: %s.  Using default instead.\n",opj_optarg);
				}
			}
 			break;

			/* Largely as in OpenJPEG below */
			case 'i':			/* input file */
			{
				char *infile = opj_optarg;
				strncpy(parameters->infile, infile, sizeof(parameters->infile)-1);
			}
			break;

				/* ----------------------------------------------------- */

			case 'o':			/* output format - either J2K or JP2 - differs from image_to_j2k.c */
			{
				char *outfile = opj_optarg;

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
				char *s = opj_optarg;
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
				char *s = opj_optarg;
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

				char *s = opj_optarg;
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
				sscanf(opj_optarg, "%d,%d", &parameters->cp_tdx, &parameters->cp_tdy);
				parameters->tile_size_on = OPJ_TRUE;
			}
			break;

				/* ----------------------------------------------------- */

			case 'n':			/* resolution */
			{
				sscanf(opj_optarg, "%d", &parameters->numresolution);
			}
			break;

				/* ----------------------------------------------------- */
			case 'c':			/* precinct dimension */
			{
				char sep;
				int res_spec = 0;

				char *s = opj_optarg;
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
				sscanf(opj_optarg, "%d,%d", &cblockw_init, &cblockh_init);
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

				strncpy(progression, opj_optarg, 4);
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
				if (sscanf(opj_optarg, "%d,%d", &parameters->subsampling_dx,
                                    &parameters->subsampling_dy) != 2) {
					fprintf(stderr,	"'-s' sub-sampling argument error !  [-s dx,dy]\n");
					return 1;
				}
			}
			break;

				/* ----------------------------------------------------- */

			case 'd':			/* coordonnate of the reference grid */
			{
				if (sscanf(opj_optarg, "%d,%d", &parameters->image_offset_x0,
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

				char *s = opj_optarg;
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
				if (sscanf(opj_optarg, "%d", &value) == 1) {
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
				if (sscanf(opj_optarg, "c=%d,U=%d", &parameters->roi_compno,
                                           &parameters->roi_shift) != 2) {
					fprintf(stderr, "ROI error !! [-ROI c='compno',U='shift']\n");
					return 1;
				}
			}
			break;

				/* ------------------------------------------------------ */

			case 'T':			/* Tile offset */
			{
				if (sscanf(opj_optarg, "%d,%d", &parameters->cp_tx0, &parameters->cp_ty0) != 2) {
					fprintf(stderr, "-T 'tile offset' argument error !! [-T X0,Y0]");
					return 1;
				}
			}
			break;

				/* ------------------------------------------------------ */

			case 'C':			/* add a comment */
			{
				parameters->cp_comment = (char*)malloc(strlen(opj_optarg) + 1);
				if(parameters->cp_comment) {
					strcpy(parameters->cp_comment, opj_optarg);
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

			case 'u':			/* Tile part generation*/
			{
				parameters->tp_flag = opj_optarg[0];
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
				token = strtok(opj_optarg, ",");
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
				parameters->jpwl_epc_on = OPJ_TRUE;

			}
			break;
#endif /* USE_JPWL */
/* <<UniPG */
			/* ------------------------------------------------------ */

						case 'J':			/* jpip on */
						{
						  parameters->jpip_on = OPJ_TRUE;

						  // Switch to CODEC_JP2
						  if (parameters->cod_format == CODEC_J2K) {
							  parameters->cod_format = CODEC_JP2;
							  fprintf(stderr,"J2K output not possible with JPIP.  Switching to JP2.\n");
						  }
						}
						break;
							/* ------------------------------------------------------ */

			default:
				fprintf(stderr, "ERROR -> Command line not valid\n");
				return 1;
		}
	} while (c != -1);

	/* check for possible errors */
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

	/* Compression benchmarking is only accurate if all planes of a data cube are read.  Display a message
	 * informing the user of this if they specify a custom starting plane.
	 */
	if (*performCompressionBenchmarking && *startFrame != -1) {
		fprintf(stderr,"Compression benchmarking results are only accurate if all planes of a data cube\n");
		fprintf(stderr,"are converted.  Beware of this when interpreting results.\n");
	}

	return 0;
}
