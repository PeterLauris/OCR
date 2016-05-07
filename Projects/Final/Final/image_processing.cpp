#ifndef _IMAGE_PROC_
#define _IMAGE_PROC_

#include <stdlib.h>
#include <string>

#include "image_processing.h"
#include "neural_network.h"
#include "language_model.h"
#include "utilities.h"


using namespace std;
using namespace cv;

void print(Mat img) {
	for (int y = 0; y < img.rows; y++) {
		for (int x = 0; x < img.cols; x++) {
			int pix = (int)img.at<uchar>(y, x);
			cout << pix << " ";
		}
		cout << "\n";
	}
}

///Sagatvo burta attēlu atpazīšanai
///Atrod burtu, nocentrē to
cv::Mat ImageProcessing::prepareImage(cv::Mat subImg) {
	//TODO needs to be improved

	Mat mSource_Gray, mThreshold;
	//cvtColor(subImg, mSource_Gray, COLOR_BGR2GRAY);
	//threshold(mSource_Gray, mThreshold, 254, 255, THRESH_BINARY_INV);

	subImg = removeNoiseBlobs(subImg);

	Mat Points, tmp;
	bitwise_not(subImg, subImg);
	findNonZero(subImg, Points);
	if (Points.empty()) {
		cout << "Points are empty!!!";
		//showImage(subImg);
		return Mat();
	}
	Rect Min_Rect = boundingRect(Points);

	bitwise_not(subImg, subImg);

	//tmp = subImg.clone();
	//rectangle(tmp, Min_Rect.tl(), Min_Rect.br(), Scalar(0, 255, 0), 2);
	//showImage(tmp);

	tmp = subImg(Min_Rect);
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

	bitwise_not(subImg, subImg);

	resize(subImg, subImg, Size(LETTER_WIDTH, LETTER_HEIGHT));

	subImg = convertImageToBinary(subImg);

	//print(subImg);
	//getchar();

	Points.release();
	mThreshold.release();
	mSource_Gray.release();
	return subImg;
}

