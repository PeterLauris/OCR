#include <iostream>

#include "opencv2\opencv.hpp"
#include "opencv2\imgproc\imgproc.hpp"
#include "opencv2\core\core.hpp"

#include "allheaders.h"

#include "fann.h"

#include "dirent.h"

using namespace cv;
using namespace std;

void OCR() {
	printf("Start OCR");
}

void readImages() {
	cout << "read" << endl;
	char *dirName = "../../../images/letter-spacing/";
	string dir1bpp = "../../../images/letter-spacing/1bpp/";
	char *imgPath;

	/*SARRAY *safiles = getSortedPathnamesInDirectory(dirName, NULL, 0, 0);
	l_int32 nfiles = sarrayGetCount(safiles);

	PIX *pixs, *pixt;
	char targetPath[1024];
	char newName[512];

	for (l_int32 i = 0; i < nfiles; i++) {
		imgPath = sarrayGetString(safiles, i, 0);
		if ((pixs = pixRead(imgPath)) == NULL) {
			printf("image file %s not read\n", imgPath);
			continue;
		}

		pixt = pixConvertTo1(pixs, 150);
		sprintf_s(newName, "%s.%05d.png", "s", i);
		sprintf_s(targetPath, "%s%s%s", dirName, "/1bpp/", newName);
		pixWrite(targetPath, pixt, IFF_PNG);

		pixDestroy(&pixs);
		pixDestroy(&pixt);

	}
	sarrayDestroy(&safiles);
	cout << "Created 1bpp images of spacing images\n";*/

	DIR *dir = opendir(dir1bpp.c_str());
	string imgName;
	struct dirent *ent;
	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			string imgPath(dir1bpp + ent->d_name);
			cout << ent->d_name << "\n";
			cout << imgPath << "\n";



			Mat img = imread(imgPath);
			if (img.empty()) {
				std::cout << "Cannot load " << imgName << endl;
				continue;
			}
			//cvtColor(img, img, CV_BGR2GRAY);

			int *arr = new int[img.rows * img.cols];
			for (int y = 0; y < img.rows; y++) {
				for (int x = 0; x < img.cols; x++) {
					Vec3b intensity = img.at<Vec3b>(y, x);
					arr[y*img.cols + x] = (intensity[0] <= 128 ? 0 : 1);
				}
			}

			for (int y = 0; y < img.rows; y++) {
				for (int x = 0; x < img.cols; x++) {
					cout << arr[y*img.cols + x] << " ";
				}
				cout << "\n";
			}
			delete[] arr;

			namedWindow("MyWindow", CV_WINDOW_AUTOSIZE);
			imshow("MyWindow", img);


			waitKey(0);

			destroyWindow("MyWindow");
			
			img.release();
		}
		closedir(dir);
	}
	else {
		cout << "not present" << endl;
	}
}

void trainNN() {
	readImages();
}

int main() {
	//test FANN
	//struct fann *ann = fann_create_standard(3, 1, 3, 1);
	//fann_destroy(ann);

	//test leptonica
	//PIX *pixs;


	//test opencv
	//Mat img = imread("b&w.jpg", CV_LOAD_IMAGE_UNCHANGED);

	char c;
	do {
		cout << "### OCR ###\n" <<
				"1 - run OCR on sample data\n" <<
				"2 - run image preprocessing\n" << 
				"3 - train neural network\n" <<
				"Make a choice:";
		//cin >> c;
		c = '3';

		switch (c) {
		case '1':
			OCR();
			break;
		case '2':

			break;
		case '3':
			trainNN();
			break;
		default:
			return 0;
		}
		cout << "Press any key to continue...\n\n";
		getchar();
	} while (true);

	return 0;
}