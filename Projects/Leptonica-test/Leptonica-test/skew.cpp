#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>

#include "allheaders.h"

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

int main_skew() {
	char* filename_paper = "../../../images/test-set/all-in-one.jpg";
	char* filename_color = "color.jpg";
	/*filename = _getcwd(filename, _MAX_PATH);
	strcat_s(filename, sizeof filename , "\\test-image.jpg");*/

	printf("reading file: %s\n", filename_paper);
	PIX* pixs_paper = pixRead(filename_paper);
	if (!pixs_paper) {
		printf("Error opening file");
		return 1;
	}
	//printf("Image width : %d\n", pixs_paper->w);
	//printf("Image height : %d\n", pixs_paper->h);

	double deg2rad = 3.1415926535 / 180.;

	l_float32    angle, conf, score;

	PIX *pix_paper_binary = pixConvertTo1(pixs_paper, 130); //parveido par binaru attelu, 2. parametrs nosaka, cik gaišs/tumšs bus attels
	printf("Create new result image\n");
	pixWrite("test-image.result.tif", pix_paper_binary, IFF_TIFF_G4);

	pixFindSkew(pix_paper_binary, &angle, &conf);
	fprintf(stderr, "pixFindSkew():\nconf = %5.3f, angle = %7.3f degrees\n", conf, angle);
	/*pixFindSkewSweepAndSearchScorePivot(pix_paper_binary, &angle, &conf, &score,
		SWEEP_REDUCTION2, SEARCH_REDUCTION,
		0.0, SWEEP_RANGE2, SWEEP_DELTA2,
		SEARCH_MIN_DELTA,
		L_SHEAR_ABOUT_CORNER);
	fprintf(stderr, "pixFind...Pivot(about corner):\n"
		"  conf = %5.3f, angle = %7.3f degrees, score = %f\n",
		conf, angle, score);
	
	pixFindSkewSweepAndSearchScorePivot(pix_paper_binary, &angle, &conf, &score,
		SWEEP_REDUCTION2, SEARCH_REDUCTION,
		0.0, SWEEP_RANGE2, SWEEP_DELTA2,
		SEARCH_MIN_DELTA,
		L_SHEAR_ABOUT_CENTER);
	fprintf(stderr, "pixFind...Pivot(about center):\n"
		"  conf = %5.3f, angle = %7.3f degrees, score = %f\n",
		conf, angle, score);*/

	PIX *pix_paper_rotated = pixDeskew(pix_paper_binary, 0);
	printf("Create new result2 image\n");
	pixWriteImpliedFormat("test-image.result2.tif", pix_paper_rotated, 0, 0);


	printf("reading file: %s\n", filename_color);
	PIX *pixs_color = pixRead(filename_color);
	if (!pixs_color) {
		printf("Error opening file");
		//return 1;
	}

	/*if (pixRasterop(pixs_paper, 0, 0, 1000, 500, PIX_MASK, pixs_color, 0, 0)) {
		printf("rasterop error");
	}
	else {
		printf("Create new rasterop-1 image\n");
		pixWrite("test-image.rasterop-1.tif", pixs_paper, IFF_TIFF_G4);
	}
	------*/

	printf("Create Sel for color image\n");
	SEL *sel_color = selCreate(1, 2, NULL);
	printf("Create new dilate image\n");
	PIX *pix_paper_dilate = NULL;
	pixDilate(pix_paper_dilate, pixs_color, sel_color);
	pixWrite("test-image.dilate.tif", pix_paper_dilate, IFF_TIFF_G4);
	pixDestroy(&pix_paper_dilate);

	pixDestroy(&pixs_paper);
	pixDestroy(&pix_paper_binary);
	pixDestroy(&pix_paper_rotated);
	pixDestroy(&pixs_color);

	printf("\n---\nEnd\n");
	getchar();
	return 0;
}