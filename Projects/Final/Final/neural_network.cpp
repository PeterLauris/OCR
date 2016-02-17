#ifndef _NN_
#define _NN_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

#include "neural_network.h"
#include "utilities.h"


using namespace std;
using namespace cv;


void NeuralNetwork::prepareTrainingData() {
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
		pixt = pixConvertTo1(pixs, 150);
		sprintf_s(newName, "%s.%05d.png", "s", i);
		sprintf_s(targetPath, "%s%s%s", dirSpacingName, "1bpp/", newName);
		pixWrite(targetPath, pixs, IFF_PNG);

		pixDestroy(&pixs);
		//pixDestroy(&pixt);
	}
	sarrayDestroy(&safiles);


	cout << "Created 1bpp images of spacing images\n";
}

void NeuralNetwork::prepareTestData() {
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

/*void createTrainingData() {
cout << "Creating training data..." << endl;

ofstream fout("train_spacing.in");

if (fout.is_open()) {
int validFileCount = 0;
int inputSize = 0;
string trainingDataContent = "";

DIR *dirLetters = opendir(dirSpacingLetters1bpp??.c_str());
DIR *dirSpacing = opendir(dirSpacing1bpp.c_str());
string imgName;
struct dirent *ent;
if (dirLetters != NULL && dirSpacing != NULL) {
while ((ent = readdir(dirLetters)) != NULL) {
string imgPath(dirSpacingLetters1bpp?? + ent->d_name);
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
}*/

string NeuralNetwork::convertImageToString(Mat img) {
	string trainingDataContent = "";
	int inputSize = img.rows * img.cols;
	int *arr = new int[inputSize];
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			Vec3b intensity = img.at<Vec3b>(y, x);
			int t = (intensity[0] <= 128 ? 0 : 1);
			arr[y*img.cols + x] = t;
			trainingDataContent = trainingDataContent + to_string(t) + " ";
			//cout << t << " ";
		}
		//cout << "\n";
	}
	delete[] arr;
	//getchar();
	return trainingDataContent;
}