void ImageProcessing::testFoundSymbols(cv::Mat source, vector<SpacingGroup*> spacingGroups, int spacingIterationWidth, int spacingTestWidth) {
	ofstream out("results.txt");
	std::vector<SymbolResult*> wordResults;
	int prevGroupX = 0;
	//int nr = 0;
	cout << "Image height: " << source.rows << endl;
	//showImage(source);
	bool isSpacingLast = false;//vai vārda beigās ir atstarpes
	struct fann *ann = fann_create_from_file("result_letters.net");
	for (int i = 0; i < spacingGroups.size(); i++) {
		int currGroupEndX = spacingGroups[i]->startX + (spacingGroups[i]->groupSize * spacingIterationWidth + spacingTestWidth); //iegūst spacing beigas
		cout << source.cols << " " << currGroupEndX << endl;
		if (currGroupEndX == source.cols) {
			isSpacingLast = true;
		}

		if (spacingGroups[i]->groupSize >= 1) { //spacing group is valid
			int currGroupX = spacingGroups[i]->startX + (spacingGroups[i]->groupSize * spacingIterationWidth + spacingTestWidth) / 2.0; //iegūst spacing vidu

			if (spacingGroups[i]->startX > 0) { //ja grupa nav paš? v?rda s?kum?
				//cout << "Xs: " << prevGroupX << " " << currGroupX << endl;
				Mat subImg = source(cv::Range(0, source.rows), cv::Range(prevGroupX, currGroupX));
				//cout << "S: " << subImg.rows << " " << subImg.cols << endl;
				subImg = convertImageToBinary(subImg);
				if (Utilities::isImageEmpty(subImg)) {
					continue;
				}
				subImg = prepareImage(subImg);
				//subImg = convertImageToBinary(subImg);
				//subImg = removeNoiseBlobs(subImg);
				if (Utilities::isImageEmpty(subImg)) {
					continue;
				}
				//subImg = prepareImage(subImg);

				if (subImg.cols == 0 || subImg.rows == 0) {
					subImg.release();
					continue;
				}

				SymbolResult *result = NeuralNetwork::testNN_image_letter(subImg, ann);
				if (result != NULL) {
					wordResults.push_back(result);
					//showImage(subImg, "Cutout SubImg");
					string name = "../../../images/results/spacing/r_" + to_string(outputNr) + ".jpg";
					imwrite(name, subImg);
					subImg.release();
					outputNr++;
				}
			}
			//TODO uncomment
			//if (spacingGroups[i]->groupSize > SPACE_MIN_SPACING_COUNT) { //there is a space between the letters
			//	std::string resultWord = LanguageModel::determineWord(wordResults);
			//	out << resultWord << " ";

			//	while (wordResults.size() > 0) {
			//		delete wordResults.back();
			//		wordResults.pop_back();
			//	}
			//}

			prevGroupX = currGroupX;
		}
	}
	//parbauda arī pēdējo simbolu, aiz kura nav atstarpe
	Mat subImg = source(cv::Range(0, source.rows), cv::Range(prevGroupX, source.cols));
	if (Utilities::isImageEmpty(subImg)) {
		
	}
	else {
		Mat tmp = prepareImage(subImg);
		subImg.release();
		subImg = tmp;
		//TODO memory leak?
		//cout << subImg.cols << " " << subImg.rows << endl;

		if (subImg.cols == 0 || subImg.rows == 0 || isSpacingLast) {
			cout << "Invalid (empty) letter image" << endl;
			subImg.release();
		}
		else {
			SymbolResult *result = NeuralNetwork::testNN_image_letter(subImg, ann);
			if (result != NULL) {
				wordResults.push_back(result);
				string name = "../../../images/results/spacing/r_" + to_string(outputNr) + ".jpg";
				imwrite(name, subImg);
				subImg.release();
				outputNr++;
			}
		}
	}

	std::string resultWord = LanguageModel::determineWord(wordResults);
	out << resultWord << " ";

	cout << "Clear wordResults vector" << endl;
	while (wordResults.size() > 0) {
		delete wordResults.back();
		wordResults.pop_back();
	}
	out.close();
	bitwise_not(source, source);
	//showImage(source);

	//showImage(subImg, "Cutout SubImg");
}

void ImageProcessing::iterateOverImage(Mat source) {
	//Mat source = imread("C:\\Users\\peter\\OneDrive\\Projects\\OCR\\images\\test_digits_1.png");

	Mat smaller = source;
	//cv::resize(source, smaller, Size(source.cols/3, source.rows/3));

	int width = smaller.cols;
	int height = smaller.rows;

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
		Mat subImg = smaller(cv::Range(0, height), cv::Range(currX, currX + spacingTestWidth));
		//subImg = removeNoiseBlobs(subImg);
		NeuralNetwork::testNN_image_spacing(subImg, calcIdx, calcProb, ann);

		//cout << "Calc prob: " << calcProb << endl;
		//showImage(subImg);
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
		if (spacingGroups[i]->groupSize >= 1) {
			validCount++;
		}
	}
	cout << "Valid spacing groups found: " << validCount << endl;

	testFoundSymbols(smaller, spacingGroups, spacingIterationWidth, spacingTestWidth);

	//clear memory
	while (!spacingGroups.empty()) {
		delete spacingGroups.back();
		spacingGroups.pop_back();
	}
}

