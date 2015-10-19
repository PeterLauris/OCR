#include "allheaders.h"
#include <stdlib.h>
#include <string>


static const l_int32  MIN_WORD_WIDTH = 6;
static const l_int32  MIN_WORD_HEIGHT = 4;
static const l_int32  MAX_WORD_WIDTH = 1000;
static const l_int32  MAX_WORD_HEIGHT = 93;

#define   BUF_SIZE                  1024

/* select additional debug output */
#define   RENDER_PAGES              1
#define   RENDER_DEBUG              1

#define   BINARY_THRESHOLD			130


#define   DESKEW_REDUCTION      2      /* 1, 2 or 4 */

/* sweep only */
#define   SWEEP_RANGE           10.    /* degrees */
#define   SWEEP_DELTA           0.2    /* degrees */
#define   SWEEP_REDUCTION       2      /* 1, 2, 4 or 8 */

/* sweep and search */
#define   SWEEP_RANGE2          10.    /* degrees */
#define   SWEEP_DELTA2          1.     /* degrees */
#define   SWEEP_REDUCTION2      2      /* 1, 2, 4 or 8 */
#define   SEARCH_REDUCTION      2      /* 1, 2, 4 or 8 */
#define   SEARCH_MIN_DELTA      0.01   /* degrees */

#define L_BUF_SIZE   512

#define MIN_JUMP  2
#define MIN_REVERSAL 3

#define  GAMMA		 1 //no enhancement
#define  MINVAL      100
#define  MAXVAL      155
#define  CONTRAST_FACTOR 0.15

#define UNSHARP_HALF_WIDTH 20 //1, 2, ...
#define UNSHARP_FRACT 0.4 //0.2 < fract < 0.7

#define DARK_THRESH_LIGHT 0
#define LIGHT_THRESH_LIGHT 155
#define DARK_THRESH_DARK 100
#define LIGHT_THRESH_DARK 255
#define DIFF_THRESH 0
#define FACTOR 4
#define BGVAL_MIDDLE 186
#define BGVAL_DELTA 30

l_int32 getLightingBGval(PIX *pixs) {
	l_float32 lightPixFract, lightColorFract;
	l_float32 darkPixFract, darkColorFract;
	pixColorFraction(pixs, DARK_THRESH_LIGHT, LIGHT_THRESH_LIGHT, DIFF_THRESH, FACTOR, &lightPixFract, &lightColorFract);
	pixColorFraction(pixs, DARK_THRESH_DARK, LIGHT_THRESH_DARK, DIFF_THRESH, FACTOR, &darkPixFract, &darkColorFract);

	l_float32 pixFractDiff = lightPixFract - darkPixFract;
	printf("BGVAL: %d\n", (l_int32)(BGVAL_MIDDLE + pixFractDiff * BGVAL_DELTA));
	return (l_int32)(BGVAL_MIDDLE + pixFractDiff * BGVAL_DELTA);
}

l_int32 light_pixDisplayWriteFormat(PIX *pixs, l_int32  reduction, l_int32  format)
{
	return 0;
	char            buffer[L_BUF_SIZE];
	l_int32         ignore;
	l_float32       scale;
	PIX            *pixt, *pix8;
	static l_int32  index = 0;  /* caution: not .so or thread safe */

	if (reduction == 0) return 0;

	if (reduction < 0) {
		index = 0;  /* reset; this will cause erasure at next call to write */
		return 0;
	}

	if (format != IFF_JFIF_JPEG && format != IFF_PNG)
		printf("ERROR: invalid format\n");
	if (!pixs)
		printf("ERROR: pixs not defined\n");

	if (index == 0) {
		snprintf(buffer, L_BUF_SIZE,
			"del /f tmp\\junk_write_display.*.png tmp\\junk_write_display.*.jpg");
		//printf("test\n");
		ignore = system(buffer);
	}
	index++;

	if (reduction == 1)
		pixt = pixClone(pixs);
	else {
		scale = 1. / (l_float32)reduction;
		if (pixGetDepth(pixs) == 1)
			pixt = pixScaleToGray(pixs, scale);
		else
			pixt = pixScale(pixs, scale, scale);
	}

	if (pixGetDepth(pixt) == 16) {
		pix8 = pixMaxDynamicRange(pixt, L_LOG_SCALE);
		snprintf(buffer, L_BUF_SIZE, "tmp\\junk_write_display.%03d.png", index);
		pixWrite(buffer, pix8, IFF_PNG);
		pixDestroy(&pix8);
	}
	else if (pixGetDepth(pixt) < 8 || pixGetColormap(pixt) ||
		format == IFF_PNG) {
		snprintf(buffer, L_BUF_SIZE, "tmp\\junk_write_display.%03d.png", index);
		pixWrite(buffer, pixt, IFF_PNG);
	}
	else {
		snprintf(buffer, L_BUF_SIZE, "tmp\\junk_write_display.%03d.jpg", index);
		if (pixt == NULL) printf("pixt is NULL");
		pixWrite(buffer, pixt, format);
	}
	pixDestroy(&pixt);

	return 0;
}

