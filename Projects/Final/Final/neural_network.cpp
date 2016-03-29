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

//create 1bpp images
void NeuralNetwork::prepareTrainingData() {
	SARRAY *safiles = getSortedPathnamesInDirectory(dirLettersTrainingSet.c_str(), NULL, 0, 0);
	l_int32 nfiles = sarrayGetCount(safiles);

	PIX *pixs, *pixt;
	char targetPath[1024];
	char newName[512];
	string imgPath = "";

	for (l_int32 i = 0; i < nfiles; i++) {
		imgPath = sarrayGetString(safiles, i, 0);
		if ((pixs = pixRead(imgPath.c_str())) == NULL) {
			printf("image file %s not read\n", imgPath);
			continue;
		}

		pixt = pixConvertTo1(pixs, 150);
		sprintf_s(newName, "%s.%05d.png", "s", i);
		sprintf_s(targetPath, "%s%s%s", dirLettersTrainingSet, "1bpp/", newName);
		pixWrite(targetPath, pixt, IFF_PNG);

		pixDestroy(&pixs);
		pixDestroy(&pixt);
	}
	sarrayDestroy(&safiles);


	safiles = getSortedPathnamesInDirectory(dirSpacingTrainingSet.c_str(), NULL, 0, 0);
	nfiles = sarrayGetCount(safiles);

	for (l_int32 i = 0; i < nfiles; i++) {
		imgPath = sarrayGetString(safiles, i, 0);
		if ((pixs = pixRead(imgPath.c_str())) == NULL) {
			printf("image file %s not read\n", imgPath);
			continue;
		}

		//TODO error when	 converting to 1bpp
		pixt = pixConvertTo1(pixs, 150);
		sprintf_s(newName, "%s.%05d.png", "s", i);
		sprintf_s(targetPath, "%s%s%s", dirSpacingTrainingSet.c_str(), "1bpp/", newName);
		pixWrite(targetPath, pixs, IFF_PNG);

		pixDestroy(&pixs);
		//pixDestroy(&pixt);
	}
	sarrayDestroy(&safiles);


	cout << "Created 1bpp images of spacing images\n";
}

//create 1bb imagess
void NeuralNetwork::prepareTestData() {
	SARRAY *safiles = getSortedPathnamesInDirectory(dirSpacingTestSet.c_str(), NULL, 0, 0);
	l_int32 nfiles = sarrayGetCount(safiles);

	PIX *pixs, *pixt;
	char targetPath[1024];
	char newName[512];
	string imgPath = "";

	for (l_int32 i = 0; i < nfiles; i++) {
		imgPath = sarrayGetString(safiles, i, 0);
		if ((pixs = pixRead(imgPath.c_str())) == NULL) {
			printf("image file %s not read\n", imgPath);
			continue;
		}

		pixt = pixConvertTo1(pixs, 150);
		sprintf_s(newName, "%s.%05d.png", "s", i);
		sprintf_s(targetPath, "%s%s", dirSpacingTestSet, newName);
		pixWrite(targetPath, pixt, IFF_PNG);

		pixDestroy(&pixs);
		pixDestroy(&pixt);
	}
	sarrayDestroy(&safiles);

	cout << "Created 1bpp images of spacing test images\n";
}

//Pievieno spritesheet bild?m atbilstošos burtus
void NeuralNetwork::completeSpritesheet_letters(int type) {
	cout << "Complete spritesheet" << endl;
	string newString = "";
	string word;

	bool firstLine = true;

	ifstream in;

	if (type == 0) in.open("../../../images/learning/letters/training-set-spritesheet-original.txt");
	else if (type == 1) in.open("../../../images/learning/letters/test-set-spritesheet-original.txt");
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
	if (type == 0) out.open("../../../images/learning/letters/training-set-spritesheet.txt", std::ios::out | std::ios::binary);
	else if (type == 1) out.open("../../../images/learning/letters/test-set-spritesheet.txt", std::ios::out | std::ios::binary);

	out << newString;
	out.close();
}

//Nolasa spritesheet
void NeuralNetwork::readDataset_letters(int type = 0) {
	ifstream in;
	if (type == 0) {
		in.open("../../../images/learning/letters/training-set-spritesheet.txt");
		cout << "Create training dataset" << endl;
	}
	else if (type == 1) {
		in.open("../../../images/learning/letters/test-set-spritesheet.txt");
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
		cout << lineCount << endl;

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

		if (type == 0) NeuralNetwork::trainingRecords_letters = typeRecords;
		else if (type == 1) NeuralNetwork::testRecords_letters = typeRecords;

		/*for (int i = 0; i < lineCount; i++) {
		cout << "(" << trainingRecords[i].x << ";" << trainingRecords[i].y << ") --> " << trainingRecords[i].letter << endl;
		}*/
	}
	else {
		printf("Failed to open the data file\n");
	}
	in.close();
}