///Atrod vektoru ar vārdiem attēlā
std::vector<cv::Rect> ImageProcessing::detectLetters(cv::Mat img) {
	std::vector<cv::Rect> boundRect;
	cv::Mat img_gray, img_sobel, img_threshold, element;
	//cvtColor(img, img_gray, CV_BGR2GRAY);
	img_gray = img;
	cv::Sobel(img_gray, img_sobel, CV_8U, 4, 4, 15, 1, 0, cv::BORDER_DEFAULT);
	cv::threshold(img_sobel, img_threshold, 0, 255, CV_THRESH_OTSU + CV_THRESH_BINARY);
	element = getStructuringElement(cv::MORPH_RECT, cv::Size(6, 3));
	cv::morphologyEx(img_threshold, img_threshold, CV_MOP_CLOSE, element); //Does the trick
	std::vector< std::vector< cv::Point> > contours;
	cv::findContours(img_threshold, contours, 0, 1);
	std::vector<std::vector<cv::Point> > contours_poly(contours.size());
	for (int i = 0; i < contours.size(); i++)
		if (contours[i].size()>143) {
			cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
			cv::Rect appRect(boundingRect(cv::Mat(contours_poly[i])));
			//if (appRect.width>appRect.height)
				boundRect.push_back(appRect);
		}
	return boundRect;
}

///Izgriež vārdus, izmantojot OpenCV
void ImageProcessing::findWords_cv(cv::Mat rgb) {

	//rgb = imread("../../../images/pages/word_finding_1.png");
	// downsample and use it for processing
	//pyrDown(large, rgb);
	Mat small = rgb;
	//cvtColor(rgb, small, CV_BGR2GRAY);
	// morphological gradient
	Mat grad;
	Mat morphKernel = getStructuringElement(MORPH_ELLIPSE, Size(3, 3));
	morphologyEx(small, grad, MORPH_GRADIENT, morphKernel);
	// binarize
	Mat bw;
	threshold(grad, bw, 0.0, 255.0, THRESH_BINARY | THRESH_OTSU);
	// connect horizontally oriented regions
	Mat connected;
	int sizeX = rgb.cols / 20;
	morphKernel = getStructuringElement(MORPH_RECT, Size(sizeX, 1));
	morphologyEx(bw, connected, MORPH_CLOSE, morphKernel);
	// find contours
	Mat mask = Mat::zeros(bw.size(), CV_8UC1);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(connected, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
	// filter contours
	for (int idx = 0; idx >= 0; idx = hierarchy[idx][0]) {
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
			rectangle(rgb, rect, Scalar(0, 0, 0), 2);
			//Mat subImg = rgb(rect);
			////iterateOverImage(subImg);
			//showImage(subImg, "Cutout SubImg");

			//subImg.release();
		}
	}

	//showImage(rgb, "Cutout SubImg");
	imwrite(string("rgb.jpg"), rgb);
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

			//showImage(subImg, "Coutout SubImg");

			iterator++;
		}

		in >> s;
	}
}

void ImageProcessing::showImage(cv::Mat img, std::string name) {
	namedWindow(name, CV_WINDOW_AUTOSIZE);
	imshow(name, img);
	waitKey(0);
	destroyWindow(name);
}

//http://stackoverflow.com/questions/1585535/convert-rgb-to-black-white-in-opencv
cv::Mat ImageProcessing::convertImageToBinary(cv::Mat source) {
	Mat res = source.clone();
	if (source.type() != CV_8UC1)
		cvtColor(source, res, CV_RGB2GRAY);
	////showImage(im_gray);
	//Mat img_bw = im_gray > 210;
	for (int y = 0; y < res.rows; y++) {
		for (int x = 0; x < res.cols; x++) {
			int pix = (int)res.at<uchar>(y, x);
			res.at<uchar>(y, x) = (pix <= 210 ? 0 : 255); //otrādi, nekā, pārveidojot uz string
		}
	}

	return res;
}

cv::Mat ImageProcessing::removeNoise(cv::Mat source) {
	cv::Mat res1, res2;
	medianBlur(source, res1, 3);
	if (res1.empty()) {
		cout << "Noise result is empty!" << endl;
	}

	res1 = convertImageToBinary(res1);
	res1 = removeNoiseBlobs(res1);
	return res1;
}