PIX * normalizeLighting(PIX *pixs) {
	l_int32      d;
	PIX         *pixc, *pixr, *pixg, *pixb, *pixsg, *pixsm, *pixd;

	l_int32 bgval = getLightingBGval(pixs);

	/* Normalize for uneven illumination on RGB image */
	pixBackgroundNormRGBArraysMorph(pixs, NULL, 8, 5, bgval, &pixr, &pixg, &pixb);
	pixd = pixApplyInvBackgroundRGBMap(pixs, pixr, pixg, pixb, 8, 8);
	light_pixDisplayWriteFormat(pixd, 2, IFF_JFIF_JPEG);
	pixDestroy(&pixr);
	pixDestroy(&pixg);
	pixDestroy(&pixb);
	pixDestroy(&pixd);

	/* Convert the RGB image to grayscale. */
	pixsg = pixConvertRGBToLuminance(pixs);
	light_pixDisplayWriteFormat(pixsg, 2, IFF_JFIF_JPEG);

	/* Remove the text in the fg. */
	pixc = pixCloseGray(pixsg, 25, 25);
	light_pixDisplayWriteFormat(pixc, 2, IFF_JFIF_JPEG);

	/* Smooth the bg with a convolution. */
	pixsm = pixBlockconv(pixc, 15, 15);
	light_pixDisplayWriteFormat(pixsm, 2, IFF_JFIF_JPEG);
	pixDestroy(&pixc);

	/* Normalize for uneven illumination on gray image. */
	pixBackgroundNormGrayArrayMorph(pixsg, NULL, 8, 5, bgval, &pixg);
	pixc = pixApplyInvBackgroundGrayMap(pixsg, pixg, 8, 8);
	light_pixDisplayWriteFormat(pixc, 2, IFF_JFIF_JPEG);
	pixDestroy(&pixg);

	pixDestroy(&pixsg);
	pixDestroy(&pixsm);

	return pixc;
}

PIX * removeNoise(PIX *pixs) {
	pixGammaTRC(pixs, pixs, GAMMA, MINVAL, MAXVAL);

	pixContrastTRC(pixs, pixs, CONTRAST_FACTOR);

	//pixEqualizeTRC(pixs, pixs, 0, 1);

	pixs = pixUnsharpMaskingGray2D(pixs, 4, 0.4);

	//PIX *pix_old = pixs;
	//pixs = pixUnsharpMasking(pixs, UNSHARP_HALF_WIDTH, UNSHARP_FRACT);
	//pixDestroy(&pix_old);

	/*pix_old = pixs;
	pixs = pixBlockconv(pixs, 100, 100);*/
	//pixDestroy(&pix_old);

	return pixs;
}