//nolasa katrai spritesheet bildei atbilstošo simbolu no txt faila
void NeuralNetwork::readDataset_spacing(int type = 0) {
	ifstream in;
	if (type == 0) {
		in.open("../../../images/learning/spacing/narrow/training-set-spritesheet.txt");
		cout << "Read training dataset" << endl;
	}
	else if (type == 1) {
		in.open("../../../images/learning/spacing/narrow/test-set-spritesheet.txt");
		cout << "Read test dataset" << endl;
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
		bool readingLetter = true;
		int coordIdx = 0;
		int num = -1;
		string word;

		while (in >> word) {
			//cout << word << endl;

			if (word == "=") {
				readingCoords = true;
				readingLetter = false;
			}
			else {
				if (readingCoords) {
					if (coordIdx == 0)
						typeRecords[num].x = stoi(word);
					else if (coordIdx == 1)
						typeRecords[num].y = stoi(word);
					//cout << "Coord [" << coordIdx << "]: " << stoi(word) << "\n";
					coordIdx++;

					if (coordIdx >= 4) {
						readingCoords = false;
						readingLetter = true;
						continue;
					}
				}
				else if (readingLetter) {
					num++;
					typeRecords[num].letter = word[word.find('_') + 1]; //0 vai 1
					readingLetter = false;
					coordIdx = 0;
				}
			}
		}

		if (type == 0) NeuralNetwork::trainingRecords_spacing = typeRecords;
		else if (type == 1) NeuralNetwork::testRecords_spacing = typeRecords;
	}
	else {
		printf("Failed to open the data file\n");
	}
	in.close();
}

void NeuralNetwork::createNNData_letters(int type = 0) {
	string imgPath = "";

	ofstream out;
	if (type == 0) {
		out.open("train_letters.in");
		imgPath = NeuralNetwork::dirLettersTrainingSpritemap.c_str();
		cout << "Creating letters training data..." << endl;
	}
	else if (type == 1) {
		out.open("test_letters.in");
		imgPath = NeuralNetwork::dirLettersTestSpritemap.c_str();
		cout << "Creating letters test data..." << endl;
	}
	else {
		cerr << "ERROR: Invalid type\n";
		exit(1);
	}

	if (out.is_open()) {
		int validFileCount = 0;
		int inputSize = INPUT_SIZE_LETTERS;
		string dataContent = "";

		cout << imgPath << "\n";

		Mat img = imread(imgPath);
		if (img.empty()) {
			cout << "Cannot load " << imgPath << endl;
			return;
		}
		//cvtColor(img, img, CV_BGR2GRAY);

		RecordInfo* typeRecords;
		if (type == 0) typeRecords = NeuralNetwork::trainingRecords_letters;
		else if (type == 1) typeRecords = NeuralNetwork::testRecords_letters;
		else {
			exit(1);
		}

		int repCount = (type == 0) ? TRANSFORMATION_COUNT_LETTER : 1;

		Point2f srcTri[3];
		Point2f dstTri[3];
		Mat rot_mat(2, 3, CV_32FC1);
		Mat warp_mat(2, 3, CV_32FC1);
		Mat warp_dst, warp_rotate_dst;

		std::vector<std::vector<string> > sortingVector(SORTING_VECTOR_COUNT, std::vector<string>());

		cout << "Records count: " << recordsCount << endl;

		for (int i = 0; i < recordsCount; i++) {
			//cout << i << ": " << typeRecords[i].y << " " << typeRecords[i].y + LETTER_HEIGHT << " | " << typeRecords[i].x << " " << typeRecords[i].x + LETTER_WIDTH << endl;
			Mat subImg = img(cv::Range(typeRecords[i].y, typeRecords[i].y + LETTER_HEIGHT), cv::Range(typeRecords[i].x, typeRecords[i].x + LETTER_WIDTH));

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

					dstTri[0] = Point2f(subInvImg.cols*Utilities::randomFloat(0, DEFORMATION_AMOUNT_LETTER), subInvImg.rows*Utilities::randomFloat(0, DEFORMATION_AMOUNT_LETTER));
					dstTri[1] = Point2f(subInvImg.cols*Utilities::randomFloat(1 - DEFORMATION_AMOUNT_LETTER, 1), subInvImg.rows*Utilities::randomFloat(0, DEFORMATION_AMOUNT_LETTER));
					dstTri[2] = Point2f(subInvImg.cols*Utilities::randomFloat(0, DEFORMATION_AMOUNT_LETTER), subInvImg.rows*Utilities::randomFloat(1 - DEFORMATION_AMOUNT_LETTER, 1));
					
					warp_mat = getAffineTransform(srcTri, dstTri);

					//warp_dst.setTo(255);
					warpAffine(subInvImg, warp_dst, warp_mat, warp_dst.size());

					bitwise_not(warp_dst, warp_dst);

					//TODO vai vajag izt?r?t atmi?u??
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

					// Apply the erosion operation
					//erode(warp_dst, erosion_dst, element);

					resString = Utilities::convertImageToString(erosion_dst, true);
				}
				else {
					resString = Utilities::convertImageToString(subImg, true);
				}

				resString += "\n";
				for (int j = 0; j < SYMBOL_COUNT; j++) {
					if (typeRecords[i].letter == SYMBOLS[j]) {
						resString += "1 ";
					}
					else {
						resString += "-1 ";
					}
				}
				resString += "\n";

				sortingVector[Utilities::randomInt(0, SORTING_VECTOR_COUNT - 1)].push_back(resString);
			}

			/*namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
			imshow("MyWindow", subImg);
			waitKey(0);
			destroyWindow("MyWindow");*/

			subImg.release();
		}

		img.release();

		for (int i = 0; i < SORTING_VECTOR_COUNT; i++) {
			while (!sortingVector[i].empty()) {
				dataContent += sortingVector[i].back();
				sortingVector[i].pop_back();
			}
		}

		out << (recordsCount*repCount) << " " << inputSize << " " << SYMBOL_COUNT << "\n" << dataContent;
		out.close();
	}
}

