﻿#ifndef _IMAGE_PROC_
#define _IMAGE_PROC_

#include <stdlib.h>
#include <string>

#include "image_processing.h"
#include "neural_network.h"
#include "utilities.h"


using namespace std;
using namespace cv;

// Att?la spilgtuma koeficienta ieguve (0-256)
l_int32 ImageProcessing::getLightingBGval(PIX *pixs) {
	l_float32 lightPixFract, darkPixFract; //fraction of pixels in intermediate brightness range that were considered for color content
	l_float32 lightColorFract, darkColorFract; //fraction of pixels that meet the criterion for sufficient color
	pixColorFraction(pixs, DARK_THRESH_LIGHT, LIGHT_THRESH_LIGHT, DIFF_THRESH, FACTOR, &lightPixFract, &lightColorFract);
	pixColorFraction(pixs, DARK_THRESH_DARK, LIGHT_THRESH_DARK, DIFF_THRESH, FACTOR, &darkPixFract, &darkColorFract);

	l_float32 pixFractDiff = lightPixFract - darkPixFract;
	printf("BGVAL: %d\n", (l_int32)(BGVAL_MIDDLE + pixFractDiff * BGVAL_DELTA));
	return (l_int32)(BGVAL_MIDDLE + pixFractDiff * BGVAL_DELTA);
}

l_int32 ImageProcessing::light_pixDisplayWriteFormat(PIX *pixs, l_int32  reduction, l_int32  format)
{
	char            buffer[L_BUF_SIZE];
	l_int32         ignore;
	l_float32       scale;
	PIX            *pixt, *pix8;
	static l_int32  index = 0;  /* caution: not .so or thread safe */

	if (reduction == 0) return 0;

	if (reduction < 0) {
		index = 0;  /* reset; this will cause erasure at next call to write */
		return 0;
	}

	if (format != IFF_JFIF_JPEG && format != IFF_PNG)
		printf("ERROR: invalid format\n");
	if (!pixs)
		printf("ERROR: pixs not defined\n");

	if (index == 0) {
		snprintf(buffer, L_BUF_SIZE,
			"del /f tmp\\junk_write_display.*.png tmp\\junk_write_display.*.jpg");
		//printf("test\n");
		ignore = system(buffer);
	}
	index++;

	if (reduction == 1)
		pixt = pixClone(pixs);
	else {
		scale = 1. / (l_float32)reduction;
		if (pixGetDepth(pixs) == 1)
			pixt = pixScaleToGray(pixs, scale);
		else
			pixt = pixScale(pixs, scale, scale);
	}

	if (pixGetDepth(pixt) == 16) {
		pix8 = pixMaxDynamicRange(pixt, L_LOG_SCALE);
		snprintf(buffer, L_BUF_SIZE, "tmp\\junk_write_display.%03d.png", index);
		pixWrite(buffer, pix8, IFF_PNG);
		pixDestroy(&pix8);
	}
	else if (pixGetDepth(pixt) < 8 || pixGetColormap(pixt) ||
		format == IFF_PNG) {
		snprintf(buffer, L_BUF_SIZE, "tmp\\junk_write_display.%03d.png", index);
		pixWrite(buffer, pixt, IFF_PNG);
	}
	else {
		snprintf(buffer, L_BUF_SIZE, "tmp\\junk_write_display.%03d.jpg", index);
		if (pixt == NULL) printf("pixt is NULL");
		pixWrite(buffer, pixt, format);
	}
	pixDestroy(&pixt);

	return 0;
}