void NeuralNetwork::createNNData(int type = 0) {
	string imgPath = "";

	ofstream out;
	if (type == 0) {
		out.open("train_letters.in");
		imgPath = NeuralNetwork::dirLettersTrainingSpritemap.c_str();
		cout << "Creating training data..." << endl;
	}
	else if (type == 1) {
		out.open("test_letters.in");
		imgPath = NeuralNetwork::dirLettersTestSpritemap.c_str();
		cout << "Creating test data..." << endl;
	}
	else {
		cerr << "ERROR: Invalid type\n";
		exit(1);
	}

	if (out.is_open()) {
		int validFileCount = 0;
		int inputSize = 1024; // 32*32
		string dataContent = "";

		cout << imgPath << "\n";

		Mat img = imread(imgPath);
		if (img.empty()) {
			cout << "Cannot load " << imgPath << endl;
			return;
		}
		//cvtColor(img, img, CV_BGR2GRAY);

		RecordInfo* typeRecords;
		if (type == 0) typeRecords = NeuralNetwork::trainingRecords;
		else if (type == 1) typeRecords = NeuralNetwork::testRecords;
		else {
			exit(1);
		}

		int repCount = (type == 0) ? TRANSFORMATION_COUNT : 1;


		Point2f srcTri[3];
		Point2f dstTri[3];
		Mat rot_mat(2, 3, CV_32FC1);
		Mat warp_mat(2, 3, CV_32FC1);
		Mat warp_dst, warp_rotate_dst;


		for (int i = 0; i < recordsCount; i++) {
			Mat subImg = img(cv::Range(typeRecords[i].y, typeRecords[i].y + 32), cv::Range(typeRecords[i].x, typeRecords[i].x + 32));
			Mat subInvImg;
			bitwise_not(subImg, subInvImg);
			srcTri[0] = Point2f(0, 0);
			srcTri[1] = Point2f(subImg.cols - 1, 0);
			srcTri[2] = Point2f(0, subImg.rows - 1);

			//cout << "Current letter: " << trainingRecords[i].letter << endl;

			for (int rep = 0; rep < repCount; rep++) {
				string resString = "";

				if (rep > 0) { //pirmaj? iter?cij? ori?in?lu nemaina
							   //randomly modify subImg
					warp_dst = Mat::zeros(subInvImg.rows, subInvImg.cols, subInvImg.type());

					dstTri[0] = Point2f(subInvImg.cols*Utilities::randomFloat(0, 0.15f), subInvImg.rows*Utilities::randomFloat(0, 0.15f));
					dstTri[1] = Point2f(subInvImg.cols*Utilities::randomFloat(0.85f, 1), subInvImg.rows*Utilities::randomFloat(0, 0.15f));
					dstTri[2] = Point2f(subInvImg.cols*Utilities::randomFloat(0, 0.15f), subInvImg.rows*Utilities::randomFloat(0.85f, 1));
					//TODO trešais?

					warp_mat = getAffineTransform(srcTri, dstTri);

					//warp_dst.setTo(255);
					warpAffine(subInvImg, warp_dst, warp_mat, warp_dst.size());

					bitwise_not(warp_dst, warp_dst);

					//TODO vai vajag izt?r?t atmi?u??

					/*namedWindow("WarpWindow", CV_WINDOW_AUTOSIZE);
					imshow("WarpWindow", warp_dst);
					waitKey(0);
					destroyWindow("WarpWindow");*/

					int erosion_type;
					int erosion_elem = 0 + (rand() % (int)(2 - 0 + 1));
					int erosion_size = (0 + (rand() % (int)(2 - 0 + 1))) / 2; //66%, ka 0

					Mat erosion_dst;
					if (erosion_elem == 0) { erosion_type = MORPH_RECT; }
					else if (erosion_elem == 1) { erosion_type = MORPH_CROSS; }
					else if (erosion_elem == 2) { erosion_type = MORPH_ELLIPSE; }

					Mat element = getStructuringElement(erosion_type,
						Size(2 * erosion_size + 1, 2 * erosion_size + 1),
						Point(erosion_size, erosion_size));

					erode(warp_dst, erosion_dst, element);
					//dilate pagaid?m neizmantoju, jo burti p?r?k v?ji

					/*namedWindow("ErodeWindow", CV_WINDOW_AUTOSIZE);
					imshow("ErodeWindow", erosion_dst);
					waitKey(0);
					destroyWindow("ErodeWindow");*/

					/// Apply the erosion operation
					erode(warp_dst, erosion_dst, element);

					resString = convertImageToString(erosion_dst);
				}
				else {
					resString = convertImageToString(subImg);
				}

				dataContent += resString + "\n";
				for (int j = 0; j < SYMBOL_COUNT; j++) {
					if (typeRecords[i].letter == SYMBOLS[j]) {
						dataContent += "1 ";
					}
					else {
						dataContent += "-1 ";
					}
				}
				dataContent += "\n";
			}

			/*namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
			imshow("MyWindow", subImg);
			waitKey(0);
			destroyWindow("MyWindow");*/

			subImg.release();
		}


		img.release();

		out << (recordsCount*repCount) << " " << inputSize << " " << SYMBOL_COUNT << "\n" << dataContent;
		out.close();
	}
}

/*void createTestData() {
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

//fout << validFileCount << " " << inputSize << " " << 1 << "\n" << trainingDataContent;
fout.close();
}
}*/

void NeuralNetwork::trainNN_spacing() {
	const unsigned int num_input = 512;
	const unsigned int num_output = 1;
	const unsigned int num_layers = 15;
	const unsigned int num_neurons_hidden = 3;
	const float desired_error = (const float) 0.001;
	const unsigned int max_epochs = 500000;
	const unsigned int epochs_between_reports = 100;


	unsigned int layers[4] = { num_input, 6, 6, SYMBOL_COUNT };
	struct fann *ann = fann_create_standard_array(4, layers); //fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);


	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_train_on_file(ann, "train_spacing.in", max_epochs, epochs_between_reports, desired_error);

	fann_save(ann, "result_spacing.net");

	fann_destroy(ann);

	getchar();
}

