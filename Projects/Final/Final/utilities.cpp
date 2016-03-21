#pragma once

#include <iostream>
#include <random>
#include "utilities.h"

using namespace std;
using namespace cv;

void Utilities::setStartTime() {
	startTime = clock();
}

void Utilities::getTimePassed() {
	std::cout << "Time passed: " << (double(clock() - startTime) / CLOCKS_PER_SEC) << std::endl;
}

int Utilities::randomInt(int a, int b) {
	std::random_device rd;
	std::mt19937 rng(rd());
	std::uniform_int_distribution<int> uni(a, b);
	return uni(rng);
}

float Utilities::randomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

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

//#ifndef _UTILITIES_
//#define _UTILITIES_
//
//#include <iostream>
//#include <ctime>
//
//using namespace std;
//
//
//class Utilities {
//public:
//	static clock_t startTime;
//
//	static void setStartTime() {
//		startTime = clock();
//	}
//
//	static void getTimePassed() {
//		cout << "Time passed: " << (double(clock() - startTime) / CLOCKS_PER_SEC) << endl;
//	}
//};
//
//#endif