/*void testRemoveNoise(PIX *pixs) {
	PIX *pix_gamma = NULL;
	PIX *pix_contrast = NULL;
	PIX *pix_unsharp = NULL;
	PIX *pix_1bpp = NULL;
	char filename[BUF_SIZE];
	for (float gamma = 0.5; gamma < 1.5; gamma += 0.3) {
		for (int minVal = 0; minVal < 125; minVal += 20) {
			for (int maxVal = 255; maxVal > 130; maxVal -= 20) {
				pix_gamma = pixGammaTRC(pix_gamma, pixs, gamma, minVal, maxVal);
				for (float contrast = 0.1; contrast < 1; contrast += 0.2) {
					pix_contrast = pixContrastTRC(pix_contrast, pix_gamma, contrast);
					for (int halfwidth = 1; halfwidth < 3; halfwidth += 1) {
						for (float fract = 0.1; fract < 1; fract += 0.2) {
							pix_unsharp = pixUnsharpMaskingGray2D(pix_contrast, halfwidth, fract);
							for (int i = 140; i < 170; i += 10) {
								pix_1bpp = pixConvertTo1(pix_unsharp, i);

								snprintf(filename, BUF_SIZE, "%s_%f_%d_%d_%f_%d_%f_%d.png", "../../../images/results/noise-test/NOISE", gamma, minVal, maxVal, contrast, halfwidth, fract, i);
								pixWrite(filename, pix_1bpp, IFF_PNG);
								pixDestroy(&pix_1bpp);
							}
							pixDestroy(&pix_unsharp);
						}
					}
					pixDestroy(&pix_contrast);
				}
				pixDestroy(&pix_gamma);
			}
		}
	}
}*/

void PixAddEdgeData(PIXA *pixa, PIX *pixs, l_int32 side, l_int32 minjump, l_int32  minreversal)
{
	l_float32  jpl, jspl, rpl;
	PIX       *pixt1, *pixt2;

	pixMeasureEdgeSmoothness(pixs, side, minjump, minreversal, &jpl,
		&jspl, &rpl, "junkedge.png");
	fprintf(stderr, "side = %d: jpl = %6.3f, jspl = %6.3f, rpl = %6.3f\n",
		side, jpl, jspl, rpl);
	pixt1 = pixRead("junkedge.png");
	pixt2 = pixAddBorder(pixt1, 10, 0);  /* 10 pixel white border */
	pixaAddPix(pixa, pixt2, L_INSERT);
	pixDestroy(&pixt1);
	return;
}

//is not used / not working correctly?
//pixs needs to be 1bpp
PIX * removeImages(PIX *pixs) {
	char         buf[256];
	l_int32      index, zero;
	PIX         *pixr;   /* image reduced to 150 ppi */
	PIX         *pixhs;  /* image of halftone seed, 150 ppi */
	PIX         *pixm;   /* image of mask of components, 150 ppi */
	PIX         *pixhm1; /* image of halftone mask, 150 ppi */
	PIX         *pixht;  /* image of halftone components, 150 ppi */
	PIX         *pixnht; /* image without halftone components, 150 ppi */
	PIX         *pixt1, *pixt2;

	pixt1 = pixScaleToGray2(pixs);

	if (pixt1 == NULL) printf("pix is NULL\n");
	//pixWrite("tmp/orig.gray.150.png", pixt1, IFF_PNG);
	pixDestroy(&pixt1);
	pixr = pixReduceRankBinaryCascade(pixs, 1, 0, 0, 0);

	/* Get seed for halftone parts */
	pixt1 = pixReduceRankBinaryCascade(pixr, 4, 4, 3, 0);
	pixt2 = pixOpenBrick(NULL, pixt1, 5, 5);
	pixhs = pixExpandBinaryPower2(pixt2, 8);
	//pixWrite("tmp/htseed.150.png", pixhs, IFF_PNG);
	pixDestroy(&pixt1);
	pixDestroy(&pixt2);

	/* Get mask for connected regions */
	pixm = pixCloseSafeBrick(NULL, pixr, 4, 4);
	//pixWrite("tmp/ccmask.150.png", pixm, IFF_PNG);

	/* Fill seed into mask to get halftone mask */
	pixhm1 = pixSeedfillBinary(NULL, pixhs, pixm, 4);
	pixWrite("tmp/htmask.150.png", pixhm1, IFF_PNG);

	// Extract halftone stuff 
	pixht = pixAnd(NULL, pixhm1, pixr);
	pixWrite("tmp/ht.150.png", pixht, IFF_PNG);

	// Extract non-halftone stuff
	pixnht = pixXor(NULL, pixht, pixr);
	pixWrite("tmp/text.150_1.png", pixht, IFF_PNG);
	pixWrite("tmp/text.150_2.png", pixr, IFF_PNG);
	pixWrite("tmp/text.150_0.png", pixs, IFF_PNG);
	pixWrite("tmp/text.150.png", pixnht, IFF_PNG);
	pixZero(pixht, &zero);

	pixDestroy(&pixr);
	pixDestroy(&pixhs);
	pixDestroy(&pixm);
	pixDestroy(&pixhm1);
	pixDestroy(&pixht);

	return pixnht;
}