cv::Mat ImageProcessing::removeNoiseBlobs(cv::Mat res1) {
	double threshold = 20;

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<int> small_blobs;
	double contour_area;
	Mat temp_image;

	res1 = convertImageToBinary(res1);

	for (int y = 0; y < res1.rows; y++) {
		for (int x = 0; x < res1.cols; x++) {
			if (y == 0 || y == res1.rows - 1 || x == 0 || x == res1.cols - 1)
				res1.at<uchar>(y, x) = 255;
		}
	}

	bitwise_not(res1, res1);
	// find all contours in the binary image
	res1.copyTo(temp_image);
	findContours(temp_image, contours, hierarchy, CV_RETR_LIST,
		CV_CHAIN_APPROX_NONE);

	// Find indices of contours whose area is less than `threshold` 
	if (!contours.empty()) {
		for (size_t i = 0; i<contours.size(); ++i) {
			contour_area = contourArea(contours[i]);
			if (contour_area < threshold)
				small_blobs.push_back(i);
		}
	}

	// fill-in all small contours with zeros
	for (size_t i = 0; i < small_blobs.size(); ++i) {
		drawContours(res1, contours, small_blobs[i], cv::Scalar(0),
			CV_FILLED, 8);
	}
	bitwise_not(res1, res1);

	return res1;
}

//http://stackoverflow.com/questions/24046089/calculating-skew-of-text-opencv
double compute_skew(cv::Mat img) {
	// Binarize
	cv::threshold(img, img, 225, 255, cv::THRESH_BINARY);

	// Invert colors
	cv::bitwise_not(img, img);

	cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 3));
	cv::erode(img, img, element);

	std::vector<cv::Point> points;
	cv::Mat_<uchar>::iterator it = img.begin<uchar>();
	cv::Mat_<uchar>::iterator end = img.end<uchar>();
	for (; it != end; ++it)
		if (*it)
			points.push_back(it.pos());

	cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

	double angle = box.angle;
	if (angle < -45.)
		angle += 90.;

	cv::Point2f vertices[4];
	box.points(vertices);
	for (int i = 0; i < 4; ++i)
		cv::line(img, vertices[i], vertices[(i + 1) % 4], cv::Scalar(255, 0, 0), 1, CV_AA);

	return angle;
}

cv::Mat ImageProcessing::deskewImage(cv::Mat source) {
	cv::Mat img = source.clone();
	if (img.empty()) {
		cout << "Deskew image is empty!" << endl;
	}
	//img = convertImageToBinary(img);
	//img = removeNoise(img);

	double angle = compute_skew(img.clone());
	std::cout << "Skew angle: " << angle << std::endl;


	cv::bitwise_not(img, img);

	std::vector<cv::Point> points;
	cv::Mat_<uchar>::iterator it = img.begin<uchar>();
	cv::Mat_<uchar>::iterator end = img.end<uchar>();
	for (; it != end; ++it)
		if (*it)
			points.push_back(it.pos());

	cv::RotatedRect box = cv::minAreaRect(cv::Mat(points));

	cv::Mat rot_mat = cv::getRotationMatrix2D(box.center, angle, 1);

	cv::Mat rotated;
	cv::warpAffine(img, rotated, rot_mat, img.size(), cv::INTER_CUBIC);

	cv::Size box_size = box.size;
	if (box.angle < -45.)
		std::swap(box_size.width, box_size.height);
	cv::Mat cropped;
	cv::getRectSubPix(rotated, box_size, box.center, cropped);

	cv::bitwise_not(cropped, cropped);

	return cropped;
}

cv::Mat ImageProcessing::setBrightness(cv::Mat source, int limit) {
	Ptr<CLAHE> clahe = createCLAHE();
	clahe->setClipLimit(limit);

	Mat dst;
	clahe->apply(source, dst);

	return dst;
}

//http://docs.opencv.org/2.4/doc/tutorials/core/basic_linear_transform/basic_linear_transform.html
cv::Mat ImageProcessing::setContrast(cv::Mat source) {
	Mat new_image = Mat::zeros(source.size(), source.type());

	/// Initialize values
	std::cout << " Basic Linear Transforms " << std::endl;
	std::cout << "-------------------------" << std::endl;
	//std::cout << "* Enter the alpha value [1.0-3.0]: "; std::cin >> alpha;
	//std::cout << "* Enter the beta value [0-100]: "; std::cin >> beta;
	double alpha = 1.55;
	int beta = 25;

	/// Do the operation new_image(i,j) = alpha*image(i,j) + beta
	for (int y = 0; y < source.rows; y++) {
		for (int x = 0; x < source.cols; x++) {
			for (int c = 0; c < 3; c++) {
				new_image.at<Vec3b>(y, x)[c] = saturate_cast<uchar>(alpha*(source.at<Vec3b>(y, x)[c]) + beta);
			}
		}
	}

	return new_image;
}

