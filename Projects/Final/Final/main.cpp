#ifndef _MAIN_
#define _MAIN_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

#include "image_processing.h"
#include "neural_network.h"
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

std::string* NeuralNetwork::SYMBOLS = new std::string[SYMBOL_COUNT]{ "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
std::string NeuralNetwork::dirLettersTrainingSet				=	"../../../images/learning/letters/training-set/";
std::string NeuralNetwork::dirSpacingTrainingSet				=	"../../../images/learning/spacing/narrow/training-set/";
std::string NeuralNetwork::dirSpacingTestSet					=	"../../../images/learning/spacing/narrow/test/1bpp/";

std::string NeuralNetwork::dirLettersTrainingSpritemap			=	"../../../images/learning/letters/training-set-spritesheet.png";
std::string NeuralNetwork::dirLettersTestSpritemap				=	"../../../images/learning/letters/test-set-spritesheet.png";
std::string NeuralNetwork::dirSpacingTrainingSpritemap			=	"../../../images/learning/spacing/narrow/training-set-spritesheet.png";
std::string NeuralNetwork::dirSpacingTestSpritemap				=	"../../../images/learning/spacing/narrow/test-set-spritesheet.png";
//std::string NeuralNetwork::dirSpacingValidTrainingSpritemap;
//std::string NeuralNetwork::dirSpacingInvalidTrainingSpritemap;

int NeuralNetwork::recordsCount = 0;
RecordInfo* NeuralNetwork::trainingRecords_letters;
RecordInfo* NeuralNetwork::testRecords_letters;
RecordInfo* NeuralNetwork::trainingRecords_spacing;
RecordInfo* NeuralNetwork::testRecords_spacing;

clock_t Utilities::startTime;

int main() {

	BOOL ret = SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

	srand(time(NULL));

	/*for (int i = 0; i < 10; i++) {
		cout << Utilities::randomInt(0, 9) << endl;
	}*/

	char c;
	do {
		cout << "### OCR ###\n" <<
				"1 - run OCR on sample data\n" <<
				"2 - prepare training and test data\n" <<
				"3 - create training data\n" <<
				"4 - create test data\n" <<
				"5 - train neural network\n" <<
				"6 - test neural network\n" <<
				"d - create dataset\n" <<
				"s - complete spritemap\n" <<
				"r - TRAIN\n" <<
				"t - TRAIN QUICK\n" <<
				"Make a choice: ";
		//cin >> c;
		c = 'j';

		switch (c) {
		case '1':
			NeuralNetwork::trainOCR_spacing();
			break;
		case '2':
			NeuralNetwork::prepareTrainingData();
			NeuralNetwork::prepareTestData();
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
			ImageProcessing::iterateOverImage();
			break;
		case 'j':
			//NeuralNetwork::trainOCR_letters();
			ImageProcessing::iterateOverImage();
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