PIX * ImageProcessing::normalizeLighting(PIX *pixs, l_int32 *lightingVal) {
	l_int32      d;
	PIX         *pixc, *pixr, *pixg, *pixb, *pixsg, *pixsm, *pixd;

	*lightingVal = getLightingBGval(pixs);

	/* Normalize for uneven illumination on RGB image */
	pixBackgroundNormRGBArraysMorph(pixs, NULL, 8, 5, *lightingVal, &pixr, &pixg, &pixb);
	if (pixs == NULL) printf("pixs is NULL\n");
	pixd = pixApplyInvBackgroundRGBMap(pixs, pixr, pixg, pixb, 8, 8);
	if (pixd == NULL) printf("pixd is NULL\n");
	light_pixDisplayWriteFormat(pixd, 2, IFF_JFIF_JPEG);
	pixDestroy(&pixr);
	pixDestroy(&pixg);
	pixDestroy(&pixb);
	pixDestroy(&pixd);

	/* Convert the RGB image to grayscale. */
	pixsg = pixConvertRGBToLuminance(pixs);
	light_pixDisplayWriteFormat(pixsg, 2, IFF_JFIF_JPEG);

	/* Remove the text in the fg. */
	pixc = pixCloseGray(pixsg, 25, 25);
	light_pixDisplayWriteFormat(pixc, 2, IFF_JFIF_JPEG);

	/* Smooth the bg with a convolution. */
	pixsm = pixBlockconv(pixc, 15, 15);
	light_pixDisplayWriteFormat(pixsm, 2, IFF_JFIF_JPEG);
	pixDestroy(&pixc);

	/* Normalize for uneven illumination on gray image. */
	pixBackgroundNormGrayArrayMorph(pixsg, NULL, 8, 5, *lightingVal, &pixg);
	pixc = pixApplyInvBackgroundGrayMap(pixsg, pixg, 8, 8);
	light_pixDisplayWriteFormat(pixc, 2, IFF_JFIF_JPEG);
	pixDestroy(&pixg);

	pixDestroy(&pixsg);
	pixDestroy(&pixsm);

	return pixc;
}

PIX * ImageProcessing::removeNoise(PIX *pixs, l_int32 *lightingVal) {
	pixGammaTRC(pixs, pixs, GAMMA_IMG, MINVAL, MAXVAL);

	pixContrastTRC(pixs, pixs, CONTRAST_FACTOR);

	//pixEqualizeTRC(pixs, pixs, 0, 1);

	PIX *pix_new = pixUnsharpMaskingGray2D(pixs, 2, 0.1);

	//PIX *pix_old = pixs;
	//pixs = pixUnsharpMasking(pixs, UNSHARP_HALF_WIDTH, UNSHARP_FRACT);
	//pixDestroy(&pix_old);

	/*pix_old = pixs;
	pixs = pixBlockconv(pixs, 100, 100);*/
	//pixDestroy(&pix_old);

	return pix_new;
}

void ImageProcessing::PixAddEdgeData(PIXA *pixa, PIX *pixs, l_int32 side, l_int32 minjump, l_int32  minreversal)
{
	l_float32  jpl, jspl, rpl;
	PIX       *pixt1, *pixt2;

	pixMeasureEdgeSmoothness(pixs, side, minjump, minreversal, &jpl,
		&jspl, &rpl, "junkedge.png");
	fprintf(stderr, "side = %d: jpl = %6.3f, jspl = %6.3f, rpl = %6.3f\n",
		side, jpl, jspl, rpl);
	pixt1 = pixRead("junkedge.png");
	pixt2 = pixAddBorder(pixt1, 10, 0);  /* 10 pixel white border */
	pixaAddPix(pixa, pixt2, L_INSERT);
	pixDestroy(&pixt1);
	return;
}

