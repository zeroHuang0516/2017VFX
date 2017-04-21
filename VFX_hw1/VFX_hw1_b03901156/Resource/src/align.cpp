#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "iostream"
#include <vector>
#include <string>
#include <cmath>
#include <fstream>

using namespace cv;
using namespace std;

void ImageShrink2(Mat &src_image, Mat& dst_image) {
	resize(src_image, dst_image, Size(src_image.cols / 2, src_image.rows / 2));
	return;
}

void computeBitmaps(Mat &input_image, Mat& threshold_bitmap, Mat& exclusion_bitmap) {
	int rows = input_image.rows;
	int cols = input_image.cols;
	int thres = 0;
	int counter[256] = { 0 };
	threshold_bitmap = Mat::zeros(rows, cols, CV_8U);
	exclusion_bitmap = Mat::zeros(rows, cols, CV_8U);


	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			counter[(int)input_image.at<uchar>(i, j)]++;
		}
	}

	//compute threshold
	int median_count = rows*cols / 2;
	for (int i = 0; i < 256; i++) {
		if (median_count - counter[i] > 0) {
			median_count -= counter[i];
		}
		else {
			thres = i;
			break;
		}
	}
	//cout << "thres: " << thres << endl;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if ((int)input_image.at<uchar>(i, j) > thres) {
				threshold_bitmap.at<uchar>(i, j) = 1;
			}
			if (abs((int)input_image.at<uchar>(i, j) - thres) >= 0) {
				exclusion_bitmap.at<uchar>(i, j) = 1;
			}
		}
	}
	return;
}

void BitmapShift(Mat& input_image, int x_shift, int y_shift, Mat& output_image) {
	int rows = input_image.rows;
	int cols = input_image.cols;
	output_image = Mat::zeros(rows, cols, CV_8U);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (j - y_shift >= 0 && j - y_shift<cols && i - x_shift >= 0 && i - x_shift<rows) {
				output_image.at<uchar>(i, j) = input_image.at<uchar>(i - x_shift, j - y_shift);
			}
		}
	}
	return;
}

int BitmapSum(Mat img) {
	int sum = 0;
	int rows = img.rows;
	int cols = img.cols;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			sum += (int)img.at<uchar>(i, j);
		}
	}
	return sum;
}

void getExpShift(Mat& img1, Mat& img2, int shift_bits, int shift_res[2]) {
	int rows = img1.rows;
	int cols = img1.cols;
	int min_err;
	int cur_shifts[2];
	Mat thres_bitmap1, thres_bitmap2;
	Mat exclu_bitmap1, exclu_bitmap2;
	if (shift_bits > 0) {
		Mat smaller_img1, smaller_img2;
		ImageShrink2(img1, smaller_img1);
		ImageShrink2(img2, smaller_img2);
		getExpShift(smaller_img1, smaller_img2, shift_bits - 1, cur_shifts);
		cur_shifts[0] *= 2;
		cur_shifts[1] *= 2;
	}
	else {
		cur_shifts[0] = 0;
		cur_shifts[1] = 0;
	}
	computeBitmaps(img1, thres_bitmap1, exclu_bitmap1);
	computeBitmaps(img2, thres_bitmap2, exclu_bitmap2);

	min_err = cols*rows;
	for (int j = -1; j <= 1; j++) {
		for (int i = -1; i <= 1; i++) {
			int x_shift = cur_shifts[0] + i;
			int y_shift = cur_shifts[1] + j;
			Mat shifted_thres_bitmap2, shifted_exclu_bitmap2;
			Mat diff;
			int err;
			BitmapShift(thres_bitmap2, x_shift, y_shift, shifted_thres_bitmap2);
			BitmapShift(exclu_bitmap2, x_shift, y_shift, shifted_exclu_bitmap2);
			diff = thres_bitmap1 ^ shifted_thres_bitmap2;
			diff = diff & exclu_bitmap1;
			diff = diff & shifted_exclu_bitmap2;
			err = BitmapSum(diff);
			if (err < min_err) {
				shift_res[0] = x_shift;
				shift_res[1] = y_shift;
				min_err = err;
			}
		}
	}
	return;
}

void align(Mat& input_image, int x_shift, int y_shift) {
	int rows = input_image.rows;
	int cols = input_image.cols;
	Mat result(rows, cols, CV_8UC3);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			for (int k = 0; k < 3; k++) {
				if (i - x_shift >= 0 && i - x_shift < rows && j - y_shift >= 0 && j - y_shift < cols) {
					result.at<Vec3b>(i, j).val[k] = input_image.at<Vec3b>(i - x_shift, j - y_shift).val[k];
				}
				else {
					result.at<Vec3b>(i, j).val[k] = 0;
				}
			}

		}
	}
	input_image = result;
	return;
}


int main()
{
	cout << "Please input the list file: ";
	string listFileName;
	cin >> listFileName;
	ifstream infile(listFileName);
	vector<string> imageName;
	string line;
	while (getline(infile, line)) {
		imageName.push_back(line);
	}

	vector<Mat> images;
	string imgName;
	
	for (int i = 2; i < imageName.size(); i++) {
		imgName = imageName[0];
		imgName += imageName[i];
		Mat img = imread(imgName);
		images.push_back(img);
		cout << "Now reading... " << imgName << endl;
	}
	Mat img1, img2;
	int shift_bits = 6;
	int img_ref_idx = 0;
	Mat img_ref = images[img_ref_idx];
	int shift_res[2];
	cout << "Now align......" << endl;
	for (int i = 0; i < images.size(); i++) {
		cvtColor(img_ref, img1, CV_BGR2GRAY);
		cvtColor(images[i], img2, CV_BGR2GRAY);
		getExpShift(img1, img2, shift_bits, shift_res);
		//cout << "idx: " << i << " " << shift_res[0] << ", " << shift_res[1] << endl;
		align(images[i], shift_res[0], shift_res[1]);
	}
	cout << "Finish aligning......" << endl;
	
	cout << "Now save aligned images......" << endl;
	for (int i = 0; i < images.size(); i++) {
		imgName = imageName[1];
		imgName += "Aligned";
		imgName += imageName[i+2];
		imwrite(imgName, images[i]);
		cout << "Save file: " <<imgName << endl;
	}
	cout << "Finish saving......" << endl;
	system("pause");
	return 0;
}