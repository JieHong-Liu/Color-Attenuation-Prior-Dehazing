#include <iostream>
#include <cmath>
#include <random>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "guidedfilter.h"

#define CV_SORT_EVERY_ROW		0
#define CV_SORT_ASCENDING		0
#define CV_SORT_EVERY_COLUMN	1
#define CV_SORT_DESCENDING		16

typedef struct pos {
	int x;
	int y;
};
typedef struct atom {
	double b;
	double g;
	double r;
};
using namespace std;
using namespace cv;

Mat buildDepthMap(Mat& src); // 
Mat buildMediumTransmission(Mat& dMap); // build T
atom buildAtmoLight(Mat& src); // build A
Mat recoverOrigin(Mat& src,atom& A, Mat& t);
Mat doSubstraction(Mat& src, atom& A);

void demoSort()
{
	int testArrLen = 2;
	Mat_<int> testArr = cv::Mat::zeros(3, testArrLen, CV_64F);
	testArr(0, 0) = 100;
	testArr(0, 1) = 3;
	testArr(1, 0) = 15;
	testArr(1, 1) = 87;
	testArr(2, 0) = 33;
	testArr(2, 1) = 55;
	Mat_<int> sortArr,sortIdxArr;
	cv::sortIdx(testArr, sortIdxArr, CV_SORT_EVERY_ROW + CV_SORT_DESCENDING);
	cv::sort(testArr, sortArr, CV_SORT_EVERY_ROW + CV_SORT_DESCENDING);
	cout << "testArr = \n" << testArr << endl;
	cout << "sortArr = \n" << sortArr << endl;
	cout << "sortIdx = \n" << sortIdxArr << endl;
}


int main()
{
	Mat img = imread("C:/Users/jieho/OneDrive - 國立臺灣科技大學/meeting/20220524/forest1.jpg",CV_64FC3);
	Mat tmpSrc; 
	img.convertTo(tmpSrc, CV_64FC3); cout << "tmp.type: " << tmpSrc.type() << endl;
#if 1
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < img.rows; j++)
		{
			for (int k = 0; k < img.cols; k++)
			{
				tmpSrc.at<Vec3d>(j, k)[i] = img.at<Vec3b>(j, k)[i] / 255.0;
			}
		}
	}
#endif 
	Mat dMap = buildDepthMap(img);
	Mat mediumTransmission = buildMediumTransmission(dMap);
	atom A = buildAtmoLight(tmpSrc);
	cout << "A = " << A.b << " " << A.g << " " << A.r << endl;
	Mat dst = recoverOrigin(tmpSrc,A,mediumTransmission);
	imshow("src",img);
	imshow("depth-map.jpg", dMap);
	imshow("medium-transmission", mediumTransmission);
	imshow("dst", dMap);

	waitKey(0);
	return 0;
}

Mat buildDepthMap(Mat& src)
{
	Mat hsv;

	cvtColor(src,hsv,COLOR_BGR2HSV); // hsv result
	vector<Mat> channels;
	split(hsv, channels);
	Mat H = channels.at(0);
	Mat S = channels.at(1);
	Mat V = channels.at(2);
	Mat dst(H.size(), CV_64FC1);
	cout << dst.size() << endl;
	double theta0(0.121779), theta1(0.959710), theta2(-0.780245),sigma(0.041337);
	
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(0, sigma*sigma);

	for (int i = 0; i < hsv.rows; i++)
	{
		for (int j = 0; j < hsv.cols; j++)
		{
			double epsilon = distribution(generator);
			double depth = theta0 + theta1 * V.at<uchar>(i,j) + theta2 * S.at<uchar>(i,j) + epsilon;
			//cout << depth / 255.0 << endl;
			dst.at<double>(i, j) = depth / 255.0;
		}
	} 
	Mat erosionDst,guideDst;
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(5, 5));
	erode(dst, erosionDst, erodeElement);
	erosionDst.copyTo(guideDst);


	int r = 4; // try r=2, 4, or 8
	double eps = 0.2 * 0.2; // try eps=0.1^2, 0.2^2, 0.4^2

	eps *= 255 * 255;   // Because the intensity range of our images is [0, 255]

	cv::Mat q = guidedFilter(erosionDst, erosionDst, r, eps);
	
	return q;

}

