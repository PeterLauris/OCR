#ifndef _MAIN_
#define _MAIN_


#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

#include "image_processing.h"
#include "neural_network.h"
#include "language_model.h"
#include "utilities.h"

#include "utf8.h"



using namespace cv;
using namespace std;

/// Attīra programmas atmiņu
void cleanup() {
	delete[] NeuralNetwork::trainingRecords_letters;
	delete[] NeuralNetwork::trainingRecords_spacing;
	delete[] NeuralNetwork::testRecords_letters;
	delete[] NeuralNetwork::testRecords_spacing;
}

/// Izsauc cleanup funkciju pie loga aizvēršanas
BOOL WINAPI ConsoleHandlerRoutine(DWORD dwCtrlType)
{
	switch (dwCtrlType) {
	case CTRL_CLOSE_EVENT:
	case CTRL_C_EVENT:
	case CTRL_SHUTDOWN_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_LOGOFF_EVENT:
		cleanup();
		return TRUE;
	}

	return FALSE;
}

std::string* NeuralNetwork::SYMBOLS = new std::string[SYMBOL_COUNT];/*{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
																	"A", "Ā", "B", "C", "Č", "D", "E", "Ē", "F", "G", "Ģ", "H", "I", "Ī", "J", "K", "Ķ", "L", "Ļ", "M", "N", "Ņ", "O", "P", "R", "S", "Š", "T", "U", "Ū", "V", "Z", "Ž",
																	"a", "ā", "b", "c", "č", "d", "e", "ē", "f", "g", "ģ", "h", "i", "ī", "j", "k", "Ķ", "l", "ļ", "m", "n", "ņ", "o", "p", "r", "s", "š", "t", "u", "ū", "v", "z", "ž",
																	"Q", "q", "W", "w", "X", "x", "Y", "y"
																	}*/;


std::string NeuralNetwork::dirLettersTrainingSet = "../../../images/learning/letters/training-set/";
std::string NeuralNetwork::dirSpacingTrainingSet = "../../../images/learning/spacing/narrow/training-set/";
std::string NeuralNetwork::dirSpacingTestSet = "../../../images/learning/spacing/narrow/test/1bpp/";

std::string NeuralNetwork::dirLettersTrainingSpritemap = "../../../images/learning/letters/training-set-spritesheet.png";
std::string NeuralNetwork::dirLettersTestSpritemap = "../../../images/learning/letters/test-set-spritesheet.png";
std::string NeuralNetwork::dirSpacingTrainingSpritemap = "../../../images/learning/spacing/narrow/training-set-spritesheet.png";
std::string NeuralNetwork::dirSpacingTestSpritemap = "../../../images/learning/spacing/narrow/test-set-spritesheet.png";
//std::string NeuralNetwork::dirSpacingValidTrainingSpritemap;
//std::string NeuralNetwork::dirSpacingInvalidTrainingSpritemap;

int NeuralNetwork::recordsCount = 0;
RecordInfo* NeuralNetwork::trainingRecords_letters;
RecordInfo* NeuralNetwork::testRecords_letters;
RecordInfo* NeuralNetwork::trainingRecords_spacing;
RecordInfo* NeuralNetwork::testRecords_spacing;
Ngram* LanguageModel::ngramLM;
LM* LanguageModel::useLM;

clock_t Utilities::startTime;



int main() {
	//LanguageModel::ngramLM = NULL;
	//LanguageModel::useLM = NULL;
	//LanguageModel::getProbability();


	//BOOL ret = SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

	srand(time(NULL));

	/*for (int i = 0; i < 10; i++) {
	cout << Utilities::randomInt(0, 9) << endl;
	}*/

	ifstream in;
	in.open("symbols.in");
	if (in.is_open()) {
		for (int i = 0; i < SYMBOL_COUNT; i++) {
			in >> NeuralNetwork::SYMBOLS[i];
		}
	}

	char c;
	do {
		cout << "### OCR ###\n" <<
			"1 - test\n" <<
			"Make a choice: ";
		//cin >> c;
		c = 'i';

		Mat source, tmp;
		std::vector<cv::Rect> letterBoxes;

		switch (c) {
		case '1':
			source = imread("../../../images/pages/piemers.png");
			//ImageProcessing::showImage(source, "Original");
			source = ImageProcessing::setContrast(source);
			source = ImageProcessing::removeNoise(source);
			source = ImageProcessing::deskewImage(source);
			//imwrite("tmp.jpg", source);
			//source = imread("tmp.jpg");
			tmp = source.clone();
			letterBoxes = ImageProcessing::detectLetters(source);
			cout << "Word boxes found: " << letterBoxes.size() << endl;
			for (int i = 0; i < letterBoxes.size(); i++) {
				//rectangle(tmp, letterBoxes[i], Scalar(0, 0, 0), 2);
				Mat subImg = tmp(letterBoxes[i]);
				ImageProcessing::iterateOverImage(subImg);
				//ImageProcessing::showImage(subImg, "Cutout SubImg");
				subImg.release();
			}
			ImageProcessing::showImage(tmp, "Result 1");
			//imwrite("tmp.jpg", tmp);
			break;
		case '2':
			ImageProcessing::findWords_cv(source);
			break;
		case '3':
			NeuralNetwork::createNNData_letters(0);
			break;
		case '4':
			NeuralNetwork::createNNData_letters(1);
			break;
		case '5':
			NeuralNetwork::trainNN_letters();
			break;
		case '6':
			NeuralNetwork::testNN_letters();
			break;
		case 'd':
			NeuralNetwork::readDataset_spacing(0);
			break;
		case 's':
			NeuralNetwork::completeSpritesheet_letters ();
			break;
		case 'r':
			NeuralNetwork::trainOCR_letters();
			break;
		case 't':
			NeuralNetwork::trainOCR_letters_quicker();
			break;
		case 'i':
			NeuralNetwork::trainOCR_spacing();
			source = imread("../../../images/iteration_test_6.png", CV_8UC1);
			ImageProcessing::showImage(source);
			ImageProcessing::iterateOverImage(source);
			source.release();
			break;
		case 'j':
			NeuralNetwork::trainOCR_letters();
			source = imread("../../../images/iteration_test_6.png");
			ImageProcessing::iterateOverImage(source);
			source.release();
			break;
		case 'k':
			//NeuralNetwork::trainOCR_spacing();
			//ImageProcessing::findWords_cv("../../../images/pages/word_finding_1.png");
			break;
		case 'l':
			ImageProcessing::cutWords();
			break;
		case 'b':
			//ImageProcessing::convertTo1bpp("../../../images/learning/letters/training-set/");
			break;
		default:
			cleanup();
			return 0;
		}
		cout << "Press any key to continue...\n\n";
		getchar();
	} while (true);

	return 0;
}

#endif