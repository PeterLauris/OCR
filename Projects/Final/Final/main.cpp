#ifndef _MAIN_
#define _MAIN_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

//#include "image_processing.cpp"
#include "neural_network.h"
#include "utilities.h"

#include "utf8.h"



using namespace cv;
using namespace std;

/// Attīra programmas atmiņu
void cleanup() {
	delete[] NeuralNetwork::trainingRecords;
	delete[] NeuralNetwork::testRecords;
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

std::string* NeuralNetwork::SYMBOLS;
char* NeuralNetwork::dirLettersName;
char* NeuralNetwork::dirSpacingName;
char* NeuralNetwork::dirTestName;
std::string NeuralNetwork::dirTest1bpp;
std::string NeuralNetwork::dirLettersTrainingSpritemap;
std::string NeuralNetwork::dirLettersTestSpritemap;
char* NeuralNetwork::imgPath;
int NeuralNetwork::recordsCount;
RecordInfo* NeuralNetwork::trainingRecords;
RecordInfo* NeuralNetwork::testRecords;

clock_t Utilities::startTime;

int main() {

	BOOL ret = SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

	srand(time(NULL));

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
		cin >> c;
		//c = '6';

		switch (c) {
		case '1':

			break;
		case '2':
			NeuralNetwork::prepareTrainingData();
			NeuralNetwork::prepareTestData();
			break;
		case '3':
			NeuralNetwork::createNNData(0);
			break;
		case '4':
			NeuralNetwork::createNNData(1);
			break;
		case '5':
			NeuralNetwork::trainNN_letters();
			break;
		case '6':
			NeuralNetwork::testNN();
			break;
		case 'd':
			//NeuralNetwork::readDataset();
			break;
		case 's':
			//NeuralNetwork::completeSpritesheet();
			break;
		case 'r':
			NeuralNetwork::trainOCR();
			break;
		case 't':
			NeuralNetwork::trainOCR_quicker();
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