void NeuralNetwork::createNNData_spacing(int type = 0) {
	string imgPath = "";

	ofstream out;
	if (type == 0) {
		out.open("train_spacing.in");
		imgPath = NeuralNetwork::dirSpacingTrainingSpritemap.c_str();
		cout << "Creating spacing training data..." << endl;
	}
	else if (type == 1) {
		out.open("test_spacing.in");
		imgPath = NeuralNetwork::dirSpacingTestSpritemap.c_str();
		cout << "Creating spacing test data..." << endl;
	}
	else {
		cerr << "ERROR: Invalid type\n";
		exit(1);
	}

	if (out.is_open()) {
		int validFileCount = 0;
		int inputSize = INPUT_SIZE_SPACING; // 10*32
		string dataContent = "";

		cout << imgPath << "\n";

		Mat img = imread(imgPath);
		if (img.empty()) {
			cout << "Cannot load " << imgPath << endl;
			return;
		}
		//cvtColor(img, img, CV_BGR2GRAY);

		RecordInfo* typeRecords;
		if (type == 0) typeRecords = NeuralNetwork::trainingRecords_spacing;
		else if (type == 1) typeRecords = NeuralNetwork::testRecords_spacing;
		else {
			cout << "Wrong type!" << endl;
			exit(1);
		}

		int repCount = (type == 0) ? TRANSFORMATION_COUNT_SPACING : 1;

		Point2f srcTri[3];
		Point2f dstTri[3];
		Mat rot_mat(2, 3, CV_32FC1);
		Mat warp_mat(2, 3, CV_32FC1);
		Mat warp_dst, warp_rotate_dst;

		std::vector<std::vector<string> > sortingVector(SORTING_VECTOR_COUNT, std::vector<string>());

		for (int i = 0; i < recordsCount; i++) {
			//cout << typeRecords[i].y << " " << typeRecords[i].x << endl;
			Mat subImg = img(cv::Range(typeRecords[i].y, typeRecords[i].y + SPACING_HEIGHT), cv::Range(typeRecords[i].x, typeRecords[i].x + SPACING_WIDTH));
			Mat subInvImg;
			bitwise_not(subImg, subInvImg);
			srcTri[0] = Point2f(0, 0);
			srcTri[1] = Point2f(subImg.cols - 1, 0);
			srcTri[2] = Point2f(0, subImg.rows - 1);

			//for (int rep = 0; rep < repCount; rep++) {
			//	string resString = Utilities::convertImageToString(subImg, true);

			//	dataContent += resString + "\n";
			//	dataContent += typeRecords[i].letter; //1, ja atstarpe, 0, ja nav
			//	dataContent += "\n";
			//}

			for (int rep = 0; rep < repCount; rep++) {
				string resString = "";

				if (rep > 0) { //pirmaj? iter?cij? ori?in?lu nemaina
							   //randomly modify subImg
					warp_dst = Mat::zeros(subInvImg.rows, subInvImg.cols, subInvImg.type());

					dstTri[0] = Point2f(subInvImg.cols*Utilities::randomFloat(0, DEFORMATION_AMOUNT_SPACING), subInvImg.rows*Utilities::randomFloat(0, DEFORMATION_AMOUNT_SPACING));
					dstTri[1] = Point2f(subInvImg.cols*Utilities::randomFloat(1 - DEFORMATION_AMOUNT_SPACING, 1), subInvImg.rows*Utilities::randomFloat(0, DEFORMATION_AMOUNT_SPACING));
					dstTri[2] = Point2f(subInvImg.cols*Utilities::randomFloat(0, DEFORMATION_AMOUNT_SPACING), subInvImg.rows*Utilities::randomFloat(1 - DEFORMATION_AMOUNT_SPACING, 1));

					warp_mat = getAffineTransform(srcTri, dstTri);

					//warp_dst.setTo(255);
					warpAffine(subInvImg, warp_dst, warp_mat, warp_dst.size());

					bitwise_not(warp_dst, warp_dst);

					//TODO vai vajag izt?r?t atmi?u??
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

					/*namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
					imshow("MyWindow", erosion_dst);
					waitKey(0);
					destroyWindow("MyWindow");*/

					resString = Utilities::convertImageToString(erosion_dst, true);
				}
				else {
					resString = Utilities::convertImageToString(subImg, true);
				}

				if (typeRecords[i].letter == "1") {
					resString += "\n1\n";
				}
				else {
					resString += "\n-1\n";
				}

				sortingVector[Utilities::randomInt(0, SORTING_VECTOR_COUNT-1)].push_back(resString);
			}

			subImg.release();
		}
		img.release();

		for (int i = 0; i < SORTING_VECTOR_COUNT; i++) {
			while (!sortingVector[i].empty()) {
				dataContent += sortingVector[i].back();
				sortingVector[i].pop_back();
			}
		}

		out << (recordsCount*repCount) << " " << inputSize << " " << 1 << "\n" << dataContent;
		out.close();
	}
	cout << "Finished" << endl;
}