//is not used / not working correctly?
//pixs needs to be 1bpp
PIX * ImageProcessing::removeImages(PIX *pixs) {
	char         buf[256];
	l_int32      index, zero;
	PIX         *pixr;   /* image reduced to 150 ppi */
	PIX         *pixhs;  /* image of halftone seed, 150 ppi */
	PIX         *pixm;   /* image of mask of components, 150 ppi */
	PIX         *pixhm1; /* image of halftone mask, 150 ppi */
	PIX         *pixht;  /* image of halftone components, 150 ppi */
	PIX         *pixnht; /* image without halftone components, 150 ppi */
	PIX         *pixt1, *pixt2;

	pixt1 = pixScaleToGray2(pixs);

	if (pixt1 == NULL) printf("pix is NULL\n");
	//pixWrite("tmp/orig.gray.150.png", pixt1, IFF_PNG);
	pixDestroy(&pixt1);
	pixr = pixReduceRankBinaryCascade(pixs, 1, 0, 0, 0);

	/* Get seed for halftone parts */
	pixt1 = pixReduceRankBinaryCascade(pixr, 4, 4, 3, 0);
	pixt2 = pixOpenBrick(NULL, pixt1, 5, 5);
	pixhs = pixExpandBinaryPower2(pixt2, 8);
	//pixWrite("tmp/htseed.150.png", pixhs, IFF_PNG);
	pixDestroy(&pixt1);
	pixDestroy(&pixt2);

	/* Get mask for connected regions */
	pixm = pixCloseSafeBrick(NULL, pixr, 4, 4);
	//pixWrite("tmp/ccmask.150.png", pixm, IFF_PNG);

	/* Fill seed into mask to get halftone mask */
	pixhm1 = pixSeedfillBinary(NULL, pixhs, pixm, 4);
	pixWrite("tmp/htmask.150.png", pixhm1, IFF_PNG);

	// Extract halftone stuff 
	pixht = pixAnd(NULL, pixhm1, pixr);
	pixWrite("tmp/ht.150.png", pixht, IFF_PNG);

	// Extract non-halftone stuff
	pixnht = pixXor(NULL, pixht, pixr);
	pixWrite("tmp/text.150_1.png", pixht, IFF_PNG);
	pixWrite("tmp/text.150_2.png", pixr, IFF_PNG);
	pixWrite("tmp/text.150_0.png", pixs, IFF_PNG);
	pixWrite("tmp/text.150.png", pixnht, IFF_PNG);
	pixZero(pixht, &zero);

	pixDestroy(&pixr);
	pixDestroy(&pixhs);
	pixDestroy(&pixm);
	pixDestroy(&pixhm1);
	pixDestroy(&pixht);

	return pixnht;
}

PIX * ImageProcessing::findWords(PIX *pixt1, BOXAA *baa, NUMAA *naa) {
	l_int32      i, j, w, h, ncomp;
	l_int32      index, ival, rval, gval, bval;
	BOX         *box;
	BOXA        *boxa;
	JBDATA      *data;
	JBCLASSER   *classer;
	NUMA        *nai;
	PIX         *pixt2, *pixd;
	PIXCMAP     *cmap;


	pixGetWordBoxesInTextlines(pixt1, 1, MIN_WORD_WIDTH, MIN_WORD_HEIGHT,
		MAX_WORD_WIDTH, MAX_WORD_HEIGHT,
		&boxa, &nai);
	boxaaAddBoxa(baa, boxa, L_INSERT);
	numaaAddNuma(naa, nai, L_INSERT);

	/* Show the results on a 2x reduced image, where each
	* word is outlined and the color of the box depends on the
	* computed textline. */
	pixGetDimensions(pixt1, &w, &h, NULL);
	pixd = pixCreate(w, h, 8);
	printf("%d, %d\n", w, h);
	//if ((pixCreateHeader(w, h, 8)) == NULL) printf("pixd not made\n");
	if (!pixd) printf("pix not defined\n");
	cmap = pixcmapCreateRandom(8, 1, 1);  /* first color is black */
	pixSetColormap(pixd, cmap);

	pixt2 = pixUnpackBinary(pixt1, 8, 1);
	pixRasterop(pixd, 0, 0, w, h, PIX_SRC | PIX_DST, pixt2, 0, 0);
	ncomp = boxaGetCount(boxa);
	for (j = 0; j < ncomp; j++) {
		box = boxaGetBox(boxa, j, L_CLONE);
		numaGetIValue(nai, j, &ival);
		index = 1 + (ival % 254);  /* omit black and white */
		pixcmapGetColor(cmap, index, &rval, &gval, &bval);
		pixRenderBoxArb(pixd, box, 2, rval, gval, bval);
		boxDestroy(&box);
	}

	printf("end");

	pixDestroy(&pixt2);
	return pixd;
}

