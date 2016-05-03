#include <stdlib.h>
#include <string>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"

//#include "allheaders.h"

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

//main?gie att?la gaišuma noteikšanai
#define DARK_THRESH_LIGHT		0
#define LIGHT_THRESH_LIGHT		155
#define DARK_THRESH_DARK		100
#define LIGHT_THRESH_DARK		255
#define DIFF_THRESH				0
#define FACTOR					4
#define BGVAL_MIDDLE			186
#define BGVAL_DELTA				30

#define MIN_SPACING_PROBABILITY 0.95
#define SPACE_MIN_SPACING_COUNT 6
#define ALLOWED_ITERATION_ERROR 1


struct SpacingGroup {
public:
	int startX;
	int groupSize;

	SpacingGroup(int s) {
		startX = s;
		groupSize = 1;
	}

	void Add() {
		groupSize++;
	}
};

class ImageProcessing {
public:
	static void findWords_cv(cv::Mat);
	static void showImage(cv::Mat, std::string s="Title");
	static void iterateOverImage(cv::Mat);
	static cv::Mat prepareImage(cv::Mat subImg);
	static void cutWords();
	static std::vector<cv::Rect> detectLetters(cv::Mat img);
	static void testFoundSymbols(cv::Mat source, std::vector<SpacingGroup*> spacingGroups, int spacingIterationWidth, int spacingTestWidth);
	static cv::Mat convertImageToBinary(cv::Mat);
	static cv::Mat removeNoise(cv::Mat);
	static cv::Mat deskewImage(cv::Mat);
	static cv::Mat setBrightness(cv::Mat, int);
	static cv::Mat setContrast(cv::Mat);
};