void NeuralNetwork::trainNN_spacing() {
	const unsigned int num_input = INPUT_SIZE_SPACING;
	const unsigned int num_output = 1;
	const unsigned int num_layers = 4;
	const float desired_error = (const float) 0.00001;
	const unsigned int max_epochs = 500000;
	const unsigned int epochs_between_reports = 100;


	unsigned int layers[num_layers] = { num_input, 3, 3, num_output };
	struct fann *ann = fann_create_standard_array(num_layers, layers); //fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
	fann_randomize_weights(ann, -0.3, 0.3);

	//FANN_ELLIOT_SYMMETRIC
	//FANN_SIGMOID_SYMMETRIC
	fann_set_activation_function_hidden(ann, FANN_ELLIOT_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_ELLIOT_SYMMETRIC);

	fann_train_on_file(ann, "train_spacing.in", max_epochs, epochs_between_reports, desired_error);

	fann_save(ann, "result_spacing.net");

	fann_destroy(ann);
}

void NeuralNetwork::trainNN_letters() {
	const unsigned int num_input = INPUT_SIZE_LETTERS;
	const unsigned int num_layers = 4;
	const unsigned int max_epochs = 500000;
	const unsigned int epochs_between_reports = 20;
	const float desired_error = 0.00001;

	cout << "Training..." << endl;

	unsigned int layers[num_layers] = { num_input, 9, 8, SYMBOL_COUNT };
	struct fann *ann = fann_create_standard_array(num_layers, layers); //fann_create_standard(num_layers, num_input, num_neurons_hidden, num_output);
	fann_randomize_weights(ann, -0.3, 0.3);

	fann_set_activation_function_hidden(ann, FANN_ELLIOT_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_ELLIOT_SYMMETRIC);

	fann_train_on_file(ann, "train_letters.in", max_epochs, epochs_between_reports, desired_error);

	fann_save(ann, "result_letters.net");

	fann_destroy(ann);

	cout << "\a";
	//getchar();
}