void ImageProcessing::writeWords(PIX *pixs, char *dirout, char *rootname, l_int32 nr = 0) {
	char filename[BUF_SIZE];
	char targetPath[1024];
	snprintf(filename, BUF_SIZE, "%s.%05d.png", rootname, nr);
	fprintf(stderr, "filename: %s\n", filename);
	targetPath[0] = '\0';
	sprintf_s(targetPath, "%s%s%s", dirout, "/", filename);
	printf("file path: %s\n", targetPath);
	if (pixs == NULL) printf("pixd is NULL\n");
	pixWrite(targetPath, pixs, IFF_PNG);
}

void ImageProcessing::findWords2(char* dirin, char *dirout) {
	char         filename[BUF_SIZE];
	char        *rootname, *fname;
	l_int32      reduction, i, firstpage, npages, nfiles;
	l_float32    thresh, weight;
	JBDATA      *data;
	JBCLASSER   *classer;
	NUMA        *natl;
	SARRAY      *safiles;
	PIX         *pix;
	PIXA        *pixa, *pixadb;

	reduction = 2;// atoi(argv[2]);
	thresh = 0.8;// atof(argv[3]);
	weight = 0.6;// atof(argv[4]);
	rootname = "result";

	firstpage = 0;
	npages = 0;

	classer = jbWordsInTextlines(dirin, reduction, MAX_WORD_WIDTH,
		MAX_WORD_HEIGHT, thresh, weight,
		&natl, firstpage, npages);

	printf("pixa components: %d\n", pixaGetCount(classer->pixat));

	/* Save and write out the result */
	snprintf(filename, BUF_SIZE, "%s%s%s", dirout, "/", rootname);
	data = jbDataSave(classer);
	jbDataWrite(filename, data);

#if  RENDER_PAGES
	/* Render the pages from the classifier data, and write to file.
	* Use debugflag == FALSE to omit outlines of each component. */
	pixa = jbDataRender(data, FALSE);
	npages = pixaGetCount(pixa);
	printf("npages: %d\n", npages);
	for (i = 0; i < npages; i++) {
		pix = pixaGetPix(pixa, i, L_CLONE);
		snprintf(filename, BUF_SIZE, "%s%s%s.%05d.png", dirout, "/", rootname, i);
		fprintf(stderr, "filename: %s\n", filename);
		pixWrite(filename, pix, IFF_PNG);
		pixDestroy(&pix);
	}
	pixaDestroy(&pixa);
#endif  /* RENDER_PAGES */

#if  RENDER_DEBUG
	/* Use debugflag == TRUE to see outlines of each component. */
	pixadb = jbDataRender(data, TRUE);
	/* Write the debug pages out */
	npages = pixaGetCount(pixadb);
	for (i = 0; i < npages; i++) {
		pix = pixaGetPix(pixadb, i, L_CLONE);
		snprintf(filename, BUF_SIZE, "%s%s%s.db.%05d.png", dirout, "/", rootname, i);
		fprintf(stderr, "filename: %s\n", filename);
		pixWrite(filename, pix, IFF_PNG);
		pixDestroy(&pix);
	}
	pixaDestroy(&pixadb);
#endif  /* RENDER_DEBUG */

	jbClasserDestroy(&classer);
	jbDataDestroy(&data);
	numaDestroy(&natl);
}



