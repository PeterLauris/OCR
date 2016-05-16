#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

#include "utilities.h"

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

#include "fann.h"

#include "dirent.h"

#include "utf8.h"

#define LETTER_WIDTH 32
#define LETTER_HEIGHT 32
#define SPACING_WIDTH 8
#define SPACING_HEIGHT 32
#define INPUT_SIZE_LETTERS LETTER_WIDTH*LETTER_HEIGHT //32x32
#define INPUT_SIZE_SPACING SPACING_WIDTH*SPACING_HEIGHT //8x32

#define SYMBOL_COUNT 85
//#define SYMBOL_COUNT 99
//#define SYMBOL_COUNT 4
//#define SYMBOL_COUNT 33
//#define SYMBOL_COUNT 10
//#define SYMBOL_COUNT 84

#define TRANSFORMATION_COUNT_SPACING 1 //cik reizes katru bildi pievienos trenēšanas setam (oriģināls + pamainītas)
#define TRANSFORMATION_COUNT_LETTER 2
#define DEFORMATION_AMOUNT_SPACING 0.0001
#define DEFORMATION_AMOUNT_LETTER 0.07

#define SORTING_VECTOR_COUNT 1000

#define VALID_PROBABILITY 0.98

class NeuralNetwork {
public:
	static std::string *SYMBOLS;

	static std::string dirLettersTrainingSet;

	//vai vajag?
	static std::string dirSpacingTrainingSet;
	static std::string dirSpacingTestSet;

	static std::string dirLettersTrainingSpritemap;
	static std::string dirLettersTestSpritemap;
	static std::string dirSpacingTrainingSpritemap;
	static std::string dirSpacingTestSpritemap;

	//static char *imgPath;

	static int recordsCount;
	static RecordInfo *trainingRecords_letters;
	static RecordInfo *testRecords_letters;
	static RecordInfo *trainingRecords_spacing;
	static RecordInfo *testRecords_spacing;

	NeuralNetwork() {
		
	}

	static void createNNData_spacing(int);
	static void createNNData_letters(int);
	static void trainNN_spacing();
	static void testNN_spacing();
	static void testNN_image_spacing(cv::Mat, int &idx, double &prob, fann *ann);
	static void trainNN_letters();
	static void testNN_letters();
	static SymbolResult * testNN_image_letter(cv::Mat, fann *ann);
	static void readDataset_letters(int);
	static void readDataset_spacing(int);
	static void completeSpritesheet_letters(int t = 0);
	static void trainOCR_letters();
	static void trainOCR_letters_quicker();
	static void trainOCR_spacing();
};