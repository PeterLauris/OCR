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


#define TRANSFORMATION_COUNT 1 //cik reizes katru bildi pievienos tren?šanas setam (ori?in?ls + pamain?tas)
#define SYMBOL_COUNT 96
//#define SYMBOL_COUNT 4
//#define SYMBOL_COUNT 33

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

	static char *dirLettersName;
	static char *dirSpacingName;
	static char *dirTestName;
	//string dirLetters1bpp = "../../../images/letters/1bpp/";
	//string dirSpacingLetters1bpp = "../../../images/letter-spacing/narrow/invalid/1bpp/";
	//string dirSpacing1bpp = "../../../images/letter-spacing/narrow/valid/1bpp/";
	static std::string dirTest1bpp;
	static std::string dirLettersTrainingSpritemap;
	static std::string dirLettersTestSpritemap;

	static char *imgPath;

	static int recordsCount;
	static RecordInfo *trainingRecords;
	static RecordInfo *testRecords;

	NeuralNetwork() {
		SYMBOLS = new std::string[SYMBOL_COUNT] { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
			"A", "?", "B", "C", "?", "D", "E", "?", "F", "G", "?", "H", "I", "?", "J", "K", "?", "L", "?", "M", "N", "?", "O", "P", "R", "S", "Š", "T", "U", "?", "V", "Z", "Ž",
			"a", "?", "b", "c", "?", "d", "e", "?", "f", "g", "?", "h", "i", "?", "j", "k", "?", "l", "?", "m", "n", "?", "o", "p", "r", "s", "š", "t", "u", "?", "v", "z", "ž",
			"Q", "q", "W", "w", "X", "x", "Y", "y",
			".", ",", "!", "?", "-", "€", "$", "(", ")", "@", "[", "]"
		};

		//const string SYMBOLS[SYMBOL_COUNT] = {
		//	"A", "B", "C", "D"
		//};

		//const string SYMBOLS[SYMBOL_COUNT] = {
		//"a", "?", "b", "c", "?", "d", "e", "?", "f", "g", "?", "h", "i", "?", "j", "k", "?", "l", "?", "m", "n", "?", "o", "p", "r", "s", "š", "t", "u", "?", "v", "z", "ž"
		//};

		recordsCount = 0;

		dirLettersName = "../../../images/letters/training-set/";
		dirSpacingName = "../../../images/letter-spacing/narrow/valid/";
		dirTestName = "../../../images/letter-spacing/narrow/test/";

		dirTest1bpp = "../../../images/letter-spacing/narrow/test/1bpp/";
		dirLettersTrainingSpritemap = "../../../images/letters/training-set-spritesheet.png";
		dirLettersTestSpritemap = "../../../images/letters/test-set-spritesheet.png";
	}

	static void prepareTrainingData();
	static void prepareTestData();
	static std::string convertImageToString(cv::Mat img);
	static void createNNData(int);
	static void trainNN_spacing();
	static void trainNN_letters();
	static void testNN();
	static void readDataset(int);
	static void completeSpritesheet(int);
	static void trainOCR();
	static void trainOCR_quicker();
};