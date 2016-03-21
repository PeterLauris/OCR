#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

#include "utilities.h"

#include "opencv2\opencv.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\core\core.hpp"

#include "allheaders.h"

#include "fann.h"

#include "dirent.h"

#include "utf8.h"

#define INPUT_SIZE_LETTERS 1024
#define INPUT_SIZE_SPACING 512
#define TRANSFORMATION_COUNT 10 //cik reizes katru bildi pievienos tren?anas setam (ori?in?ls + pamain?tas)
//#define SYMBOL_COUNT 96
//#define SYMBOL_COUNT 4
//#define SYMBOL_COUNT 33

#define DEFORMATION_AMOUNT 0.13

#define SYMBOL_COUNT 10

#define SORTING_VECTOR_COUNT 1000

class RecordInfo {
public:
	int x;
	int y;
	std::string letter;

	RecordInfo() {
		letter = "";
	}
};

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
		/*SYMBOLS = new std::string[SYMBOL_COUNT] { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
			"A", "?", "B", "C", "?", "D", "E", "?", "F", "G", "?", "H", "I", "?", "J", "K", "?", "L", "?", "M", "N", "?", "O", "P", "R", "S", "", "T", "U", "?", "V", "Z", "",
			"a", "?", "b", "c", "?", "d", "e", "?", "f", "g", "?", "h", "i", "?", "j", "k", "?", "l", "?", "m", "n", "?", "o", "p", "r", "s", "", "t", "u", "?", "v", "z", "",
			"Q", "q", "W", "w", "X", "x", "Y", "y",
			".", ",", "!", "?", "-", "", "$", "(", ")", "@", "[", "]"
		};*/

		//const string SYMBOLS[SYMBOL_COUNT] = {
		//	"A", "B", "C", "D"
		//};

		//const string SYMBOLS[SYMBOL_COUNT] = {
		//"a", "?", "b", "c", "?", "d", "e", "?", "f", "g", "?", "h", "i", "?", "j", "k", "?", "l", "?", "m", "n", "?", "o", "p", "r", "s", "", "t", "u", "?", "v", "z", ""
		//};
	}

	static void prepareTrainingData();
	static void prepareTestData();
	static void createNNData_spacing(int);
	static void createNNData_letters(int);
	static void trainNN_spacing();
	static void trainNN_letters();
	static void testNN_letters();
	static void testNN_spacing();
	static void testNN_image_spacing(cv::Mat);
	static void readDataset_letters(int);
	static void readDataset_spacing(int);
	static void completeSpritesheet_letters(int t = 0);
	static void trainOCR_letters();
	static void trainOCR_letters_quicker();
	static void trainOCR_spacing();
	static void trainOCR_spacing_quicker();
};