///Sagatvo burta attēlu atpazīšanai
///Atrod burtu, nocentrē to
cv::Mat ImageProcessing::prepareImage(cv::Mat subImg) {
	//TODO needs to be improved

	Mat mSource_Gray, mThreshold;
	cvtColor(subImg, mSource_Gray, COLOR_BGR2GRAY);
	threshold(mSource_Gray, mThreshold, 254, 255, THRESH_BINARY_INV);

	Mat Points;
	findNonZero(mThreshold, Points);
	Rect Min_Rect = boundingRect(Points);

	//rectangle(subImg, Min_Rect.tl(), Min_Rect.br(), Scalar(0, 255, 0), 2);
	//imshow("Result", subImg);

	Mat tmp = subImg(Min_Rect);
	subImg.release();
	subImg = tmp;

	if (subImg.cols == 0 || subImg.rows == 0) { //attela nekas netika atrasts
		Points.release();
		mThreshold.release();
		mSource_Gray.release();
		return subImg;
	}

	int diff = subImg.cols - subImg.rows;
	int targetSize = (diff >= 0) ? subImg.cols : subImg.rows;

	bitwise_not(subImg, tmp);
	subImg.release();
	subImg = tmp;
	cv::Mat padded;
	padded.create(targetSize, targetSize, subImg.type()); //(diff+1)/2 nepieciešams, lai ari nepara diff gadijuma izveidotu kvadratu. para gadijuma tas neko nemaina
	padded.setTo(cv::Scalar::all(0));
	//cout << "s1 " << (diff / 2) << " " << subImg.cols << " " << subImg.rows << endl;
	if (diff > 0)
		subImg.copyTo(padded(Rect(0, diff / 2, subImg.cols, subImg.rows)));
	else
		subImg.copyTo(padded(Rect(- diff / 2, 0, subImg.cols, subImg.rows)));

	subImg.release();
	subImg = padded;

	bitwise_not(subImg, tmp);
	subImg.release();
	subImg = tmp;

	resize(subImg, tmp, Size(LETTER_WIDTH, LETTER_HEIGHT));
	subImg.release();
	subImg = tmp;

	Points.release();
	mThreshold.release();
	mSource_Gray.release();
	return subImg;
}

void ImageProcessing::testFoundSymbols(cv::Mat source, vector<SpacingGroup*> spacingGroups, int spacingIterationWidth, int spacingTestWidth) {
	std::vector<SymbolResult*> wordResults;
	int prevGroupX = 0;
	int nr = 0;
	struct fann *ann = fann_create_from_file("result_letters.net");
	for (int i = 0; i < spacingGroups.size(); i++) {
		if (spacingGroups[i]->groupSize > 1) { //spacing group is valid
			int currGroupX = spacingGroups[i]->startX + (spacingGroups[i]->groupSize * spacingIterationWidth + spacingTestWidth) / 2.0;

			if (spacingGroups[i]->startX > 0) { //ja grupa nav paš? v?rda s?kum?
				cout << "Xs: " << prevGroupX << " " << currGroupX << endl;
				Mat subImg = source(cv::Range(0, source.rows), cv::Range(prevGroupX, currGroupX));
				cout << "S: " << subImg.rows << " " << subImg.cols << endl;
				Mat tmp = prepareImage(subImg);
				subImg.release();
				subImg = tmp;

				if (subImg.cols == 0 || subImg.rows == 0) {
					subImg.release();
					continue;
				}

				SymbolResult *result = NeuralNetwork::testNN_image_letter(subImg, ann);
				wordResults.push_back(result);

				/*namedWindow("Coutout SubImg", CV_WINDOW_AUTOSIZE);
				imshow("Coutout SubImg", subImg);
				waitKey(0);
				destroyWindow("Coutout SubImg");*/

				string name = "../../../images/results/spacing/r_" + to_string(nr) + ".jpg";
				imwrite(name, subImg);
				subImg.release();
				nr++;
			}

			prevGroupX = currGroupX;
		}
	}
	//parbauda arī pēdējo simbolu, aiz kura nav atstarpe
	//Mat subImg = source(cv::Range(0, source.rows), cv::Range(prevGroupX, source.cols));
	//subImg = prepareImage(subImg);
	//TODO memory leak?
	//cout << subImg.cols << " " << subImg.rows << endl;

	//SymbolResult *result = NeuralNetwork::testNN_image_letter(subImg);
	//wordResults.push_back(result);

	std::string resultWord = NeuralNetwork::determineWord(wordResults);
	cout << "Result word: " << resultWord << endl;

	cout << "Clear wordResults vector" << endl;
	while(wordResults.size() > 0) {
		delete wordResults.back();
		wordResults.pop_back();
	}

	/*namedWindow("Coutout SubImg", CV_WINDOW_AUTOSIZE);
	imshow("Coutout SubImg", subImg);
	waitKey(0);
	destroyWindow("Coutout SubImg");*/
}

