#include <iostream>

#define strdup _strdup

#include "option.h"
#include "version.h"
#include "File.h"
#include "Vocab.h"
#include "SubVocab.h"
#include "MultiwordVocab.h"
#include "MultiwordLM.h"
#include "NonzeroLM.h"
#include "NBest.h"
#include "TaggedVocab.h"
#include "Ngram.h"
#include "TaggedNgram.h"
#include "StopNgram.h"
#include "ClassNgram.h"
#include "SimpleClassNgram.h"
#include "DFNgram.h"
#include "SkipNgram.h"
#include "HiddenNgram.h"
#include "HiddenSNgram.h"
#include "NullLM.h"
#include "LMClient.h"
#include "BayesMix.h"
#include "LoglinearMix.h"
#include "AdaptiveMix.h"
#include "AdaptiveMarginals.h"
#include "NgramCountLM.h"
#include "MSWebNgramLM.h"
#include "CacheLM.h"
#include "DynamicLM.h"
#include "DecipherNgram.h"
#include "HMMofNgrams.h"
#include "RefList.h"
#include "ProductNgram.h"
#include "Array.cc"
#include "MEModel.h"
#include "MStringTokUtil.h"
#include "BlockMalloc.h"

//#include "fann.h"

int main() {
	cout << "success" << endl;
	getchar();

	return 0;
}