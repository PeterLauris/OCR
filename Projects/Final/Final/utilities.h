#pragma once

#include <ctime>

#include "opencv2\opencv.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\core\core.hpp"

#include "fann.h"

#include "dirent.h"

#include "utf8.h"

class Utilities {
public:
	static clock_t startTime;
	static void setStartTime();
	static void getTimePassed();
	static int randomInt(int, int);
	static float randomFloat(float, float);

	static std::string convertImageToString(cv::Mat img, bool useSpaces);
};