#include "allheaders.h"
#include <stdlib.h>
#include <string>


static const l_int32  MIN_WORD_WIDTH = 6;
static const l_int32  MIN_WORD_HEIGHT = 4;
static const l_int32  MAX_WORD_WIDTH = 500;
static const l_int32  MAX_WORD_HEIGHT = 100;

#define   BUF_SIZE                  512

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

void findWords(PIX *pixt1, char *dirout, char *rootname, l_int32 nr, BOXAA *baa, NUMAA *naa){
	char         filename[BUF_SIZE];
	l_int32      i, j, w, h, ncomp;
	l_int32      index, ival, rval, gval, bval;
	BOX         *box;
	BOXA        *boxa;
	JBDATA      *data;
	JBCLASSER   *classer;
	NUMA        *nai;
	PIX         *pixt2, *pixd;
	PIXCMAP     *cmap;

	char targetPath[1024];


	pixGetWordBoxesInTextlines(pixt1, 1, MIN_WORD_WIDTH, MIN_WORD_HEIGHT,
		MAX_WORD_WIDTH, MAX_WORD_HEIGHT,
		&boxa, &nai);
	boxaaAddBoxa(baa, boxa, L_INSERT);
	numaaAddNuma(naa, nai, L_INSERT);

#if  RENDER_PAGES
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

	snprintf(filename, BUF_SIZE, "%s.%05d.png", rootname, nr);
	fprintf(stderr, "filename: %s\n", filename);
	targetPath[0] = '\0';
	sprintf_s(targetPath, "%s%s%s", dirout, "\\", filename);
	printf("file path: %s\n", targetPath);
	if (pixd == NULL) printf("pixd is NULL\n");
	pixWrite(filename, pixd, IFF_PNG);
	//free(targetPath);
	pixDestroy(&pixt2);
	pixDestroy(&pixd);
	//getchar();
#endif  /* RENDER_PAGES */
}

void findSkew(PIX *pixs, l_float32 &angle, l_float32 &conf, l_float32 &score) {
	//pixFindSkew(pixs, &angle, &conf);

	pixFindSkewSweepAndSearchScorePivot(pixs, &angle, &conf, &score,
		SWEEP_REDUCTION2, SEARCH_REDUCTION,
		0.0, SWEEP_RANGE2, SWEEP_DELTA2,
		SEARCH_MIN_DELTA,
		L_SHEAR_ABOUT_CORNER);
	fprintf(stderr, "pixFind...Pivot(about corner):\n"
		"  conf = %5.3f, angle = %7.3f degrees, score = %f\n",
		conf, angle, score);

	/*pixFindSkewSweepAndSearchScorePivot(pix_deskew, &angle, &conf, &score,
	SWEEP_REDUCTION2, SEARCH_REDUCTION,
	0.0, SWEEP_RANGE2, SWEEP_DELTA2,
	SEARCH_MIN_DELTA,
	L_SHEAR_ABOUT_CENTER);
	fprintf(stderr, "pixFind...Pivot(about center):\n"
	"  conf = %5.3f, angle = %7.3f degrees, score = %f\n",
	conf, angle, score);*/
}

PIX * normalizeLighting(PIX *pixs) {
	l_int32      d;
	PIX         *pixc, *pixr, *pixg, *pixb, *pixsg, *pixsm, *pixd;

	/* Normalize for uneven illumination on RGB image */
	pixBackgroundNormRGBArraysMorph(pixs, NULL, 4, 5, 200, &pixr, &pixg, &pixb);
	pixd = pixApplyInvBackgroundRGBMap(pixs, pixr, pixg, pixb, 4, 4);
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
	pixBackgroundNormGrayArrayMorph(pixsg, NULL, 4, 5, 200, &pixg);
	pixc = pixApplyInvBackgroundGrayMap(pixsg, pixg, 4, 4);
	light_pixDisplayWriteFormat(pixc, 2, IFF_JFIF_JPEG);
	pixDestroy(&pixg);

	/* Generate the output image */
	/*pixa = pixaReadFiles("\\tmp", "junk_write_display");
	pixd = pixaDisplayTiledAndScaled(pixa, 8, 350, 4, 0, 25, 2);
	pixWrite("tmp\\adapt.png", pixd, IFF_PNG);
	pixDisplayWithTitle(pixd, 100, 100, NULL, 1);
	pixDestroy(&pixd);*/

	pixDestroy(&pixsg);
	pixDestroy(&pixsm);

	return pixc;
}

int main() {
	char *dirin, *dirout, *rootname, *fname;

	PIX *pixs, *pixt, *pix_deskew, *pix_light;

	l_int32 firstpage = 0;
	l_int32 npages = 0;


	dirin = "C:\\Users\\peter\\OneDrive\\Projects\\OCR\\images\\test-set";
	dirout = "C:\\Users\\peter\\OneDrive\\Projects\\OCR\\images\\result";
	rootname = "result_";

	/* Compute the word bounding boxes at 2x reduction, along with
	* the textlines that they are in. */
	SARRAY *safiles = getSortedPathnamesInDirectory(dirin, NULL, firstpage, npages);
	l_int32 nfiles = sarrayGetCount(safiles);
	printf("Count received: %d\n", nfiles);
	BOXAA *baa = boxaaCreate(nfiles);
	NUMAA *naa = numaaCreate(nfiles);


	printf("Begin image processing\n");

	for (l_int32 i = 0; i < nfiles; i++) {
		fname = sarrayGetString(safiles, i, 0);
		if ((pixs = pixRead(fname)) == NULL) {
			printf("image file %s not read\n", fname);
			continue;
		}

		l_float32    angle, conf, score;

		findSkew(pixs, angle, conf, score);
		pix_deskew = pixDeskew(pixs, 0);


		pix_light = normalizeLighting(pix_deskew);

		pixt = pixConvertTo1(pix_light, 150);
		
		findWords(pixt, dirout, rootname, i, baa, naa);


		pixDestroy(&pixt);
		pixDestroy(&pix_light);
		pixDestroy(&pix_deskew);
		pixDestroy(&pixs);
	}

	pixDestroy(&pixt);
	pixDestroy(&pix_deskew);
	pixDestroy(&pix_light);
	pixDestroy(&pixs);
	boxaaDestroy(&baa);
	numaaDestroy(&naa);
	sarrayDestroy(&safiles);

	printf("\n---\nEND\n");
	getchar();
	return 0;
}