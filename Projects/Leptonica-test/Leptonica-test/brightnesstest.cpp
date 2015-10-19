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
* colorspacetest.c
*
*/

#include <stdio.h>
#include <stdlib.h>
#include "allheaders.h"

#define DARK_THRESH 100
#define LIGHT_THRESH 255
#define DIFF_THRESH 0
#define FACTOR 4


int main_brightnesstest(int argc, char **argv)
{
	l_float32 res_pixfract = 0;
	l_float32 res_colorfract = 0;

	PIX *pixs = pixRead("color_all.jpg");
	pixColorFraction(pixs, DARK_THRESH, LIGHT_THRESH, DIFF_THRESH, FACTOR, &res_pixfract, &res_colorfract);
	pixDestroy(&pixs);
	printf("All: %f, %f\n", res_pixfract, res_colorfract);

	pixs = pixRead("color_black.jpg");
	pixColorFraction(pixs, DARK_THRESH, LIGHT_THRESH, DIFF_THRESH, FACTOR, &res_pixfract, &res_colorfract);
	pixDestroy(&pixs);
	printf("Black: %f, %f\n", res_pixfract, res_colorfract);

	pixs = pixRead("color_white.jpg");
	pixColorFraction(pixs, DARK_THRESH, LIGHT_THRESH, DIFF_THRESH, FACTOR, &res_pixfract, &res_colorfract);
	pixDestroy(&pixs);
	printf("White: %f, %f\n", res_pixfract, res_colorfract);

	pixs = pixRead("color_tiny.jpg");
	pixColorFraction(pixs, DARK_THRESH, LIGHT_THRESH, DIFF_THRESH, FACTOR, &res_pixfract, &res_colorfract);
	pixDestroy(&pixs);
	printf("Tiny: %f, %f\n", res_pixfract, res_colorfract);

	printf("\n\n");

	pixs = pixRead("..\\..\\..\\images\\test-set\\source.jpg");
	pixColorFraction(pixs, DARK_THRESH, LIGHT_THRESH, DIFF_THRESH, FACTOR, &res_pixfract, &res_colorfract);
	pixDestroy(&pixs);
	printf("Source: %f, %f\n", res_pixfract, res_colorfract);

	pixs = pixRead("..\\..\\..\\images\\test-set\\darkened.jpg");
	pixColorFraction(pixs, DARK_THRESH, LIGHT_THRESH, DIFF_THRESH, FACTOR, &res_pixfract, &res_colorfract);
	pixDestroy(&pixs);
	printf("Darkened: %f, %f\n", res_pixfract, res_colorfract);

	pixs = pixRead("..\\..\\..\\images\\test-set\\lightened.jpg");
	pixColorFraction(pixs, DARK_THRESH, LIGHT_THRESH, DIFF_THRESH, FACTOR, &res_pixfract, &res_colorfract);
	pixDestroy(&pixs);
	printf("Lightened: %f, %f\n", res_pixfract, res_colorfract);
	
	getchar();
	return 0;
}

