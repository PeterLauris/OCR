#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

#include "utilities.h"
#include "srilm.h"


class LanguageModel {
public:
	static double getProbability();
};