PIX * findWords(PIX *pixt1, BOXAA *baa, NUMAA *naa){
	l_int32      i, j, w, h, ncomp;
	l_int32      index, ival, rval, gval, bval;
	BOX         *box;
	BOXA        *boxa;
	JBDATA      *data;
	JBCLASSER   *classer;
	NUMA        *nai;
	PIX         *pixt2, *pixd;
	PIXCMAP     *cmap;


	pixGetWordBoxesInTextlines(pixt1, 1, MIN_WORD_WIDTH, MIN_WORD_HEIGHT,
		MAX_WORD_WIDTH, MAX_WORD_HEIGHT,
		&boxa, &nai);
	boxaaAddBoxa(baa, boxa, L_INSERT);
	numaaAddNuma(naa, nai, L_INSERT);

	/* Show the results on a 2x reduced image, where each
	* word is outlined and the color of the box depends on the
	* computed textline. */
	pixGetDimensions(pixt1, &w, &h, NULL);
	pixd = pixCreate(w, h, 8);
	printf("%d, %d\n", w, h);
	//if ((pixCreateHeader(w, h, 8)) == NULL) printf("pixd not made\n");
	if (!pixd) printf("pix not defined\n");
	cmap = pixcmapCreateRandom(8, 1, 1);  /* first color is black */
	pixSetColormap(pixd, cmap);

	pixt2 = pixUnpackBinary(pixt1, 8, 1);
	pixRasterop(pixd, 0, 0, w, h, PIX_SRC | PIX_DST, pixt2, 0, 0);
	ncomp = boxaGetCount(boxa);
	for (j = 0; j < ncomp; j++) {
		box = boxaGetBox(boxa, j, L_CLONE);
		numaGetIValue(nai, j, &ival);
		index = 1 + (ival % 254);  /* omit black and white */
		pixcmapGetColor(cmap, index, &rval, &gval, &bval);
		pixRenderBoxArb(pixd, box, 2, rval, gval, bval);
		boxDestroy(&box);
	}

	pixDestroy(&pixt2);
	return pixd;
}

void writeWords(PIX *pixs, char *dirout, char *rootname, l_int32 nr) {
	char filename[BUF_SIZE];
	char targetPath[1024];
	snprintf(filename, BUF_SIZE, "%s.%05d.png", rootname, nr);
	fprintf(stderr, "filename: %s\n", filename);
	targetPath[0] = '\0';
	sprintf_s(targetPath, "%s%s%s", dirout, "/", filename);
	printf("file path: %s\n", targetPath);
	if (pixs == NULL) printf("pixd is NULL\n");
	pixWrite(targetPath, pixs, IFF_PNG);
}

void findWords2(char* dirin, char *dirout) {
	char         filename[BUF_SIZE];
	char        *rootname, *fname;
	l_int32      reduction, i, firstpage, npages, nfiles;
	l_float32    thresh, weight;
	JBDATA      *data;
	JBCLASSER   *classer;
	NUMA        *natl;
	SARRAY      *safiles;
	PIX         *pix;
	PIXA        *pixa, *pixadb;

	reduction = 1;// atoi(argv[2]);
	thresh = 0.8;// atof(argv[3]);
	weight = 0.6;// atof(argv[4]);
	rootname = "result_";

	firstpage = 0;
	npages = 0;

	classer = jbWordsInTextlines(dirin, reduction, MAX_WORD_WIDTH,
		MAX_WORD_HEIGHT, thresh, weight,
		&natl, firstpage, npages);

	printf("pixa components: %d\n", pixaGetCount(classer->pixat));

	/* Save and write out the result */
	snprintf(filename, BUF_SIZE, "%s%s%s", dirout, "/", rootname);
	data = jbDataSave(classer);
	jbDataWrite(filename, data);

#if  RENDER_PAGES
	/* Render the pages from the classifier data, and write to file.
	* Use debugflag == FALSE to omit outlines of each component. */
	pixa = jbDataRender(data, FALSE);
	npages = pixaGetCount(pixa);
	printf("npages: %d\n", npages);
	for (i = 0; i < npages; i++) {
		pix = pixaGetPix(pixa, i, L_CLONE);
		snprintf(filename, BUF_SIZE, "%s%s%s.%05d.png", dirout, "/", rootname, i);
		fprintf(stderr, "filename: %s\n", filename);
		pixWrite(filename, pix, IFF_PNG);
		pixDestroy(&pix);
	}
	pixaDestroy(&pixa);
#endif  /* RENDER_PAGES */

#if  RENDER_DEBUG
	/* Use debugflag == TRUE to see outlines of each component. */
	pixadb = jbDataRender(data, TRUE);
	/* Write the debug pages out */
	npages = pixaGetCount(pixadb);
	for (i = 0; i < npages; i++) {
		pix = pixaGetPix(pixadb, i, L_CLONE);
		snprintf(filename, BUF_SIZE, "%s%s%s.db.%05d.png", dirout, "/", rootname, i);
		fprintf(stderr, "filename: %s\n", filename);
		pixWrite(filename, pix, IFF_PNG);
		pixDestroy(&pix);
	}
	pixaDestroy(&pixadb);
#endif  /* RENDER_DEBUG */

	jbClasserDestroy(&classer);
	jbDataDestroy(&data);
	numaDestroy(&natl);
}

