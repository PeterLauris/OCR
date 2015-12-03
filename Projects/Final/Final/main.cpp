#include <iostream>
#include <fstream>

#include "opencv2\opencv.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\core\core.hpp"

#include "allheaders.h"

#include "fann.h"

#include "dirent.h"

using namespace cv;
using namespace std;

void OCR() {
	printf("Start OCR");
}

struct RecordInfo
{
	int x;
	int y;
	string letter = "";
};


char *dirLettersName = "../../../images/letter-spacing/narrow/invalid/";
char *dirSpacingName = "../../../images/letter-spacing/narrow/valid/";
char *dirTestName = "../../../images/letter-spacing/narrow/test/";
string dirLetters1bpp = "../../../images/letter-spacing/narrow/invalid/1bpp/";
string dirSpacing1bpp = "../../../images/letter-spacing/narrow/valid/1bpp/";
string dirTest1bpp = "../../../images/letter-spacing/narrow/test/1bpp/";
char *imgPath;

RecordInfo *records = NULL;

void prepareTrainingData() {
	SARRAY *safiles = getSortedPathnamesInDirectory(dirLettersName, NULL, 0, 0);
	l_int32 nfiles = sarrayGetCount(safiles);

	PIX *pixs, *pixt;
	char targetPath[1024];
	char newName[512];

	for (l_int32 i = 0; i < nfiles; i++) {
		imgPath = sarrayGetString(safiles, i, 0);
		if ((pixs = pixRead(imgPath)) == NULL) {
			printf("image file %s not read\n", imgPath);
			continue;
		}

		pixt = pixConvertTo1(pixs, 150);
		sprintf_s(newName, "%s.%05d.png", "s", i);
		sprintf_s(targetPath, "%s%s%s", dirLettersName, "1bpp/", newName);
		pixWrite(targetPath, pixt, IFF_PNG);

		pixDestroy(&pixs);
		pixDestroy(&pixt);
	}
	sarrayDestroy(&safiles);


	safiles = getSortedPathnamesInDirectory(dirSpacingName, NULL, 0, 0);
	nfiles = sarrayGetCount(safiles);

	for (l_int32 i = 0; i < nfiles; i++) {
		imgPath = sarrayGetString(safiles, i, 0);
		if ((pixs = pixRead(imgPath)) == NULL) {
			printf("image file %s not read\n", imgPath);
			continue;
		}

		//TODO error when	 converting to 1bpp
		//pixt = pixConvertTo1(pixs, 150);
		sprintf_s(newName, "%s.%05d.png", "s", i);
		sprintf_s(targetPath, "%s%s%s", dirSpacingName, "1bpp/", newName);
		pixWrite(targetPath, pixs, IFF_PNG);

		pixDestroy(&pixs);
		//pixDestroy(&pixt);
	}
	sarrayDestroy(&safiles);


	cout << "Created 1bpp images of spacing images\n";
}

void prepareTestData() {
	SARRAY *safiles = getSortedPathnamesInDirectory(dirTestName, NULL, 0, 0);
	l_int32 nfiles = sarrayGetCount(safiles);

	PIX *pixs, *pixt;
	char targetPath[1024];
	char newName[512];

	for (l_int32 i = 0; i < nfiles; i++) {
		imgPath = sarrayGetString(safiles, i, 0);
		if ((pixs = pixRead(imgPath)) == NULL) {
			printf("image file %s not read\n", imgPath);
			continue;
		}

		pixt = pixConvertTo1(pixs, 150);
		sprintf_s(newName, "%s.%05d.png", "s", i);
		sprintf_s(targetPath, "%s%s%s", dirTestName, "1bpp/", newName);
		pixWrite(targetPath, pixt, IFF_PNG);

		pixDestroy(&pixs);
		pixDestroy(&pixt);
	}
	sarrayDestroy(&safiles);

	cout << "Created 1bpp images of spacing test images\n";
}

