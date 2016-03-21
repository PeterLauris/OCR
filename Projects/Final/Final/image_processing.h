#include "allheaders.h"
#include <stdlib.h>
#include <string>

#include "opencv2\opencv.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\core\core.hpp"

#include "allheaders.h"

#include "fann.h"

#include "dirent.h"

#include "utf8.h"

#define MIN_WORD_WIDTH			6
#define MIN_WORD_HEIGHT			4
#define MAX_WORD_WIDTH			800
#define MAX_WORD_HEIGHT			93

#define   BUF_SIZE				1024

/* select additional debug output */
#define   RENDER_PAGES          1
#define   RENDER_DEBUG			1

#define   BINARY_THRESHOLD		130

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

#define L_BUF_SIZE				512

#define MIN_JUMP				2
#define MIN_REVERSAL			3

#define  GAMMA_IMG				1 //no enhancement
#define  MINVAL					50
#define  MAXVAL					200
#define  CONTRAST_FACTOR		0.1

#define UNSHARP_HALF_WIDTH		20 //1, 2, ...
#define UNSHARP_FRACT			0.4 //0.2 < fract < 0.7

//main?gie att?la gai�uma noteik�anai
#define DARK_THRESH_LIGHT		0
#define LIGHT_THRESH_LIGHT		155
#define DARK_THRESH_DARK		100
#define LIGHT_THRESH_DARK		255
#define DIFF_THRESH				0
#define FACTOR					4
#define BGVAL_MIDDLE			186
#define BGVAL_DELTA				30

class ImageProcessing {
public:
	static l_int32 getLightingBGval(PIX *pixs);
	static l_int32 light_pixDisplayWriteFormat(PIX *pixs, l_int32  reduction, l_int32  format);
	static PIX * normalizeLighting(PIX *pixs, l_int32 *lightingVal);
	static PIX * removeNoise(PIX *pixs, l_int32 *lightingVal);
	static void PixAddEdgeData(PIXA *pixa, PIX *pixs, l_int32 side, l_int32 minjump, l_int32  minreversal);
	static PIX * removeImages(PIX *pixs);
	static PIX * findWords(PIX *pixt1, BOXAA *baa, NUMAA *naa);
	static void writeWords(PIX *pixs, char *dirout, char *rootname, l_int32 nr);
	static void findWords2(char* dirin, char *dirout);
	static void iterateOverImage();
};