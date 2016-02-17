#pragma once

#include "utilities.h"
#include <iostream>

void Utilities::setStartTime() {
	startTime = clock();
}

void Utilities::getTimePassed() {
	std::cout << "Time passed: " << (double(clock() - startTime) / CLOCKS_PER_SEC) << std::endl;
}

float Utilities::randomFloat(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
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