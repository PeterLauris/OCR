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


int findWords(PIX *pixs, char *dirout, char *rootname, l_int32 nr, BOXAA *baa, NUMAA *naa){
	char         filename[BUF_SIZE];
	l_int32      i, j, w, h, ncomp;
	l_int32      index, ival, rval, gval, bval;
	BOX         *box;
	BOXA        *boxa;
	JBDATA      *data;
	JBCLASSER   *classer;
	NUMA        *nai;
	PIX         *pixt, *pixt1, *pixt2, *pixd;
	PIXCMAP     *cmap;
	static char  mainName[] = "wordsinorder";

	char targetPath[1024];


	pixGetWordBoxesInTextlines(pixs, 2, MIN_WORD_WIDTH, MIN_WORD_HEIGHT,
		MAX_WORD_WIDTH, MAX_WORD_HEIGHT,
		&boxa, &nai);
	boxaaAddBoxa(baa, boxa, L_INSERT);
	numaaAddNuma(naa, nai, L_INSERT);

#if  RENDER_PAGES
	/* Show the results on a 2x reduced image, where each
	* word is outlined and the color of the box depends on the
	* computed textline. */
	if (pixGetDepth(pixs) != 1) printf("pixs is not binary\n");
	pixt = pixConvertTo1(pixs, 150);
	pixt1 = pixReduceRankBinary2(pixt, 2, NULL);
	if (!pixt1) printf("pixt1 is null\n");
	pixGetDimensions(pixt1, &w, &h, NULL);
	pixd = pixCreate(w, h, 8);
	printf("%d, %d\n", w, h);
	if ((pixCreateHeader(w, h, 8)) == NULL)
		printf("pixd not made\n");
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
	printf("file path: %s", targetPath);
	pixWrite(targetPath, pixd, IFF_JFIF_JPEG);
	//free(targetPath);
	pixDestroy(&pixt);
	pixDestroy(&pixt1);
	pixDestroy(&pixt2);
	pixDestroy(&pixd);
	//getchar();
#endif  /* RENDER	_PAGES */
}

int main() {
	char        *dirin, *dirout, *rootname, *fname;

	PIX *pixs;

	//if (argc == 3) {
	l_int32 firstpage = 0;
	l_int32 npages = 0;


	dirin = "C:\\Users\\peter\\OneDrive\\Projects\\OCR\\images\\sample-pages";
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

		pixs = pixConvertTo1(pixs, 130); //convert to binary

		findWords(pixs, dirout, rootname, i, baa, naa);
	}

	pixDestroy(&pixs);
	boxaaDestroy(&baa);
	numaaDestroy(&naa);
	sarrayDestroy(&safiles);

	//apstr?d? att?lu
	//skew
	//line removal
	//...


	//att?l? samekl? tekstu

	//izveido tekstu failu


	printf("\n---\nEND\n");
	getchar();
	return 0;
}