void ImageProcessing::iterateOverImage(Mat source) {
	//Mat source = imread("C:\\Users\\peter\\OneDrive\\Projects\\OCR\\images\\test_digits_1.png");

	int width = source.cols;
	int height = source.rows;

	int spacingIterationWidth = 1;
	int spacingTestWidth = height / 4;

	int letterIterationWidth = 1;
	int letterTestWidth = height;

	std::vector<int> spacingCoordsFound;
	int calcIdx;
	double calcProb;

	struct fann *ann = fann_create_from_file("result_spacing.net");

	// TODO test if (width >= height) {
	//iterē pāri attēlam, lai atrastu atstarpes
	int currX = 0;
	while (currX + spacingTestWidth <= width) {
		Mat subImg = source(cv::Range(0, height), cv::Range(currX, currX + spacingTestWidth));
		NeuralNetwork::testNN_image_spacing(subImg, calcIdx, calcProb, ann);

		if (calcIdx == 1 && calcProb > MIN_SPACING_PROBABILITY) { //ir atstarpe
			spacingCoordsFound.push_back(currX);
		}

		subImg.release();
		currX += spacingIterationWidth;
	}

	if (spacingCoordsFound.size() == 0) {
		cout << "TODO no spacing found (only a letter??)" << endl;
		return;
	}

	std::vector<SpacingGroup*> spacingGroups;
	spacingGroups.push_back(new SpacingGroup(spacingCoordsFound[0]));

	//apstrādā atrastās atstarpju koordinātas, sadala tās pa grupām
	for (int i = 1; i < spacingCoordsFound.size(); i++) {
		SpacingGroup *lastSpacingGroup = spacingGroups.back();

		if (spacingCoordsFound[i] <= lastSpacingGroup->startX + (lastSpacingGroup->groupSize + ALLOWED_ITERATION_ERROR) * spacingIterationWidth) { //current spacing belongs to this group
			lastSpacingGroup->Add();
		}
		else { //create a new spacing group
			spacingGroups.push_back(new SpacingGroup(spacingCoordsFound[i]));
		}
	}

	cout << "Total spacing count: " << spacingCoordsFound.size() << "\nSpacing groups found: " << spacingGroups.size() << endl;

	int validCount = 0;
	for (int i = 0; i < spacingGroups.size(); i++) {
		if (spacingGroups[i]->groupSize > 1) {
			validCount++;
		}
	}
	cout << "Valid spacing groups found: " << validCount << endl;

	testFoundSymbols(source, spacingGroups, spacingIterationWidth, spacingTestWidth);

	//clear memory
	while (!spacingGroups.empty()) {
		delete spacingGroups.back();
		spacingGroups.pop_back();
	}
}


///Atrod vektoru ar vārdiem attēlā
std::vector<cv::Rect> ImageProcessing::detectLetters(cv::Mat img)
{
	std::vector<cv::Rect> boundRect;
	cv::Mat img_gray, img_sobel, img_threshold, element;
	cvtColor(img, img_gray, CV_BGR2GRAY);
	cv::Sobel(img_gray, img_sobel, CV_8U, 1, 0, 3, 1, 0, cv::BORDER_DEFAULT);
	cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	element = getStructuringElement(cv::MORPH_RECT, cv::Size(17, 3));
	cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
	std::vector< std::vector< cv::Point> > contours;
	cv::findContours(img_threshold, contours, 0, 1);
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());
	for (int i = 0; i < contours.size(); i++)
		if (contours[i].size()>100)
		{
			cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
			cv::Rect appRect(boundingRect(cv::Mat(contours_poly[i])));
			if (appRect.width>appRect.height)
				boundRect.push_back(appRect);
		}
	return boundRect;
}

