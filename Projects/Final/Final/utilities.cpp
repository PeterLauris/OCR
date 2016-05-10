#pragma once

#include <iostream>
#include <random>

#include "utilities.h"
#include "neural_network.h"

using namespace std;
using namespace cv;

/// Uzstāda sākuma laiku tā mēŗīšanai
void Utilities::setStartTime() {
	startTime = clock();
}

///Iegūst pagājušo laiku kopš tā uzstādīšanas 
void Utilities::getTimePassed() {
	std::cout << "Time passed: " << (double(clock() - startTime) / CLOCKS_PER_SEC) << std::endl;
}

///Iegūst nejaušu veselu skaitli intervālā
int Utilities::randomInt(int a, int b) {
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(a, b);
	return uni(rng);
}

///Iegūst nejaušu daļskaitli intervālā
float Utilities::randomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

bool Utilities::isImageEmpty(Mat img) {
	int blackPixels = 0;
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			int pix = (int)img.at<uchar>(y, x);
			//cout << pix << " ";
			blackPixels += (pix <= 128 ? 1 : 0); //otrādi, nekā, pārveidojot uz string
		}
		//cout << "\n";
	}
	return blackPixels < MIN_BLACK_PIXELS;
}

///Pārveido attēlu par 1 un 0 virkni
string Utilities::convertImageToString(Mat img, bool useSpaces) {
	string trainingDataContent = "";
	int inputSize = img.rows * img.cols;
	int *arr = new int[inputSize];
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			Vec3b intensity = img.at<Vec3b>(y, x);
			int t = (intensity[0] <= 128 ? 0 : 1);
			arr[y*img.cols + x] = t;
			trainingDataContent = trainingDataContent + to_string(t);
			if (useSpaces) trainingDataContent += " ";
			//cout << t << " ";
		}
		//cout << "\n";
	}
	delete[] arr;
	//getchar();
	return trainingDataContent;
}

string Utilities::convertImageToString_binary(Mat img, bool useSpaces) {
	string trainingDataContent = "";
	int inputSize = img.rows * img.cols;
	int *arr = new int[inputSize];
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			int pix = (int)img.at<uchar>(y, x);
			int t = (pix <= 128 ? 0 : 1);
			arr[y*img.cols + x] = t;
			trainingDataContent = trainingDataContent + to_string(t);
			if (useSpaces) trainingDataContent += " ";
			//cout << t << " ";
		}
		//cout << "\n";
	}
	delete[] arr;
	//cout << trainingDataContent << endl;
	//getchar();
	return trainingDataContent;
}

string Utilities::wordToString(std::vector<SymbolResult*> word, int *choiceIdxs) {
	string res = "";
	for (int i = 0; i < word.size(); i++) {
		res += NeuralNetwork::SYMBOLS[word[i]->symbolIdxs[choiceIdxs[i]]];
	}
	return res;
}

std::vector<cv::Rect> Utilities::reorderWordBoxes(std::vector<cv::Rect> wordBoxes) {
	static std::vector<cv::Rect> resultWb;

	//iegūst vidējo vārda augstumu
	int averageHeight = 0;
	for (int i = 0; i < wordBoxes.size(); i++) {
		averageHeight += wordBoxes[i].height;
	}
	averageHeight /= wordBoxes.size();

	while (!wordBoxes.empty()) {
		//atrod augstāko wb
		int highestWbY = wordBoxes[0].y;
		int highestIdx = 0;
		for (int i = 1; i < wordBoxes.size(); i++) {
			if (highestWbY > wordBoxes[i].y) {
				highestWbY = wordBoxes[i].y;
			}
		}

		Rect leftmostWb;
		int leftMostIdx;
		do {
			leftMostIdx = -1;

			for (int i = 0; i < wordBoxes.size(); i++) {
				if (wordBoxes[i].y > highestWbY - averageHeight &&
					wordBoxes[i].y < highestWbY + averageHeight) {
					if (leftMostIdx == -1 ||
						leftmostWb.x > wordBoxes[i].x) {
						leftmostWb = wordBoxes[i];
						leftMostIdx = i;
					}
				}
			}

			if (leftMostIdx > -1) {
				resultWb.push_back(wordBoxes[leftMostIdx]);
				wordBoxes.erase(wordBoxes.begin() + leftMostIdx);
			}

		} while (leftMostIdx > -1);
	}

	return resultWb;
}