void NeuralNetwork::testNN_letters() {
	cout << "Testing letters..." << endl;
	struct fann *ann = fann_create_from_file("result_letters.net");
	if (ann == NULL) {
		cout << "ann IS NULL!!??" << endl;
		return;
	}

	//struct fann_train_data *data = fann_read_train_from_file("test_letters.in");
	//fann_reset_MSE(ann);
	//fann_test_data(ann, data);
	//printf("Mean Square Error: %f\n", fann_get_MSE(ann));
	//fann_destroy_train(data);

	fann_type input[INPUT_SIZE_LETTERS];
	fann_type calc_out[SYMBOL_COUNT];
	fann_type expected_out[SYMBOL_COUNT];

	ifstream in("test_letters.in");
	string word;
	in >> word;
	int sampleCount = stoi(word);
	in >> word; //1024
	int inputCount = stoi(word);
	in >> word;
	int outputCount = stoi(word);

	int correctCount = 0;
	for (int i = 0; i < sampleCount; i++) {
		for (int j = 0; j < inputCount; j++) {
			in >> word;
			input[j] = stoi(word);
		}
		for (int j = 0; j < outputCount; j++) {
			in >> word;
			expected_out[j] = stoi(word);
		}

		int correctIdx = -1;
		int currentLargestIdx = -1;
		fann_type currentLargestValue = -1;
		fann_type* tmp = fann_run(ann, input);
		for (int k = 0; k < SYMBOL_COUNT; k++) {
			calc_out[k] = tmp[k];
			if (calc_out[k] > currentLargestValue) {
				currentLargestValue = calc_out[k];
				currentLargestIdx = k;
			}
			if (expected_out[k] == 1) {
				correctIdx = k;
			}
		}

		if (currentLargestIdx == correctIdx)
			correctCount++;
		//bool result = fann_test(ann, input, expected_out);
		//cout << "Input " << i << ":\n"; //  << " (" << (result ? "TRUE" : "FALSE")
		/*for (int j = 0; j < outputCount; j++) {
		cout << SYMBOLS[j] << ": " << calc_out[j] << " \t" << expected_out[j] << "\n";
		}
		cout << "\n---------\n";*/
		//cin >> word;
	}
	cout << correctCount << "/" << sampleCount << " (" << correctCount / sampleCount << ")\n";

	in.close();
}

void NeuralNetwork::testNN_spacing() {
	cout << "Testing spacing..." << endl;
	struct fann *ann = fann_create_from_file("result_spacing.net");
	if (ann == NULL) {
		cout << "ann IS NULL!!??" << endl;
		return;
	}

	fann_type input[INPUT_SIZE_SPACING];
	fann_type calc_out[1]; //TODO 1?
	fann_type expected_out[1];

	ifstream in("test_spacing.in");
	string word;
	in >> word;
	int sampleCount = stoi(word);
	in >> word;
	int inputCount = stoi(word);
	in >> word;
	int outputCount = stoi(word);

	int correctCount = 0;
	for (int i = 0; i < sampleCount; i++) {
		for (int j = 0; j < inputCount; j++) {
			in >> word;
			input[j] = stoi(word);
		}
		for (int j = 0; j < outputCount; j++) {
			in >> word;
			expected_out[j] = stoi(word);
		}

		int correctIdx = -1;
		int currentLargestIdx = -1;
		fann_type currentLargestValue = -1;
		fann_type* tmp = fann_run(ann, input);
		for (int k = 0; k < 1; k++) {
			calc_out[k] = tmp[k];
			if (calc_out[k] > currentLargestValue) {
				currentLargestValue = calc_out[k];
				currentLargestIdx = k;
			}
			//if (expected_out[k] == 1) {
			//	correctIdx = k;
			//}

			cout << "Expected: " << expected_out[k] << " --- " << calc_out[k] << endl;
		}

		//if (currentLargestIdx == correctIdx)
		//	correctCount++;
	}
	//cout << correctCount << "/" << sampleCount << " (" << correctCount / sampleCount << ")\n";

	in.close();
}