void NeuralNetwork::trainNN_letters() {
	const unsigned int num_input = 1024;
	const unsigned int num_layers = 10;
	const unsigned int max_epochs = 500000;
	const unsigned int epochs_between_reports = 20;
	const float desired_error = 0.0009;//0.0015;

	cout << "Training..." << endl;

	unsigned int layers[num_layers] = { num_input, 18, 18, 18, 118, 18, 18, 18, 18, SYMBOL_COUNT };
	struct fann *ann = fann_create_standard_array(num_layers, layers); //fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
	fann_randomize_weights(ann, -0.2, 0.2);

	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);

	fann_train_on_file(ann, "train_letters.in", max_epochs, epochs_between_reports, desired_error);

	fann_save(ann, "result_letters.net");

	fann_destroy(ann);

	getchar();
}

void NeuralNetwork::testNN() {
	cout << "Testing..." << endl;
	struct fann *ann = fann_create_from_file("result_letters.net");

	//struct fann_train_data *data = fann_read_train_from_file("test_letters.in");
	//fann_reset_MSE(ann);
	//fann_test_data(ann, data);
	//printf("Mean Square Error: %f\n", fann_get_MSE(ann));
	//fann_destroy_train(data);

	fann_type calc_out[4];
	fann_type input[1024];
	fann_type expected_out[SYMBOL_COUNT];

	ifstream in("test_letters.in");
	string word;
	in >> word;
	int sampleCount = stoi(word);
	in >> word; //1024
	int inputCount = stoi(word);
	in >> word;
	int outputCount = stoi(word);

	for (int i = 0; i < sampleCount; i++) {
		for (int j = 0; j < inputCount; j++) {
			in >> word;
			input[j] = stoi(word);
		}
		for (int j = 0; j < outputCount; j++) {
			in >> word;
			expected_out[j] = stoi(word);
		}
		fann_type* tmp = fann_run(ann, input);
		tmp = fann_run(ann, input);
		tmp = fann_run(ann, input);
		tmp = fann_run(ann, input);
		tmp = fann_run(ann, input);
		for (int k = 0; k < 4; k++) {
			calc_out[k] = tmp[k];
		}
		bool result = fann_test(ann, input, expected_out);
		cout << "Input " << i << ":\n"; //  << " (" << (result ? "TRUE" : "FALSE")
		for (int j = 0; j < outputCount; j++) {
			cout << SYMBOLS[j] << ": " << calc_out[j] << " \t" << expected_out[j] << "\n";
		}
		cout << "\n---------\n";
		//cin >> word;
	}
	in.close();
}

//Nolasa spritesheet
void NeuralNetwork::readDataset(int type = 0) {
	ifstream in;
	if (type == 0) {
		in.open("../../../images/letters/training-set-spritesheet.txt");
		cout << "Create training dataset" << endl;
	}
	else if (type == 1) {
		in.open("../../../images/letters/test-set-spritesheet.txt");
		cout << "Create test dataset" << endl;
	}
	else {
		cerr << "ERROR: Invalid type\n";
		exit(1);
	}

	if (in.is_open()) {
		//tiek noskaidrots rindi?u skaits
		int lineCount = 0;
		string line;
		while (getline(in, line)) lineCount++;
		//cout << lineCount << endl;

		RecordInfo* typeRecords;

		recordsCount = lineCount;
		typeRecords = new RecordInfo[lineCount];

		in.clear();
		in.seekg(0, ios::beg);

		bool readingCoords = false;
		bool readingLetter = false;
		int coordIdx = 0;
		int num = -1;
		string word;

		while (in >> word) {
			if (word == "=") {
				if (!readingCoords && !readingLetter) {
					readingLetter = true;
				}
			}
			else {
				if (readingCoords) {
					if (coordIdx >= 4) {
						readingCoords = false;
						continue;
					}
					if (coordIdx == 0)
						typeRecords[num].x = stoi(word);
					else if (coordIdx == 1)
						typeRecords[num].y = stoi(word);
					//cout << "Coord [" << coordIdx << "]: " << stoi(word) << "\n";
					coordIdx++;
				}
				else if (readingLetter) {
					num++;
					typeRecords[num].letter = word;
					//cout << "Letter : " << word << "\n";
					readingLetter = false;
					readingCoords = true;
					coordIdx = 0;
				}
			}
		}

		if (type == 0) NeuralNetwork::trainingRecords = typeRecords;
		else if (type == 1) NeuralNetwork::testRecords = typeRecords;

		/*for (int i = 0; i < lineCount; i++) {
		cout << "(" << trainingRecords[i].x << ";" << trainingRecords[i].y << ") --> " << trainingRecords[i].letter << endl;
		}*/
	}
	else {
		printf("Failed to open the data file\n");
	}
	in.close();
}

