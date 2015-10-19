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
* scaletest1.c
*
*      scaletest1 filein scalex scaley fileout
*    where
*      scalex, scaley are floating point input
*/

#include "allheaders.h"

int main_scaling(int argc, char **argv)
{
	char        *filein, *fileout;
	l_int32      d;
	l_float32    scalex, scaley;
	PIX         *pixs, *pixd;

	filein = "..\\..\\..\\images\\test-set\\streched.jpg";
	scalex = 1.5;
	scaley = 1;
	fileout = "scaling.result";

	pixs = pixRead(filein);

	/* choose type of scaling operation */

	pixd = pixScale(pixs, scalex, scaley);
	pixWrite("scaling1.png", pixd, IFF_PNG);
	pixDestroy(&pixd);

	pixd = pixScaleLI(pixs, scalex, scaley);
	pixWrite("scaling2.png", pixd, IFF_PNG);
	pixDestroy(&pixd);

	pixd = pixScaleSmooth(pixs, scalex, scaley);
	pixWrite("scaling3.png", pixd, IFF_PNG);
	pixDestroy(&pixd);

	pixd = pixScaleAreaMap(pixs, scalex, scaley);
	pixWrite("scaling4.png", pixd, IFF_PNG);
	pixDestroy(&pixd);

	pixd = pixScaleBySampling(pixs, scalex, scaley);
	pixWrite("scaling5.png", pixd, IFF_PNG);
	pixDestroy(&pixd);

	pixd = pixScaleToGray(pixs, scalex);
	pixWrite("scaling6.png", pixd, IFF_PNG);

	pixDestroy(&pixs);
	pixDestroy(&pixd);
	return 0;
}

