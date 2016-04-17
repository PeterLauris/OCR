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

double LanguageModel::getProbability() {
	Vocab *vocab;

	cout << "get prob\n" << endl;
	return 0;
	getchar();
	ifstream i("stats.txt");
	std::stringstream buffer;
	buffer << i.rdbuf();
	string str = buffer.str();
	cout << str << endl;
	int pos1 = str.find("]") + 2;
	int pos2 = str.find("[", pos1);
	int pos3 = str.find("]", str.find("]", pos2) + 1) + 2;
	int pos4 = str.find("[", pos3);
	string p1 = str.substr(pos1, pos2 - pos1 - 1);
	string p2 = str.substr(pos3, pos4 - pos3 - 1);

	istringstream op1(p1);
	istringstream op2(p2);

	double dp1, dp2;
	op1 >> dp1;
	op2 >> dp2;

	cout << dp1 << " +\n" << dp2 << " =\n" << (dp1 + dp2) << endl;

	return dp1 + dp2;
}

#endif