Mat buildMediumTransmission(Mat& dMap) // make t(x)
{
	Mat dst;
	dMap.copyTo(dst);
	double beta = 1.0;
	cout << "dMap.rows: " << dMap.rows << endl;
	for (int i = 0; i < dMap.rows; i++)
	{
		for (int j = 0; j < dMap.cols; j++)
		{
			dst.at<double>(i, j) = exp(-1 * beta * dMap.at<double>(i, j));
		}
	}
	return dst;

}

atom buildAtmoLight(Mat& src) // build A
{
	Mat tmp(src.size(), CV_64FC3);
	src.copyTo(tmp);
	vector<Mat> channels;
	split(tmp, channels);
	Mat B = channels.at(0);
	Mat G = channels.at(1);
	Mat R = channels.at(2);

	// sort total Mat.
	Mat_<int> sortIdx;

	vector<pos> brightestArray[3];


	for (int i = 0; i < 3; i++) // find the maximum value of each row
	{
		cv::sortIdx(channels.at(i), sortIdx, CV_SORT_EVERY_ROW + CV_SORT_DESCENDING);  
		for (int j = 0; j < sortIdx.rows; j++)
		{
			for (int k = 0; k < sortIdx.cols; k++)
			{
				if (sortIdx(j, k) == 0)
				{
					pos brightest_pos; brightest_pos.x = j; brightest_pos.y = k;
					brightestArray[i].push_back(brightest_pos);
					break;
				}
			}
		}
	}

	atom at;
	at.b = 0; at.g = 0; at.r = 0;	// initialize the max_channels.


	

	for (int i = 0; i < 3; i++)
	{
		cout << "channel :" << i << endl;
		cout << "brightest size: " << brightestArray[i].size() << endl;

		for (int j = 0; j < brightestArray[i].size(); j++)
		{
#if 1

			if (i == 0)
			{
				if (channels.at(i).at<double>(brightestArray[i][j].x, brightestArray[i][j].y) > at.b)
				{
					at.b = channels.at(i).at<double>(brightestArray[i][j].x, brightestArray[i][j].y);
				}
			}
			else if (i == 1)
			{
				if (channels.at(i).at<double>(brightestArray[i][j].x, brightestArray[i][j].y) > at.g)
				{
					at.g = channels.at(i).at<double>(brightestArray[i][j].x, brightestArray[i][j].y);
				}
			}
			else if (i == 2)
			{
				if (channels.at(i).at<double>(brightestArray[i][j].x, brightestArray[i][j].y) > at.r)
				{
					at.r = channels.at(i).at<double>(brightestArray[i][j].x, brightestArray[i][j].y);
				}
			}
#endif
		}
		cout << endl << endl;
	}


	return at;
}

Mat recoverOrigin(Mat& src, atom& A, Mat& t)
{
	Mat dst(src.size(), CV_64FC3);

	Mat son = doSubstraction(src, A);
	imshow("son", son);
#if 1
	for (int ch = 0; ch < 3; ch++)
	{
		for (int i = 0; i < src.rows; i++)
		{
			for (int j = 0; j < src.cols; j++)
			{
				double mother = min(max(t.at<double>(i, j), 0.1), 0.9);
				if (ch == 0)
				{
					dst.at<Vec3d>(i,j)[0] = son.at<Vec3d>(i, j)[0] / mother + A.b;
				}
				else if (ch == 1)
				{
					dst.at<Vec3d>(i, j)[1] = son.at<Vec3d>(i, j)[1] / mother + A.g;
				}
				else
				{
					dst.at<Vec3d>(i, j)[2] = son.at<Vec3d>(i, j)[2] / mother + A.r;
				}
			}
		}
	}
#endif
	return dst;
}

Mat doSubstraction(Mat& src, atom& A)
{
	Mat dst(src.size(), CV_64FC3);

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < src.rows; j++)
		{
			for (int k = 0; k < src.cols; k++)
			{
//				cout << "src:" << src.at<Vec3b>(j, k)[i] << endl;
				if (i == 0)
				{
					dst.at<Vec3d>(j,k)[i] = src.at<Vec3d>(j, k)[i] - A.b;
				}
				else if (i==1)
				{
					dst.at<Vec3d>(j, k)[i] = src.at<Vec3d>(j, k)[i] - A.g;

				}
				else
				{
					dst.at<Vec3d>(j, k)[i] = src.at<Vec3d>(j, k)[i] - A.r;
				}
			}
		}
	}
	return dst;

}