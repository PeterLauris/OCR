#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <direct.h>

#include "allheaders.h"

int main_line_removal() {
	PIX* pixs_source = pixRead("dave-start.png");
	if (!pixs_source) {
		printf("Error opening file");
		return 1;
	}


	double deg2rad = 3.1415926535 / 180.;
	l_float32    angle, conf, score;

	PIX *pix1, *pix2, *pix3, *pix4, *pix5, *pix6, *pix7, *pix8, *pix9;

	pix1 = pixThresholdToBinary(pixs_source, 160);
	printf("Create line removal image\n");
	pixWrite("line-removal/result.line-removal-1.tif", pix1, IFF_TIFF_G4);

	pixFindSkew(pix1, &angle, &conf);
	pix2 = pixRotateAMGray(pixs_source, deg2rad * angle, 160);
	printf("Create line removal image\n");
	pixWrite("line-removal/result.line-removal-2.tif", pix2, IFF_TIFF_G4);

	l_int32 HORIZ = 1;
	l_int32 VERT = 3;
	pix3 = pixCloseGray(pix2, 51, HORIZ); //k?p?c 51?
	printf("Create line removal image\n");
	pixWrite("line-removal/result.line-removal-3.tif", pix3, IFF_TIFF_G4);

	pix4 = pixErodeGray(pix3, 5, VERT); //k?p?c 5?
	printf("Create line removal image\n");
	pixWrite("line-removal/result.line-removal-4.tif", pix4, IFF_TIFF_G4);


	pix5 = pix4;
	pix5 = pixThresholdToValue(pix5, pix4, 230, 255);
	printf("Create line removal image\n");
	pixWrite("line-removal/result.line-removal-5.tif", pix5, IFF_TIFF_G4);

	pix6 = pix5;
	pix6 = pixThresholdToValue(pix5, pix5, 210, 0);
	printf("Create line removal image\n");
	pixWrite("line-removal/result.line-removal-6.tif", pix6, IFF_TIFF_G4);

	pix7 = pixThresholdToBinary(pix6, 230);
	printf("Create line removal image\n");
	pixWrite("line-removal/result.line-removal-7.tif", pix7, IFF_TIFF_G4);
	
	pixInvert(pix6, pix6);
	pix8 = pixAddGray(NULL, pix2, pix6);
	printf("Create line removal image\n");
	pixWrite("line-removal/result.line-removal-8.tif", pix8, IFF_TIFF_G4);

	VERT = 7;
	pix9 = pixOpenGray(pix8, 3, VERT);
	printf("Create line removal image\n");
	pixWrite("line-removal/result.line-removal-9.tif", pix9, IFF_TIFF_G4);

	if (pixCombineMasked(pix8, pix9, pix7)) {
		printf("!!!Error while combining pixs!!!\n");
	}
	printf("Create line removal image\n");
	pixWrite("line-removal/result.line-removal-final.tif", pix8, IFF_TIFF_G4);


	printf("\n---\nEnd\n");
	getchar();
	return 0;
}