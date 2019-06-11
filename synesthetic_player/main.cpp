#include <iostream>
#include <thread>
#include <cstdlib>

#ifndef OPENCV_INCLUDE
#define OPENCV_INCLUDE
#include <opencv2\opencv.hpp>
#endif
#include "SoundClass.h"
#include "functions.h"

#define PITCH	132000
#define N		12

using namespace cv;
using namespace std;

struct OPTIONS {
	OPTIONS() : X(-1), Y(-1), drawing_dot(false) {}
	int X;
	int Y;
	int drawing_dot;
};
OPTIONS options;
bool state_brushing = 0;

int mode;

void my_mouse_callback(int event, int x, int y, int flags, void *param)
{
	switch (event) {
	case CV_EVENT_LBUTTONUP:
		options.X = x;
		options.Y = y;
		options.drawing_dot = event;
		break;
	case CV_EVENT_RBUTTONUP:
		options.X = x;
		options.Y = y;
		options.drawing_dot = event;
		break;
	case CV_EVENT_MOUSEWHEEL:
		options.drawing_dot = event; 
		break;
	}
}


int main()
{
	Mat palette = Mat(ROW_P, COL_P, CV_8UC3, Scalar(255, 255, 255));

	HWND hWnd;

	IDirectSoundBuffer8* g_dsb8Sample1;
	IDirectSoundBuffer8* g_dsb8Sample2;
	SoundClass* g_scSound;

	clock_t start;
	bool play = 0;
	int key;
	int x, y, i, j;
	int x_recs[N];
	
	g_dsb8Sample1 = 0;
	g_dsb8Sample2 = 0;
	g_scSound = new SoundClass();

	int r = 250;
	int rgb[N][3];
	int isClrsndSet[N] = { 0 };
	int isSndclrSet[N] = { 0 };
	int clrsndMat[N] = { 0 };
	int sndclrMat[N] = { 0 };

	int prev_sound = -1, prev_color = -1;
	Scalar color;

	int waitForEnter = 0;
	int flagNext = 0;

	int toneMap[12][12] = { 0, };

	for (i = 0; i < N; i++) {
		toneMap[i][i] = 2;
		toneMap[i][(i + 4) % N] = 2;
		toneMap[i][(i + 5) % N] = 2;
		toneMap[i][(i + 7) % N] = 2;
		toneMap[i][(i + 2) % N] = 1;
		toneMap[i][(i + 9) % N] = 1;
		toneMap[i][(i + 11) % N] = 1;
	}

	std::srand(time(NULL));

	for (i = 0; i < N; i++) {
		getCirclepos(i, r, x, y);

		if(mode)	HSVtoRGB(i * 30, 0.9, 0.8, rgb[i]);
		else		HSVtoRGB(i * 30, 0.5, 0.6, rgb[i]);

		putCircle(palette, x, y, NOTYET, rgb[i]);
	}

	for (i = 0; i < N; i++)
		x_recs[i] = getRectpos(i);

	for (i = 0; i < N; i++)
		putRect(palette, i, NOTYET, NULL);

	namedWindow("Palette");
	setMouseCallback("Palette", my_mouse_callback, 0);
	cv::imshow("Palette", palette);

	hWnd = GetConsoleWindow();

	if (!g_scSound->Initialize(hWnd)) {
		std::cout << "Err: Sound Initialize" << endl;
		return -1;
	}
	if (!g_scSound->LoadWaveFile("sum_flute.wav", &g_dsb8Sample1)) {
		std::cout << "Err: LoadWaveFile1" << endl;
		return -1;
	}
	if (!g_scSound->LoadWaveFile("sum_flute.wav", &g_dsb8Sample2)) {
		std::cout << "Err: LoadWaveFile2" << endl;
		return -1;
	}

	String filename = "doona.jpg";
	Mat src = imread(filename.c_str());
	Mat src_gray, detected_edges;
	cv::resize(src, src, Size(COL_S, ROW_S), 0, 0, CV_INTER_NN);
	cv::cvtColor(src, src_gray, CV_BGR2GRAY);
	cv::blur(src_gray, detected_edges, Size(3, 3));
	cv::Canny(detected_edges, detected_edges, 25, 25 * 3, 3);
	/*
	int count = 0;

	for (i = 0; i < ROW_S; i++) {
		for (j = 0; j < COL_S; j++) {
			if (detected_edges.at<uchar>(i, j) == 255)
				count++;
		}
	}

	std::cout << count << endl;
	*/
	while (1) {
		if (play)
			if ((double)(clock() - start) >= 1500) {
				play = 0;
				//cout << "music stop" << endl;
				if (!g_scSound->StopWaveFile(&g_dsb8Sample1)) {
					std::cout << "Err: StopWaveFile" << endl;
					return 0;
				}
			}

		if (options.drawing_dot != -1) {
			switch (options.drawing_dot)
			{
			case CV_EVENT_MOUSEWHEEL:
				mode = !mode;
				std::cout << "mode changed" << endl;

				for (i = 0; i < N; i++)	{
					if (mode)	HSVtoRGB(i * 30, 0.9, 0.8, rgb[i]);
					else		HSVtoRGB(i * 30, 0.5, 0.6, rgb[i]);
				}

				for (i=0;i<N;i++) {
					getCirclepos(i, r, x, y);
					putCircle(palette, x, y, NOTYET, rgb[i]);

					if (isClrsndSet[i] == SET)		putCircle(palette, x, y, SET, NULL);

					if (isSndclrSet[i] == SET)		putRect(palette, i, SET, rgb[sndclrMat[i]]);
					else							putRect(palette, i, NOTYET, NULL);
					
					if (isClrsndSet[i] == CHOSEN)	isClrsndSet[i] = NOTYET;
					if (isSndclrSet[i] == CHOSEN)	isSndclrSet[i] = NOTYET;
				}

				prev_color = -1;
				prev_sound = -1;

				break;
			case CV_EVENT_RBUTTONUP:
				if (isClrsndSet[prev_color] == CHOSEN && isSndclrSet[prev_sound] == CHOSEN) {
					getCirclepos(prev_color, r, x, y);
					putCircle(palette, x, y, SET, NULL);
					putRect(palette, prev_sound, SET, rgb[prev_color]);

					isClrsndSet[prev_color] = SET;
					isSndclrSet[prev_sound] = SET;
					clrsndMat[prev_color] = prev_sound;
					sndclrMat[prev_sound] = prev_color;

					prev_color = -1;
					prev_sound = -1;
				}
				else {
					color = palette.at<Vec3b>(options.Y, options.X);

					for (i = 0; i < N; i++) {
						if (color[2] == rgb[i][0] && color[1] == rgb[i][1] && color[0] == rgb[i][2]) {
							if (options.Y < 650) {
								if (isClrsndSet[i] == SET) {
									if (prev_color != -1) {
										getCirclepos(prev_color, r, x, y);
										putCircle(palette, x, y, NOTYET, rgb[prev_color]);
										isClrsndSet[prev_color] = NOTYET;
									}
									if (prev_sound != -1) {
										putRect(palette, i, NOTYET, NULL); 
										isSndclrSet[i] = NOTYET;
									}

									getCirclepos(i, r, x, y);
									putCircle(palette, x, y, CHOSEN, NULL);
									isClrsndSet[i] = CHOSEN;

									prev_color = i;

									for (int j = 0; j < N; j++) {
										if (sndclrMat[j] == i) {
											i = j;
											break;
										}
									}

									putRect(palette, i, CHOSEN, NULL);
									isSndclrSet[i] = CHOSEN;

									prev_sound = i;

									clrsndMat[prev_color] = prev_sound;
									sndclrMat[prev_sound] = prev_color;
								}
							}
							else {
								for (int j = 0; j < N; j++) {
									if (sndclrMat[j] == i) {
										i = j;
										break;
									}
								}

								if (isSndclrSet[i] == SET) {
									if (prev_color != -1) {
										getCirclepos(prev_color, r, x, y);
										putCircle(palette, x, y, NOTYET, rgb[prev_color]);
										isClrsndSet[prev_color] = NOTYET;
									}
									if (prev_sound != -1) {
										putRect(palette, i, NOTYET, NULL);
										isSndclrSet[i] = NOTYET;
									}

									putRect(palette, i, CHOSEN, NULL);
									isSndclrSet[i] = CHOSEN;

									prev_sound = i;

									for (int j = 0; j < N; j++) {
										if (clrsndMat[j] == i) {
											i = j;
											break;
										}
									}

									getCirclepos(i, r, x, y);
									putCircle(palette, x, y, CHOSEN, NULL);
									isClrsndSet[i] = CHOSEN;

									prev_color = i;

									clrsndMat[prev_color] = prev_sound;
									sndclrMat[prev_sound] = prev_color;
								}

							}
							break;
						}
					}
				}
				break;
			case CV_EVENT_LBUTTONUP:
				color = palette.at<Vec3b>(options.Y, options.X);
				//cout << options.Y << '\t' << options.X << endl;
				//cout << color << endl;

				for (i = 0; i < N; i++) {
					if (color[2] == rgb[i][0] && color[1] == rgb[i][1] && color[0] == rgb[i][2]) {
						if (isClrsndSet[i] == NOTYET) {
							if (prev_color == -1 || isClrsndSet[prev_color] == SET) {
								getCirclepos(i, r, x, y);
								putCircle(palette, x, y, CHOSEN, NULL);
								isClrsndSet[i] = CHOSEN;

								prev_color = i;
							}
							else {
								getCirclepos(prev_color, r, x, y);
								putCircle(palette, x, y, NOTYET, rgb[prev_color]);
								isClrsndSet[prev_color] = NOTYET;

								getCirclepos(i, r, x, y);
								putCircle(palette, x, y, CHOSEN, NULL);
								isClrsndSet[i] = CHOSEN;

								prev_color = i;
							}
						}
						else if (isClrsndSet[i] == CHOSEN) {
							putCircle(palette, x, y, NOTYET, rgb[i]);
							isClrsndSet[i] = NOTYET;

							prev_color = -1;
						}
						break;
					}
				}

				for (i = 0; i < N; i++) {
					if (options.X >= x_recs[i] && options.X < x_recs[i] + RECSIZE &&
						options.Y >= y_recs		&& options.Y < y_recs + RECSIZE) {

						play = 1;

						//for (int j = 0; j < N; j++)
						//	std::cout << isSndclrSet[j] << ' ';
						//std::cout << endl;

						if (isSndclrSet[i] == NOTYET) {
							if (prev_sound == -1 || isSndclrSet[prev_sound] == SET) {
								putRect(palette, i, CHOSEN, NULL);
								isSndclrSet[i] = CHOSEN;

								prev_sound = i;
							}
							else {
								putRect(palette, prev_sound, NOTYET, NULL); 
								isSndclrSet[prev_sound] = NOTYET;

								putRect(palette, i, CHOSEN, NULL);
								isSndclrSet[i] = CHOSEN;

								prev_sound = i;
							}
						}
						else if (isSndclrSet[i] == CHOSEN) {
							putRect(palette, i, NOTYET, NULL);
							isSndclrSet[i] = NOTYET;

							prev_sound = -1;
						}

						start = clock();
						if (!g_scSound->PlayWaveFile(&g_dsb8Sample1, PITCH * (i+N*mode), DSBVOLUME_MAX)) {
							std::cout << "Err: PlayWaveFile" << endl;
							return -1;
						}
						break;
					}
				}

				break;
			}
			options.drawing_dot = -1;
		}

		int tmp = 1;

		for (i = 0; i < N; i++) {
			tmp *= (isClrsndSet[i] == SET);
		}
		if (tmp) {
			putText(palette, "START", Point(350, 380), 3, 3, Scalar(0, 0, 0), 1);
			putText(palette, "Press Enter key", Point(400, 410), 5, 1, Scalar(0, 0, 0), 1);
			waitForEnter = 1;
		}
		else {
			cv::circle(palette, Point(500, 330), 150, Scalar(255, 255, 255), -1);
			waitForEnter = 0;
		}

		imshow("Palette", palette);

		key = waitKey(5);
		if (key == 27) break;
		else if (key == 13 && waitForEnter) {
			flagNext = 1;
			break;
		}
	}

	if (!g_scSound->StopWaveFile(&g_dsb8Sample1)) {
		std::cout << "Err: StopWaveFile" << endl;
		return 0;
	}

	if (!flagNext) return -1;
	
	cv::destroyWindow("Palette");

	vector<int> rec = getEdgeRecord(detected_edges);

	int size = rec.size();
	int flag_stop = 0;
	int n_score = 0, n_score2 = 0;
	int tmp;
	int y2, x2;
	
	i = 0, j = size - 1;

	clock_t now_score, start_score;
	clock_t now_score2, start_score2;

	int time_limit = 1500, time_limit2 = 1500;
	bool play2;

	Mat src2 = Mat::zeros(Size(COL_S,ROW_S),CV_8UC3);

	start_score = clock();
	start_score2 = clock();
	
	while (waitKey(20) != 27) {
		//cv::imshow("Window3", detected_edges);
		if (i >= j) flag_stop = 1;

		if ((double)(clock() - start_score) >= 1500) {
			play = 0;
		
			if (!g_scSound->StopWaveFile(&g_dsb8Sample1)) {
				std::cout << "Err: StopWaveFile" << endl;
				return 0;
			}
		}

		if ((double)(clock() - start_score2) >= 1500) {
			play2 = 0;

			if (!g_scSound->StopWaveFile(&g_dsb8Sample2)) {
				std::cout << "Err: StopWaveFile" << endl;
				return 0;
			}
		}

		if (!flag_stop) {
			now_score = clock();
			if ((double)(now_score - start_score) >= time_limit)
			{

				tmp = rand() % 5;
				switch (tmp) {
				case 0: 
				case 1: time_limit = 3000; break;
				case 2: 
				case 3: 
				default: time_limit = 1500; break;
				}
				
				tmp = rand() % 34;
				
				if (tmp < 20) {
					tmp /= 5;
					switch (tmp) {
					case 0: n_score = 0; break;
					case 1: n_score = 4; break;
					case 2: n_score = 5; break;
					case 3: n_score = 7; break; 
					}
				}
				else if (tmp < 29) {
					tmp -= 20;
					tmp /= 3;
					switch (tmp) {
					case 0: n_score = 2; break;
					case 1: n_score = 9; break;
					case 2: n_score = 11; break;
					}
				}
				else {
					tmp -= 29;
					switch (tmp) {
					case 0: n_score = 1; break;
					case 1: n_score = 3; break;
					case 2: n_score = 6; break;
					case 3: n_score = 8; break;
					default: n_score = 10; break;
					}
				}

				if (!g_scSound->PlayWaveFile(&g_dsb8Sample1, PITCH * (n_score + N*mode) , DSBVOLUME_MAX)) {
					std::cout << "Err: PlayWaveFile" << endl;
					return -1;
				}

				play = 1;

				start_score = now_score;
			}

			now_score2 = clock();
			if ((double)(now_score2 - start_score2) >= time_limit2)
			{
				tmp = rand() % 5;
				switch (tmp) {
				case 0: time_limit2 = 3000; break;
				case 1: 
				case 2:
				case 3:
				default: time_limit2 = 1500; break;
				}

				switch (n_score) {
				case 0: n_score2 = 4; break;
				case 1: n_score2 = 5; break;
				case 2: n_score2 = 6; break;
				case 3: n_score2 = 7; break;
				case 4: n_score2 = 0; break;
				case 5: n_score2 = 2; break;
				case 6: n_score2 = 2; break;
				case 7: n_score2 = 4; break;
				case 8: n_score2 = 2; break;
				case 9: n_score2 = 5; break;
				case 10: n_score2 = 5; break;
				case 11: n_score2 = 7;
				}
				

				if (!g_scSound->PlayWaveFile(&g_dsb8Sample2, PITCH * (n_score2+N*mode), DSBVOLUME_MAX)) {
					std::cout << "Err: PlayWaveFile" << endl;
					return -1;
				}

				play2 = 1;

				start_score2 = now_score2;
			}

			y = rec[i++];
			x = rec[i++];
			x2 = rec[j--];
			y2 = rec[j--];

			int m, n, is_in_area;

			for (m = 0; m < 3; m++) {
				for (n = 0; n < 3; n++) {
					is_in_area = (x + n) >= 0 && (y+m) >= 0 && (x + n) < COL_S && (y+m) < ROW_S;

					if (is_in_area) {
						src2.at<Vec3b>(y + m, x+n)[0] = rgb[sndclrMat[n_score]][2];
						src2.at<Vec3b>(y + m, x+n)[1] = rgb[sndclrMat[n_score]][1];
						src2.at<Vec3b>(y + m, x+n)[2] = rgb[sndclrMat[n_score]][0];
					}
				}
			}
			for (m = 0; m < 3; m++) {
				for (n = 0; n < 3; n++) {
					is_in_area = (x2 - n) >= 0 && (y2 - m) >= 0 && (x2 - n) < COL_S && (y2 - m) < ROW_S;

					if (is_in_area) {
						src2.at<Vec3b>(y2 - m, x2 - n)[0] = rgb[sndclrMat[n_score2]][2];
						src2.at<Vec3b>(y2 - m, x2 - n)[1] = rgb[sndclrMat[n_score2]][1];
						src2.at<Vec3b>(y2 - m, x2 - n)[2] = rgb[sndclrMat[n_score2]][0];
					}
				}
			}
		}

		cv::imshow("Score", src2);
	}

	g_scSound->ShutdownWaveFile(&g_dsb8Sample1);
	g_scSound->ShutdownWaveFile(&g_dsb8Sample2);
	g_scSound->Shutdown();

	return 0;
}