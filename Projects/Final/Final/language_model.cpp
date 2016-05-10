#ifndef _LM_
#define _LM_

#include <iostream>
#include <fstream>
#include <algorithm>
#include <ctime>
#include <sstream>
#include "language_model.h"
#include "utilities.h"

using namespace std;

void LanguageModel::prepareLM() {
	cout << "Please wait, preparing language model...\n";
	static char *lmFile = "teksti/result/lm.txt";

	static unsigned order = defaultNgramOrder;
	LM::initialDebugLevel = 2;

	Vocab *vocab = new Vocab;
	vocab->unkIsWord() = false;
	vocab->toLower() = false;

	File file(lmFile, "r");
	ngramLM = new Ngram(*vocab, order);
	ngramLM->read(file);
	useLM = ngramLM;
	//delete vocab;
	cout << "LM created\n";
}

double LanguageModel::getCredibility(std::vector<SymbolResult*> word, int* choiceIdxs) {
	double res = 1;
	for (int i = 0; i < word.size(); i++) {
		//res *= word[i]->prob[choiceIdxs[i]]; //[0, 1]
		res *= (word[i]->prob[choiceIdxs[i]] + 1) / 2;// [-1, 1];
	}
	return res;
}

double LanguageModel::getCredibilityLM(std::string word) {
	if (useLM == NULL) {
		//cout << "LM not prepared!\n";
		return 1;
		prepareLM();
	}

	static char *pplFile = "teksti/current.txt";

	ofstream out_curr(pplFile);
	out_curr << word;
	out_curr.close();

	ofstream out;
	out.open("teksti/result/stats.txt");

	File file(pplFile, "r");
	TextStats stats;
	useLM->dout(out);
	useLM->pplFile(file, stats, 0);

	out.close();

	ifstream i("teksti/result/stats.txt");
	std::stringstream buffer;
	buffer << i.rdbuf();
	string str = buffer.str();

	int pos1 = str.find("logprob=") + 10;
	int pos2 = str.find("ppl=") - 1;
	string p1 = str.substr(pos1, pos2 - pos1 - 1);

	istringstream op1(p1);

	double dp1;
	op1 >> dp1;

	//std::cout << dp1 << " +\n" << dp2 << " =\n" << (dp1 + dp2) << endl;

	i.close();

	return dp1;
}

std::string LanguageModel::determineWord(std::vector<SymbolResult*> wordResults) {
	std::string bestResult = "";
	double bestResultCredibility = 0;

	int *choiceIdxs = new int[wordResults.size()]{ 0 };
	bestResult = Utilities::wordToString(wordResults, choiceIdxs);
	bestResultCredibility = getCredibility(wordResults, choiceIdxs) * getCredibilityLM(Utilities::wordToString(wordResults, choiceIdxs));
	std::cout << "---\nCurrent best result: " << bestResult << " (" << bestResultCredibility << ")\n";

	//std::string tmpResult;
	double tmpResultCredibility;

	for (int i = 0; i < wordResults.size(); i++) {
		for (int j = 1; j < 3; j++) {
			if (wordResults[i]->symbolIdxs[j] == -1) break;

			//tmpResult = bestResult;
			//tmpResult = tmpResult.substr(0,i) + NeuralNetwork::SYMBOLS[wordResults[i]->symbolIdxs[j]] + tmpResult.substr(i+1, strlen(tmpResult.c_str())-i-1);
			int prevChoiceIdx = choiceIdxs[i];
			choiceIdxs[i] = j;
			tmpResultCredibility = getCredibility(wordResults, choiceIdxs) * getCredibilityLM(Utilities::wordToString(wordResults, choiceIdxs));
			cout << Utilities::wordToString(wordResults, choiceIdxs) << " (" << tmpResultCredibility << ")\n";

			if (tmpResultCredibility > bestResultCredibility) {
				bestResult = Utilities::wordToString(wordResults, choiceIdxs);
				bestResultCredibility = tmpResultCredibility;
				std::cout << "New best result: " << bestResult << " (" << bestResultCredibility << ")\n";
			}
			else {
				choiceIdxs[i] = prevChoiceIdx; //ja nav labāks, atliek atpakaļ
			}
		}
	}

	delete[] choiceIdxs;

	return bestResult;
}

#endif