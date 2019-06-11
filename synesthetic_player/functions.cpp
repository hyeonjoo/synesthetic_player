#include "functions.h"

void printUsage()
{
	std::cout << "Color Picker" << std::endl;
	std::cout << "Use:\tMouse left click - choose color, listen to each score" << std::endl;
	std::cout << "\tMouse right click - set color to chosen score" << std::endl;
	std::cout << "\tMouse wheel - change mode" << std::endl;
}
void HSVtoRGB(int H, double S, double V, int output[3])
{
	double C = S * V;
	double X = C * (1 - abs(fmod(H / 60.0, 2) - 1));
	double m = V - C;
	double Rs, Gs, Bs;

	if (H >= 0 && H < 60) {
		Rs = C;
		Gs = X;
		Bs = 0;
	}
	else if (H >= 60 && H < 120) {
		Rs = X;
		Gs = C;
		Bs = 0;
	}
	else if (H >= 120 && H < 180) {
		Rs = 0;
		Gs = C;
		Bs = X;
	}
	else if (H >= 180 && H < 240) {
		Rs = 0;
		Gs = X;
		Bs = C;
	}
	else if (H >= 240 && H < 300) {
		Rs = X;
		Gs = 0;
		Bs = C;
	}
	else {
		Rs = C;
		Gs = 0;
		Bs = X;
	}

	output[0] = (Rs + m) * 255;
	output[1] = (Gs + m) * 255;
	output[2] = (Bs + m) * 255;
}

void getCirclepos(int i, int r, int &x, int &y)
{
	x = 200 + CIRSIZE + r*sin(i * 30 * PI / 180);
	y = 50 + CIRSIZE - r*cos(i * 30 * PI / 180);
}

int getRectpos(int i)
{
	return 30 + i * 80;
}

void putCircle(cv::Mat mat, int x, int y, int mode, int* rgb)
{
	switch (mode) {
	case CHOSEN: cv::circle(mat, cv::Point(x, y), 41, cv::Scalar(100, 100, 100), 5, cv::LINE_AA); break;
	case SET: cv::circle(mat, cv::Point(x, y), 41, cv::Scalar(0, 0, 0), 5, cv::LINE_AA); break;
	default: cv::circle(mat, cv::Point(x, y), 45, cv::Scalar(rgb[2], rgb[1], rgb[0]), -1); break;
	}
}

void putRect(cv::Mat mat, int i, int mode, int* rgb) 
{
	int x = getRectpos(i);

	switch (mode) {
	case CHOSEN: cv::rectangle(mat, cv::Point(x, y_recs), cv::Point(x + RECSIZE, y_recs + RECSIZE), cv::Scalar(100, 100, 100), -1); break;
	case SET: cv::rectangle(mat, cv::Point(x, y_recs), cv::Point(x + RECSIZE, y_recs + RECSIZE), cv::Scalar(rgb[2], rgb[1], rgb[0]), -1); break;
	default: cv::rectangle(mat, cv::Point(x, y_recs), cv::Point(x + RECSIZE, y_recs + RECSIZE), cv::Scalar(0, 0, 0), -1); break;
	}
}

std::vector<int> getEdgeRecord(cv::Mat edge)
{
	int booked[ROW_S][COL_S] = { 0 };

	int x = rand() % COL_S;
	int y = rand() % ROW_S;
	
	std::stack<int> s_edg, s_non;
	std::vector<int> rec;

	s_non.push(y);
	s_non.push(x);

	bool is_in_area, is_in_area2;
	int val;
	int m, n;

	booked[y][x] = 1;

	while (1)
	{
		if (s_edg.empty() && s_non.empty()) break;
		else if (!s_edg.empty()) {
			x = s_edg.top();
			s_edg.pop();
			y = s_edg.top();
			s_edg.pop();
		}
		else {
			x = s_non.top();
			s_non.pop();
			y = s_non.top();
			s_non.pop();
		}

		val = edge.at<uchar>(y, x);

		if (val == 255 || val == 0) {
			if (val == 255) {
				rec.push_back(y);
				rec.push_back(x);
			}

			for (m = -1; m <= 1; m++) {
				for (n = -1; n <= 1; n++) {
					is_in_area = (x + n) >= 0 && (y + m) >= 0 && (x + n) < COL_S && (y + m) < ROW_S;

					if (is_in_area) {
						if (booked[y + m][x + n] == 1) continue;

						val = edge.at<uchar>(y + m, x + n);

						if (val == 255) {
							s_edg.push(y + m);
							s_edg.push(x + n);
							booked[y + m][x + n] = 1;
						}
						else if (val == 0) {
							s_non.push(y + m);
							s_non.push(x + n);
							booked[y + m][x + n] = 1;
						}
					}
				}
			}
		}
	}

	return rec;
}