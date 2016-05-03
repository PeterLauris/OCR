#pragma once

#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>

#include "neural_network.h"
#include "utilities.h"
#include "srilm.h"


class LanguageModel {
public:
	static Ngram *ngramLM;
	static LM *useLM;

	LanguageModel() {
		ngramLM = NULL;
		useLM = NULL;
	}

	static void prepareLM();
	static double getCredibility(std::vector<SymbolResult*>, int*);
	static double getCredibilityLM(std::string);
	static std::string determineWord(std::vector<SymbolResult*> wordResults);
};