///Izgriež vārdus, izmantojot OpenCV
void ImageProcessing::findWords_cv(string filename) {

	Mat rgb = imread(filename);
	// downsample and use it for processing
	//pyrDown(large, rgb);
	Mat small;
	cvtColor(rgb, small, CV_BGR2GRAY);
	// morphological gradient
	Mat grad;
	Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
	// binarize
	Mat bw;
	threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
	// connect horizontally oriented regions
	Mat connected;
	morphKernel = getStructuringElement(MORPH_RECT, Size(rgb.cols/20, 1));
	morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
	// find contours
	Mat mask = Mat::zeros(bw.size(), CV_8UC1);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	// filter contours
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0])
	{
		Rect rect = boundingRect(contours[idx]);
		Mat maskROI(mask, rect);
		maskROI = Scalar(0, 0, 0);
		// fill the contour
		drawContours(mask, contours, idx, Scalar(255, 255, 255), CV_FILLED);
		// ratio of non-zero pixels in the filled region
		double r = (double)countNonZero(maskROI) / (rect.width*rect.height);

		if (r > .45 /* assume at least 45% of the area is filled if it contains text */
			&&
			(rect.height > 8 && rect.width > 8) /* constraints on region size */
												/* these two conditions alone are not very robust. better to use something
												like the number of significant peaks in a horizontal projection as a third condition */
			)
		{
			//rectangle(rgb, rect, Scalar(0, 255, 0), 1);

			Mat subImg = rgb(rect);

			namedWindow("Coutout SubImg", CV_WINDOW_AUTOSIZE);
			imshow("Coutout SubImg", subImg);
			waitKey(0);
			destroyWindow("Coutout SubImg");

			iterateOverImage(subImg);
		}
	}
	/*namedWindow("Coutout SubImg", CV_WINDOW_AUTOSIZE);
	imshow("Coutout SubImg", rgb);
	waitKey(0);
	destroyWindow("Coutout SubImg");*/
	//imwrite(OUTPUT_FOLDER_PATH + string("rgb.jpg"), rgb);



	/*cv::Mat img1 = cv::imread(filename);
	//Detect
	std::vector<cv::Rect> letterBBoxes1 = detectLetters(img1);
	//Display
	for (int i = 0; i < letterBBoxes1.size(); i++) {
		cv::rectangle(img1, letterBBoxes1[i], cv::Scalar(0, 255, 0), 1, 8, 0);

		Mat subImg = img1(letterBBoxes1[i]);

		namedWindow("Coutout SubImg", CV_WINDOW_AUTOSIZE);
		imshow("Coutout SubImg", subImg);
		waitKey(0);
		destroyWindow("Coutout SubImg");

		//iterateOverImage(subImg);
	}
	//cv::imwrite("imgOut1.jpg", img1);*/
}

//TODO iepējams nav nepieciešams
///Ņem lielo failu ar atrastajiem vārdiem un sagriež bildes
void ImageProcessing::cutWords() {
	ifstream in;
	in.open("../../../images/results/result.data");
	Mat source = imread("../../../images/results/result.templates.png");
	
	bool start = false;
	int wordWidth, wordHeight;
	string s;
	int widthCount, heightCount, iterator = 0;
	while (!in.eof()) {
		in >> s;
		//cout << s << " ";

		if (s == "lattice") {
			in >> s >> s >> s >> s;
			s = s.substr(0, s.length()-1);
			wordWidth = stoi(s);
			cout << "width: " << wordWidth;

			in >> s >> s >> s;
			wordHeight = stoi(s);
			cout << "height: " << wordHeight;

			widthCount = source.cols / wordWidth;
			heightCount = source.rows / wordHeight;

			start = true;
		}
		else if (start) {
			int realX, realY;
			in >> realX >> realY;

			cout << "(" << realX << ";" << realY << ")\n";

			int xPos = iterator % widthCount;
			int yPos = iterator / widthCount;

			cout << xPos << " " << yPos << endl;
			Mat subImg = source(cv::Range(yPos * wordHeight, (yPos + 1) * wordHeight), cv::Range(xPos * wordWidth, (xPos + 1) * wordWidth));

			namedWindow("Coutout SubImg", CV_WINDOW_AUTOSIZE);
			imshow("Coutout SubImg", subImg);
			waitKey(0);
			destroyWindow("Coutout SubImg");

			iterator++;
		}

		in >> s;
	}
}