//Pievieno spritesheet bild?m atbilstošos burtus
void NeuralNetwork::completeSpritesheet(int type = 0) {
	cout << "Complete spritesheet" << endl;
	string newString = "";
	string word;

	bool firstLine = true;

	ifstream in;

	if (type == 0) in.open("../../../images/letters/training-set-spritesheet-original.txt");
	else if (type == 1) in.open("../../../images/letters/test-set-spritesheet-original.txt");
	else {
		cerr << "ERROR: Invalid type\n";
		exit(1);
	}

	while (in >> word) {
		//cout << word << " ";
		int _count = count(word.begin(), word.end(), '_');
		if (_count == 1) {
			if (!firstLine)
				newString.append("\r\n");
			newString.append(word);
			newString.append(" = ");

			string letter = word.substr(0, word.find('_'));
			int letterSize = utf8::distance(letter.begin(), letter.end());
			if (letterSize == 1)
				newString.append(letter);
			else {
				string tmp = letter.substr(0, letterSize / 2);
				if (utf8::is_valid(tmp.begin(), tmp.end())) {
					newString.append(tmp);
				}
				else {
					newString.append(letter.substr(0, 2));
				}
			}

			newString.append(" ");
		}
		else if (_count == 2) { //special characters
			if (!firstLine)
				newString.append("\r\n");
			newString.append(word);

			if (word.substr(1, 4) == "jaut") {
				//cout << "Jautajuma zime!\n" << endl;
				newString.append(" = ? ");
			}
			else if (word.substr(1, 4) == "kols") {
				//cout << "Kols!\n" << endl;
				newString.append(" = : ");
			}
			else if (word.substr(1, 4) == "pedi") {
				//cout << "Pedinas!\n" << endl;
				newString.append(" = \" ");
			}
			else if (word.substr(1, 4) == "punk") {
				//cout << "Punkts!\n" << endl;
				newString.append(" = . ");
			}
		}
		else {
			newString.append(word);
			newString.append(" ");
		}

		firstLine = false;
	}
	in.close();
	//cout << newString << endl;

	ofstream out;
	if (type == 0) out.open("../../../images/letters/training-set-spritesheet.txt", std::ios::out | std::ios::binary);
	else if (type == 1) out.open("../../../images/letters/test-set-spritesheet.txt", std::ios::out | std::ios::binary);

	out << newString;
	out.close();
}

/// Izsauc nepieciešam?s funkcijas OCR tren?šanai
void NeuralNetwork::trainOCR() {
	completeSpritesheet();
	readDataset();
	createNNData();
	trainNN_letters();

	completeSpritesheet(1);
	readDataset(1);
	createNNData(1);
	testNN();
}

void NeuralNetwork::trainOCR_quicker() {
	Utilities::setStartTime();
	readDataset();
	Utilities::getTimePassed();
	/*setStartTime();
	createNNData();
	getTimePassed();*/
	Utilities::setStartTime();
	trainNN_letters();
	Utilities::getTimePassed();
	Utilities::setStartTime();

	readDataset(1);
	Utilities::getTimePassed();
	/*setStartTime();
	createNNData(1);
	getTimePassed();*/
	Utilities::setStartTime();
	testNN();
	Utilities::getTimePassed();
}

#endif