int main() {
	char *dirin, *dirout_1bpp, *dirout_words, *dirout, *rootname, *fname;

	PIX *pixs, *pixt, *pix_deskew, *pix_light, *pix_images, *pix_words;

	JBDATA *jbdata;
	JBCLASSER *classer;

	l_int32 firstpage = 0;
	l_int32 npages = 0;


	dirin = "../../../images/test-set";
	dirout_1bpp = "../../../images/results/1bpp";
	dirout_words = "../../../images/results/words";
	dirout = "../../../images/results";
	rootname = "result_";

	/* Compute the word bounding boxes at 2x reduction, along with
	* the textlines that they are in. */
	SARRAY *safiles = getSortedPathnamesInDirectory(dirin, NULL, firstpage, npages);
	l_int32 nfiles = sarrayGetCount(safiles);
	printf("Count received: %d\n", nfiles);
	BOXAA *baa = boxaaCreate(nfiles);
	NUMAA *naa = numaaCreate(nfiles);


	printf("Begin image processing\n");

	//l_int32 w, h;
	//l_float32 deg2rad = 3.1415926535 / 180.;
	//pixGetDimensions(pixs, &w, &h, NULL);
	//pixFindSkew(pix_tmp, &angle, &conf);
	//pixs = pixRotate(pixs, deg2rad * angle, L_ROTATE_AREA_MAP, L_BRING_IN_WHITE, w, h);

	for (l_int32 i = 0; i < nfiles; i++) {
		printf("--- %d ---\n", i);
		fname = sarrayGetString(safiles, i, 0);
		if ((pixs = pixRead(fname)) == NULL) {
			printf("image file %s not read\n", fname);
			continue;
		}

		l_float32 angle, conf, score;
		pix_deskew = pixDeskew(pixs, 0);

		pix_light = normalizeLighting(pix_deskew);
		removeNoise(pix_light);

		pixt = pixConvertTo1(pix_light, 150);

		//create a 1bpp image in the corresponding location
		char filename[BUF_SIZE];
		snprintf(filename, BUF_SIZE, "%s%s%s.%05d.png", dirout_1bpp, "/", rootname, i);
		pixWrite(filename, pixt, IFF_PNG);
		
		pix_words = findWords(pixt, baa, naa);
		writeWords(pix_words, dirout_words, rootname, i);


		pixDestroy(&pixt);
		pixDestroy(&pix_light);
		pixDestroy(&pix_deskew);
		pixDestroy(&pix_words);
		pixDestroy(&pixs);
		printf("\n\n", i);
	}

	pixDestroy(&pixt);
	pixDestroy(&pix_deskew);
	pixDestroy(&pix_light);
	pixDestroy(&pix_words);
	pixDestroy(&pixs);
	boxaaDestroy(&baa);
	numaaDestroy(&naa);
	sarrayDestroy(&safiles);

	findWords2(dirout_1bpp, dirout);

	printf("\n---\nEND\n");
	getchar();
	return 0;
}