void ImageProcessing::convertTo1bpp(std::string src) {
	SARRAY *safiles = getSortedPathnamesInDirectory(src.c_str(), NULL, 0, 0);
	l_int32 nfiles = sarrayGetCount(safiles);
	printf("Count received: %d\n", nfiles);
	BOXAA *baa = boxaaCreate(nfiles);
	NUMAA *naa = numaaCreate(nfiles);

	char *fname;
	PIX *pixs, *pixd;

	for (l_int32 i = 0; i < nfiles; i++) {
		printf("--- %d ---\n", i);
		fname = sarrayGetString(safiles, i, 0);
		if ((pixs = pixRead(fname)) == NULL) {
			printf("image file %s not read\n", fname);
			continue;
		}

		pixd = pixConvertTo1(pixs, 150);

		char filename[BUF_SIZE];
		snprintf(filename, BUF_SIZE, "%s", fname);
		pixWrite(filename, pixd, IFF_PNG);

		pixDestroy(&pixd);
		pixDestroy(&pixs);
	}
}

int main2() {
	char *dirin, *dirout_1bpp, *dirout_words, *dirout, *rootname, *fname;

	PIX *pixs, *pixt, *pix_deskew, *pix_light, *pix_noise, *pix_words;

	JBDATA *jbdata;
	JBCLASSER *classer;

	l_int32 firstpage = 0;
	l_int32 npages = 0;

	dirin = "../../../images/test-set";
	dirout_1bpp = "../../../images/results/1bpp";
	dirout_words = "../../../images/results/words";
	dirout = "../../../images/results";
	rootname = "result";

	/* Compute the word bounding boxes at 2x reduction, along with
	* the textlines that they are in. */
	SARRAY *safiles = getSortedPathnamesInDirectory(dirin, NULL, firstpage, npages);
	l_int32 nfiles = sarrayGetCount(safiles);
	printf("Count received: %d\n", nfiles);
	BOXAA *baa = boxaaCreate(nfiles);
	NUMAA *naa = numaaCreate(nfiles);


	printf("Begin image processing\n");

	//l_int32 w, h;
	//l_float32 deg2rad = 3.1415926535 / 180.;
	//pixGetDimensions(pixs, &w, &h, NULL);
	//pixFindSkew(pix_tmp, &angle, &conf);
	//pixs = pixRotate(pixs, deg2rad * angle, L_ROTATE_AREA_MAP, L_BRING_IN_WHITE, w, h);

	for (l_int32 i = 0; i < nfiles; i++) {
		printf("--- %d ---\n", i);
		fname = sarrayGetString(safiles, i, 0);
		if ((pixs = pixRead(fname)) == NULL) {
			printf("image file %s not read\n", fname);
			continue;
		}

		l_float32 angle, conf, score;
		l_int32 lightingVal;

		pix_deskew = pixDeskew(pixs, 0);

		pix_light = ImageProcessing::normalizeLighting(pix_deskew, &lightingVal);
		pix_noise = ImageProcessing::removeNoise(pix_light, &lightingVal);

		pixt = pixConvertTo1(pix_noise, 150);

		//create a 1bpp image in the corresponding location
		char filename[BUF_SIZE];
		snprintf(filename, BUF_SIZE, "%s%s%s.%05d.png", dirout_1bpp, "/", rootname, i);
		pixWrite(filename, pixt, IFF_PNG);

		pix_words = ImageProcessing::findWords(pixt, baa, naa);
		ImageProcessing::writeWords(pix_words, dirout_words, rootname, i);


		pixDestroy(&pixt);
		pixDestroy(&pix_light);
		pixDestroy(&pix_noise);
		pixDestroy(&pix_deskew);
		pixDestroy(&pix_words);
		pixDestroy(&pixs);
		printf("\n\n", i);
	}

	pixDestroy(&pixt);
	pixDestroy(&pix_deskew);
	pixDestroy(&pix_light);
	pixDestroy(&pix_noise);
	pixDestroy(&pix_words);
	pixDestroy(&pixs);
	boxaaDestroy(&baa);
	numaaDestroy(&naa);
	sarrayDestroy(&safiles);

	//findWords2(dirout_1bpp, dirout);

	printf("\n---\nEND\n");
	getchar();
	return 0;
}

#endif