void NeuralNetwork::testNN_image_spacing(Mat img, int &calcIdx, double &calcProb) {
	struct fann *ann = fann_create_from_file("result_spacing.net");
	if (ann == NULL) {
		cout << "ann IS NULL!!??" << endl;
		return;
	}

	fann_type input[INPUT_SIZE_SPACING];
	fann_type calc_out[1];
	fann_type expected_out[1];

	Size size(SPACING_WIDTH, SPACING_HEIGHT);
	Mat resizedImg;
	resize(img, resizedImg, size);

	int sampleCount = 1;
	int inputCount = INPUT_SIZE_SPACING;
	int outputCount = 1;

	string imgString = Utilities::convertImageToString(resizedImg, false);
	//cout << imgString << endl;

	int correctCount = 0;
	for (int j = 0; j < INPUT_SIZE_SPACING; j++) {
		input[j] = imgString[j] - '0';
	}
	for (int j = 0; j < outputCount; j++) {
		expected_out[j] = 0; //UNKNOWN
	}

	int currentLargestIdx = -1;
	fann_type currentLargestValue = -1;
	fann_type* tmp = fann_run(ann, input);
	for (int k = 0; k < 1; k++) {
		calc_out[k] = tmp[k];
		if (calc_out[k] > currentLargestValue) {
			currentLargestValue = calc_out[k];
			currentLargestIdx = k;
		}
	}

	//cout << "Calculated result (is spacing): " << tmp[currentLargestIdx] << endl;

	calcIdx = (tmp[currentLargestIdx] >= 0 ? 1 : 0);
	calcProb = tmp[currentLargestIdx];
}

void NeuralNetwork::testNN_image_letter(Mat img, int &calcIdx, double &calcProb) {
	cout << "Testing image spacing..." << endl;
	struct fann *ann = fann_create_from_file("result_letters.net");
	if (ann == NULL) {
		cout << "ann IS NULL!!??" << endl;
		return;
	}

	fann_type input[INPUT_SIZE_LETTERS];
	fann_type calc_out[SYMBOL_COUNT];
	fann_type expected_out[SYMBOL_COUNT];

	Size size(LETTER_WIDTH, LETTER_HEIGHT);
	Mat resizedImg;
	resize(img, resizedImg, size);

	int sampleCount = 1;
	int inputCount = INPUT_SIZE_LETTERS;
	int outputCount = SYMBOL_COUNT;

	string imgString = Utilities::convertImageToString(resizedImg, false);
	//cout << imgString << endl << imgString.length() << endl;

	int correctCount = 0;
	for (int j = 0; j < inputCount; j++) {
		input[j] = imgString[j] - '0';
	}
	for (int j = 0; j < outputCount; j++) {
		expected_out[j] = 0; //UNKNOWN
	}

	int currentLargestIdx = -1;
	fann_type currentLargestValue = -1;
	fann_type* tmp = fann_run(ann, input);
	for (int k = 0; k < outputCount; k++) {
		calc_out[k] = tmp[k];
		if (calc_out[k] > currentLargestValue) {
			currentLargestValue = calc_out[k];
			currentLargestIdx = k;
		}
	}

	cout << "Calculated result: " << SYMBOLS[currentLargestIdx] << " (" << currentLargestValue << ")" << endl;

	calcIdx = currentLargestIdx;
	calcProb = tmp[currentLargestIdx];
}

// Izsauc nepieciešam?s funkcijas OCR tren?šanai
void NeuralNetwork::trainOCR_letters() {
	completeSpritesheet_letters();
	readDataset_letters();
	createNNData_letters();
	trainNN_letters();

	/*completeSpritesheet_letters(1);
	readDataset_letters(1);
	createNNData_letters(1);
	testNN_letters();*/
}

void NeuralNetwork::trainOCR_letters_quicker() {
	Utilities::setStartTime();
	readDataset_letters();
	Utilities::getTimePassed();
	/*setStartTime();
	createNNData_letters();
	getTimePassed();*/
	Utilities::setStartTime();
	trainNN_letters();
	Utilities::getTimePassed();
	Utilities::setStartTime();

	readDataset_letters(1);
	Utilities::getTimePassed();
	/*setStartTime();
	createNNData_letters(1);
	getTimePassed();*/
	Utilities::setStartTime();
	testNN_letters();
	Utilities::getTimePassed();
}

void NeuralNetwork::trainOCR_spacing() {
	//completeSpritesheet_spacing();
	readDataset_spacing();
	createNNData_spacing();
	trainNN_spacing();

	//completeSpritesheet_letters(1);
	//readDataset_spacing(1);
	//createNNData_spacing(1);
	//testNN_spacing();
}

#endif