//int main2() {
//	char *dirin, *dirout_1bpp, *dirout_words, *dirout, *rootname, *fname;
//
//	PIX *pixs, *pixt, *pix_deskew, *pix_light, *pix_noise, *pix_words;
//
//	JBDATA *jbdata;
//	JBCLASSER *classer;
//
//	l_int32 firstpage = 0;
//	l_int32 npages = 0;
//
//	dirin = "../../../images/test-set";
//	dirout_1bpp = "../../../images/results/1bpp";
//	dirout_words = "../../../images/results/words";
//	dirout = "../../../images/results";
//	rootname = "result";
//
//	/* Compute the word bounding boxes at 2x reduction, along with
//	* the textlines that they are in. */
//	SARRAY *safiles = getSortedPathnamesInDirectory(dirin, NULL, firstpage, npages);
//	l_int32 nfiles = sarrayGetCount(safiles);
//	printf("Count received: %d\n", nfiles);
//	BOXAA *baa = boxaaCreate(nfiles);
//	NUMAA *naa = numaaCreate(nfiles);
//
//
//	printf("Begin image processing\n");
//
//	//l_int32 w, h;
//	//l_float32 deg2rad = 3.1415926535 / 180.;
//	//pixGetDimensions(pixs, &w, &h, NULL);
//	//pixFindSkew(pix_tmp, &angle, &conf);
//	//pixs = pixRotate(pixs, deg2rad * angle, L_ROTATE_AREA_MAP, L_BRING_IN_WHITE, w, h);
//
//	for (l_int32 i = 0; i < nfiles; i++) {
//		printf("--- %d ---\n", i);
//		fname = sarrayGetString(safiles, i, 0);
//		if ((pixs = pixRead(fname)) == NULL) {
//			printf("image file %s not read\n", fname);
//			continue;
//		}
//
//		l_float32 angle, conf, score;
//		l_int32 lightingVal;
//
//		pix_deskew = pixDeskew(pixs, 0);
//
//		pix_light = ImageProcessing::normalizeLighting(pix_deskew, &lightingVal);
//		pix_noise = ImageProcessing::removeNoise(pix_light, &lightingVal);
//
//		pixt = pixConvertTo1(pix_noise, 150);
//
//		//create a 1bpp image in the corresponding location
//		char filename[BUF_SIZE];
//		snprintf(filename, BUF_SIZE, "%s%s%s.%05d.png", dirout_1bpp, "/", rootname, i);
//		pixWrite(filename, pixt, IFF_PNG);
//
//		pix_words = ImageProcessing::findWords(pixt, baa, naa);
//		ImageProcessing::writeWords(pix_words, dirout_words, rootname, i);
//
//
//		pixDestroy(&pixt);
//		pixDestroy(&pix_light);
//		pixDestroy(&pix_noise);
//		pixDestroy(&pix_deskew);
//		pixDestroy(&pix_words);
//		pixDestroy(&pixs);
//		printf("\n\n", i);
//	}
//
//	pixDestroy(&pixt);
//	pixDestroy(&pix_deskew);
//	pixDestroy(&pix_light);
//	pixDestroy(&pix_noise);
//	pixDestroy(&pix_words);
//	pixDestroy(&pixs);
//	boxaaDestroy(&baa);
//	numaaDestroy(&naa);
//	sarrayDestroy(&safiles);
//
//	//findWords2(dirout_1bpp, dirout);
//
//	printf("\n---\nEND\n");
//	getchar();
//	return 0;
//}

#endif