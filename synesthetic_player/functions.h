#include <cmath>
#include <iostream>
#include <stack>
#include <ctime>

#ifndef OPENCV_INCLUDE
#define OPENCV_INCLUDE
#include <opencv2\opencv.hpp>
#endif

#ifndef MODESET
#define MODESET
#define NOTYET	0
#define CHOSEN	1
#define SET		2
#endif

#define PI		3.14159265

#define ROW_P	800
#define COL_P	1000
#define ROW_S	400
#define COL_S	600
#define CIRSIZE	300
#define RECSIZE	60

const int y_recs = ROW_P - 80 - 1;

enum tone {
	C3,
	Cs3,
	D3,
	Ds3,
	E3,
	F3,
	Fs3,
	G3,
	Gs3,
	A3,
	As3,
	B3,
	C4,
	Cs4,
	D4,
	Ds4,
	E4,
	F4,
	Fs4,
	G4,
	Gs4,
	A4,
	As4,
	B4
};

void printUsage();
void HSVtoRGB(int H, double S, double V, int output[3]);

void getCirclepos(int i, int r, int &x, int &y);
int getRectpos(int i);
void putCircle(cv::Mat mat, int x, int y, int mode, int* rgb);
void putRect(cv::Mat mat, int i, int mode, int* rgb);
std::vector<int> getEdgeRecord(cv::Mat edge);