void createTrainingData() {
	cout << "Creating training data..." << endl;

	ofstream fout("train_spacing.in");

	if (fout.is_open()) {
		int validFileCount = 0;
		int inputSize = 0;
		string trainingDataContent = "";

		DIR *dirLetters = opendir(dirLetters1bpp.c_str());
		DIR *dirSpacing = opendir(dirSpacing1bpp.c_str());
		string imgName;
		struct dirent *ent;
		if (dirLetters != NULL && dirSpacing != NULL) {
			while ((ent = readdir(dirLetters)) != NULL) {
				string imgPath(dirLetters1bpp + ent->d_name);
				cout << imgPath << "\n";

				Mat img = imread(imgPath);
				if (img.empty()) {
					cout << "Cannot load " << imgName << endl;
					continue;
				}
				//cvtColor(img, img, CV_BGR2GRAY);
				validFileCount++;
				inputSize = img.rows * img.cols;

				int *arr = new int[img.rows * img.cols];
				for (int y = 0; y < img.rows; y++) {
					for (int x = 0; x < img.cols; x++) {
						Vec3b intensity = img.at<Vec3b>(y, x);
						int t = (intensity[0] <= 128 ? 0 : 1);
						arr[y*img.cols + x] = t;
						trainingDataContent = trainingDataContent + to_string(t) + " ";
					}
				}
				delete[] arr;
				trainingDataContent += "\n0\n"; //is not space

				//namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
				//imshow("MyWindow", img);
				//waitKey(0);
				//destroyWindow("MyWindow");

				img.release();
			}

			while ((ent = readdir(dirSpacing)) != NULL) {
				string imgPath(dirSpacing1bpp + ent->d_name);
				cout << imgPath << "\n";

				Mat img = imread(imgPath);
				if (img.empty()) {
					cout << "Cannot load " << imgName << endl;
					continue;
				}
				validFileCount++;
				inputSize = img.rows * img.cols;

				int *arr = new int[img.rows * img.cols];
				for (int y = 0; y < img.rows; y++) {
					for (int x = 0; x < img.cols; x++) {
						Vec3b intensity = img.at<Vec3b>(y, x);
						int t = (intensity[0] <= 128 ? 0 : 1);
						arr[y*img.cols + x] = t;
						trainingDataContent = trainingDataContent + to_string(t) + " ";
					}
				}
				delete[] arr;
				trainingDataContent += "\n1\n"; //is not space

				img.release();
			}

			closedir(dirLetters);
			closedir(dirSpacing);
		}
		else {
			cout << "not present" << endl;
		}

		fout << validFileCount << " " << inputSize << " " << 1 << "\n" << trainingDataContent;
		fout.close();
	}
}

void createTestData() {
	cout << "Creating test data..." << endl;

	ofstream fout("test_spacing.in");

	if (fout.is_open()) {
		int validFileCount = 0;
		int inputSize = 0;
		string trainingDataContent = "";

		DIR *dirTest = opendir(dirTest1bpp.c_str());
		string imgName;
		struct dirent *ent;
		if (dirTest != NULL) {
			while ((ent = readdir(dirTest)) != NULL) {
				string imgPath(dirTest1bpp + ent->d_name);
				cout << imgPath << "\n";

				Mat img = imread(imgPath);
				if (img.empty()) {
					cout << "Cannot load " << imgName << endl;
					continue;
				}
				//cvtColor(img, img, CV_BGR2GRAY);
				validFileCount++;
				inputSize = img.rows * img.cols;

				int *arr = new int[img.rows * img.cols];
				for (int y = 0; y < img.rows; y++) {
					for (int x = 0; x < img.cols; x++) {
						Vec3b intensity = img.at<Vec3b>(y, x);
						int t = (intensity[0] <= 128 ? 0 : 1);
						arr[y*img.cols + x] = t;
						trainingDataContent = trainingDataContent + to_string(t) + " ";
					}
				}
				delete[] arr;

				if (validFileCount > 16)
					trainingDataContent += "\n1\n";
				else
					trainingDataContent += "\n0\n";

				img.release();
			}
			closedir(dirTest);
		}
		else {
			cout << "not present" << endl;
		}

		fout << validFileCount << " " << inputSize << " " << 1 << "\n" << trainingDataContent;
		fout.close();
	}
}

