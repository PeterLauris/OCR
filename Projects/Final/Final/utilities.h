#pragma once

#include <ctime>

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "fann.h"

#include "dirent.h"

#include "utf8.h"

#define MIN_BLACK_PIXELS 6

class RecordInfo {
public:
	int x;
	int y;
	std::string letter;

	RecordInfo() {
		letter = "";
	}
};

class SymbolResult {
public:
	int symbolIdxs[3]; //3 ticamāko simbolu indeksi simbolu masīvā
	fann_type prob[3]; //3 ticamāko simbolu varbūtības

	SymbolResult() {
		symbolIdxs[0] = -1;
		symbolIdxs[1] = -1;
		symbolIdxs[2] = -1;

		prob[0] = -1;
		prob[1] = -1;
		prob[2] = -1;
	}
};

class Utilities {
public:
	static clock_t startTime;
	static void setStartTime();
	static void getTimePassed();
	static int randomInt(int, int);
	static float randomFloat(float, float);
	static std::string wordToString(std::vector<SymbolResult*>, int*);
	static bool isImageEmpty(cv::Mat);
	static std::string convertImageToString(cv::Mat img, bool useSpaces);
	static std::string convertImageToString_binary(cv::Mat img, bool useSpaces);
};