#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <vector>
#include <string>
#include <cmath>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <time.h>
#include <iostream>
#include <fstream>
#include <typeinfo>

using namespace cv;
using namespace std;
void ImageShrink2(Mat &src_image, Mat& dst_image) {
	resize(src_image, dst_image, Size(src_image.cols / 2, src_image.rows / 2));
	return;
}


int main()
{
	string input_datapath,output_datapath;
	double ***output;
	cout << "Please input the hdr image: ";
	cin >> input_datapath;
	cout << "Please input the output image: ";
	cin >> output_datapath;
	
	Mat img = imread(input_datapath, CV_LOAD_IMAGE_COLOR);
	cout <<"Now reading file... "<< input_datapath << endl;

	int rows = img.rows;
	int cols = img.cols;
	double delta = 0.001;
	Mat outputImg = Mat::zeros(rows, cols, CV_8UC3);

	int N = rows*cols;
	double maxVal, minVal;
	Mat channels[3];
	split(img, channels);
	merge(channels, 3, img);

	output = new double**[rows];
	for (int i = 0; i < rows; i++) {
		output[i] = new double*[cols];
		for (int j = 0; j < cols; ++j)
			output[i][j] = new double[3];
	}


	for (int k = 0; k < 3; k++) {
		minMaxIdx(channels[k], &minVal, &maxVal);
		double Lwhite = maxVal;
		double sum = 0;
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				sum += log((double)img.at<Vec3b>(i, j).val[k] + delta);
			}
		}
		double L_w = exp((1 / N)*sum);
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				output[i][j][k] = 0.18*(double)img.at<Vec3b>(i, j).val[k] / L_w;
			}
		}

		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				double num = output[i][j][k] * (1 + output[i][j][k] / (Lwhite*Lwhite));
				double denum = 1 + output[i][j][k];
				outputImg.at<Vec3b>(i, j).val[k] = (unsigned char)(255 * num / denum);
			}
		}
	}
	cout << "Now saving file... "<<output_datapath << endl;
	imwrite(output_datapath, outputImg);
	cout << "Finish saving!!!" << endl;
	system("pause");
	return 0;

}