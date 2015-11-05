#include "opencv2\opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;


int main(int argc, const char** argv)
{
	int inputLayerSize = 1;
	int outputLayerSize = 1;
	int numSamples = 6;
	vector<int> layerSizes = { inputLayerSize, 3, 3, outputLayerSize };
	Ptr<ml::ANN_MLP> nnPtr = ml::ANN_MLP::create();
	nnPtr->setLayerSizes(layerSizes);
	nnPtr->setBackpropWeightScale(5);
	nnPtr->setActivationFunction(cv::ml::ANN_MLP::SIGMOID_SYM);

	Mat samples(Size(inputLayerSize, numSamples), CV_32F);
	samples.at<float>(Point(0, 0)) = 0.1f;
	samples.at<float>(Point(0, 1)) = 0.2f;
	samples.at<float>(Point(0, 2)) = 0.3f;
	samples.at<float>(Point(0, 3)) = 0.4f;
	samples.at<float>(Point(0, 4)) = 0.5f;
	samples.at<float>(Point(0, 5)) = 0.6f;
	Mat responses(Size(outputLayerSize, numSamples), CV_32F);
	responses.at<float>(Point(0, 0)) = 0.1f;
	responses.at<float>(Point(0, 1)) = 0.4f;
	responses.at<float>(Point(0, 2)) = 0.9f;
	responses.at<float>(Point(0, 3)) = 1.6f;
	responses.at<float>(Point(0, 4)) = 2.5f;
	responses.at<float>(Point(0, 5)) = 3.6f;

	cout << "samples:\n" << samples << endl;
	cout << "\nresponses:\n" << responses << endl;

	if (!nnPtr->train(samples, ml::ROW_SAMPLE, responses))
		return 1;
	cout << "\nweights[0]:\n" << nnPtr->getWeights(0) << endl;
	cout << "\nweights[1]:\n" << nnPtr->getWeights(1) << endl;
	cout << "\nweights[2]:\n" << nnPtr->getWeights(2) << endl;
	cout << "\nweights[3]:\n" << nnPtr->getWeights(3) << endl;

	Mat output;
	nnPtr->predict(samples, output);
	cout << "\noutput:\n" << output << endl;

	getchar();
	return 0;


	Mat img = imread("b&w.jpg", CV_LOAD_IMAGE_UNCHANGED); //read the image data in the file "MyPic.JPG" and sto

	vector<Rect> rects;

	int imageWidth = img.cols;
	int imageHeight = img.rows;

	int step = 64;

	int winWidth = imageWidth;
	int winHeight = imageHeight; //128

	for (int i = 0; i<img.rows; i += step)
	{
		if ((i + winHeight)>img.rows) { break; }
		for (int j = 0; j<img.cols; j += step)
		{
			if ((j + winWidth)>img.cols) { break; }
			Rect rect(j, i, winWidth, winHeight);
			rects.push_back(rect);
		}
	}

	printf("\nStarting rect processing...\n");
	for (int i = 0; i < rects.size(); i++) {
		printf("rect #%d: %d %d\n", i+1, rects[i].x, rects[i].y);
		
		Mat tmpImg = img.colRange(rects[i].x, rects[i].x + winWidth).rowRange(rects[i].y, rects[i].y + winHeight);

		namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
		imshow("MyWindow", tmpImg);

		int *arr = new int[tmpImg.rows * tmpImg.cols];

		for (int y = 0; y < tmpImg.rows; y++) {
			for (int x = 0; x < tmpImg.cols; x++) {
				Vec3b &intensity = tmpImg.at<Vec3b>(y, x);
				arr[y*tmpImg.cols + x] = (intensity[0] <= 128 ? 0 : 1);
			}
		}


		for (int y = 0; y < tmpImg.rows; y++) {
			for (int x = 0; x < tmpImg.cols; x++) {
				cout << arr[y*tmpImg.cols + x] << " ";
			}
			cout << "\n";
		}




		delete[] arr;
		
		waitKey(0);

		destroyWindow("MyWindow");
	}

	/*if (img.empty()) //check whether the image is loaded or not
	{
		cout << "Error : Image cannot be loaded..!!" << endl;
		system("pause"); //wait for a key press
		return -1;
	}

	namedWindow("MyWindow", CV_WINDOW_AUTOSIZE); //create a window with the name "MyWindow"
	imshow("MyWindow", img); //display the image which is stored in the 'img' in the "MyWindow" window

	waitKey(0); //wait infinite time for a keypress

	destroyWindow("MyWindow"); //destroy the window with the name, "MyWindow"*/


	getchar();

	return 0;
}