void trainNN() {
	const unsigned int num_input = 512;
	const unsigned int num_output = 1;
	const unsigned int num_layers = 15;
	const unsigned int num_neurons_hidden = 3;
	const float desired_error = (const float) 0.001;
	const unsigned int max_epochs = 500000;
	const unsigned int epochs_between_reports = 100;


	unsigned int layers[4] = { num_input, 5, 6, 1 };
	struct fann *ann = fann_create_standard_array(4, layers); //fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);


	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_train_on_file(ann, "train_spacing.in", max_epochs, epochs_between_reports, desired_error);

	fann_save(ann, "result_spacing.net");

	fann_destroy(ann);

	getchar();
}

void testNN() {
	struct fann *ann = fann_create_from_file("result_spacing.net");

	struct fann_train_data *data = fann_read_train_from_file("test_spacing.in");
	fann_reset_MSE(ann);
	fann_test_data(ann, data);
	printf("Mean Square Error: %f\n", fann_get_MSE(ann));
	fann_destroy_train(data);
}

void readDataset() {
	ifstream in("../../../images/letter_spritesheet.txt");
	ofstream out("../../../images/test_result.txt");

	if (in.is_open()) {
		printf("File opened successfully\n");


		int lineCount = 0;
		string line;
		while (getline(in, line)) lineCount++;
		cout << lineCount << endl;

		records = new RecordInfo[lineCount];

		in.clear();
		in.seekg(0, ios::beg);


		bool readingCoords = false;
		bool readingLetter = false;
		int coordIdx = 0;
		int num = -1;
		string word;

		while (in >> word) {
			if (word == "=") {
				if (readingCoords) {
					readingCoords = false;
					readingLetter = true;
				}
				else if (readingLetter) {
					printf("Shouldn\'t be here...\n");
				}
				else {
					readingCoords = true;
					coordIdx = 0;
					num++;
				}
			}
			else {
				if (readingCoords) {
					if (coordIdx == 0)
						records[num].x = stoi(word);
					else if (coordIdx == 1)
						records[num].y = stoi(word);
					//cout << "Coord [" << coordIdx << "]: " << stoi(word) << " ";
					coordIdx++;
				}
				else if (readingLetter) {
					records[num].letter = word;
					//cout << "Letter : " << word << "\n";
					readingLetter = false;
				}
			}
		}

		for (int i = 0; i < lineCount; i++) {
			cout << "(" << records[i].x << ";" << records[i].y << ") --> " << records[i].letter << endl;
		}
	}
	else {
		printf("Failed to open the file\n");
	}

	out.close();
	in.close();

	/*sqlite3 *db;
	int rc = sqlite3_open("test.db", &db);
	if (rc == SQLITE_OK) {
		printf("Database opened successfully\n");
	}
	else {
		printf("Failed to open the database\n");
	}
	sqlite3_close(db);*/
}

void cleanup() {
	delete[] records;
}

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

int main() {
	//test FANN
	//struct fann *ann = fann_create_standard(3, 1, 3, 1);
	//fann_destroy(ann);

	//test leptonica
	//PIX *pixs;


	//test opencv
	//Mat img = imread("b&w.jpg", CV_LOAD_IMAGE_UNCHANGED);

	BOOL ret = SetConsoleCtrlHandler(ConsoleHandlerRoutine, TRUE);

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
				"Make a choice: ";
		cin >> c;
		//c = '6';

		switch (c) {
		case '1':
			OCR();
			break;
		case '2':
			prepareTrainingData();
			prepareTestData();
			break;
		case '3':
			createTrainingData();
			break;
		case '4':
			createTestData();
			break;
		case '5':
			trainNN();
			break;
		case '6':
			testNN();
			break;
		case 'd':
			readDataset();
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