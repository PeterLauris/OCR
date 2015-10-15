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
* pagesegtest1.c
*
*   Use on, e.g.:   feyn1.tif, witten.tif,
*                   pageseg1.tif, pageseg1.tif, pageseg3.tif, pageseg4.tif
*/

#include "allheaders.h"

//izk?rto lapas vien? liel? bild?

int main_pagesegtest1(int    argc,
	char **argv)
{
	PIX         *pixs, *pixhm, *pixtm, *pixtb, *pixd;
	PIXA        *pixa;
	char        *filein;
	static char  mainName[] = "pagesegtest1";


	/* Display intermediate images in a single image */
	pixa = pixaReadFiles("..\\..\\..\\images\\test-set\\", NULL); //NULL var aizvietot ar tekstu, kam j?b?t nosaukum?
	if (pixa == NULL) printf("pixa IS NULL");
	if (pixaGetCount(pixa) == 0) printf("no components %d\n", pixa->n);
	pixd = pixaDisplayTiledAndScaled(pixa, 32, 400, 4, 0, 20, 3);
	if (pixd == NULL) printf("pixd IS NULL");
	pixWrite("..\\..\\..\\images\\results\\pagesegtest1", pixd, IFF_PNG);
	pixaDestroy(&pixa);
	pixDestroy(&pixd);
	printf("\n---\nEND\n");
	getchar();
	return 0;
}

