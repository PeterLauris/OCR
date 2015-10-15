/*====================================================================*
-  Copyright (C) 2001 Leptonica.  All rights reserved.
-  This software is distributed in the hope that it will be
-  useful, but with NO WARRANTY OF ANY KIND.
-  No author or distributor accepts responsibility to anyone for the
-  consequences of using this software, or for whether it serves any
-  particular purpose or works at all, unless he or she says so in
-  writing.  Everyone is granted permission to copy, modify and
-  redistribute this source code, for commercial or non-commercial
-  purposes, with the following restrictions: (1) the origin of this
-  source code must not be misrepresented; (2) modified versions must
-  be plainly marked as such; and (3) this notice may not be removed
-  or altered from any source or modified source distribution.
*====================================================================*/

/*
* livre_adapt.c
*
* This shows two ways to normalize a document image for uneven
* illumination.  It is somewhat more complicated than using the
* morphological tophat.
*/

#include <stdio.h>
#include <stdlib.h>
#include "allheaders.h"

#define L_BUF_SIZE   512


l_int32
cheap_pixDisplayWriteFormat(PIX     *pixs,
	l_int32  reduction,
	l_int32  format)
{
	char            buffer[L_BUF_SIZE];
	l_int32         ignore;
	l_float32       scale;
	PIX            *pixt, *pix8;
	static l_int32  index = 0;  /* caution: not .so or thread safe */

	PROCNAME("cheap_pixDisplayWriteFormat");

	if (reduction == 0) return 0;

	if (reduction < 0) {
		index = 0;  /* reset; this will cause erasure at next call to write */
		return 0;
	}

	if (format != IFF_JFIF_JPEG && format != IFF_PNG)
		return ERROR_INT("invalid format", procName, 1);
	if (!pixs)
		return ERROR_INT("pixs not defined", procName, 1);

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

int main(int    argc,
	char **argv)
{
	l_int32      d;
	PIX         *pixs, *pixc, *pixr, *pixg, *pixb, *pixsg, *pixsm, *pixd;
	PIXA        *pixa;
	static char  mainName[] = "livre_adapt";

	if (argc != 1)
		exit(ERROR_INT(" Syntax:  livre_adapt", mainName, 1));

	/* Read the image in at 150 ppi. */
	pixDisplayWrite(NULL, -1);
	if ((pixs = pixRead("..\\..\\..\\images\\test-set\\darkened.jpg")) == NULL)
		exit(ERROR_INT("pix not made", mainName, 1));
	cheap_pixDisplayWriteFormat(pixs, 2, IFF_JFIF_JPEG);

	/* Normalize for uneven illumination on RGB image */
	pixBackgroundNormRGBArraysMorph(pixs, NULL, 4, 5, 200,
		&pixr, &pixg, &pixb);
	pixd = pixApplyInvBackgroundRGBMap(pixs, pixr, pixg, pixb, 4, 4);
	//pixDisplayWriteFormat(pixd, 2, IFF_JFIF_JPEG);
	pixDestroy(&pixr);
	pixDestroy(&pixg);
	pixDestroy(&pixb);
	pixDestroy(&pixd);

	/* Convert the RGB image to grayscale. */
	pixsg = pixConvertRGBToLuminance(pixs);
	cheap_pixDisplayWriteFormat(pixsg, 2, IFF_JFIF_JPEG);

	/* Remove the text in the fg. */
	pixc = pixCloseGray(pixsg, 25, 25);
	cheap_pixDisplayWriteFormat(pixc, 2, IFF_JFIF_JPEG);

	/* Smooth the bg with a convolution. */
	pixsm = pixBlockconv(pixc, 15, 15);
	cheap_pixDisplayWriteFormat(pixsm, 2, IFF_JFIF_JPEG);
	pixDestroy(&pixc);

	/* Normalize for uneven illumination on gray image. */
	pixBackgroundNormGrayArrayMorph(pixsg, NULL, 4, 5, 200, &pixg);
	pixc = pixApplyInvBackgroundGrayMap(pixsg, pixg, 4, 4);
	cheap_pixDisplayWriteFormat(pixc, 2, IFF_JFIF_JPEG);
	pixDestroy(&pixg);

	/* Increase the dynamic range. */
	pixd = pixGammaTRC(NULL, pixc, 1.0, 30, 180);
	cheap_pixDisplayWriteFormat(pixd, 2, IFF_JFIF_JPEG);
	pixDestroy(&pixc);

	/* Threshold to 1 bpp. */
	pixb = pixThresholdToBinary(pixd, 120);
	pixWrite("\\tmp\\adapt.jpg", pixb, IFF_JFIF_JPEG);
	cheap_pixDisplayWriteFormat(pixb, 2, IFF_PNG);
	pixDestroy(&pixd);
	pixDestroy(&pixb);

	/* Generate the output image */
	pixa = pixaReadFiles("\\tmp", "junk_write_display");
	pixd = pixaDisplayTiledAndScaled(pixa, 8, 350, 4, 0, 25, 2);
	pixWrite("\\tmp\\adapt.jpg", pixd, IFF_JFIF_JPEG);
	pixDisplayWithTitle(pixd, 100, 100, NULL, 1);
	pixDestroy(&pixd);

	pixDestroy(&pixs);
	pixDestroy(&pixsg);

	printf("\n---\nEND\n");
	getchar();
	return 0;
}

