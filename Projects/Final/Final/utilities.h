#pragma once

#include <ctime>

class Utilities {
public:
	static clock_t startTime;
	static void setStartTime();
	static void getTimePassed();
	static float randomFloat(float, float);
};