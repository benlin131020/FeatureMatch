// FeatureMatch.cpp: 定義主控台應用程式的進入點。
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <cstring>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/videoio.hpp>
using namespace std;
using namespace cv;
using namespace cv::xfeatures2d;

#define track false
#define fmatch true
#define tmatch false
//branch1
//master branch
cv::Point2f RotatePoint(const cv::Point2f& cen_pt, const cv::Point2f& p, float rad)
{
	const cv::Point2f trans_pt = p - cen_pt;
	const float x = std::cos(rad) * trans_pt.x - std::sin(rad) * trans_pt.y;
	const float y = std::sin(rad) * trans_pt.x + std::cos(rad) * trans_pt.y;
	const cv::Point2f rot_pt(x, y);
	const cv::Point2f fin_pt = rot_pt + cen_pt;

	return fin_pt;
}

int main()
{
#if track
	// declares all required variables
	Rect2d roi;
	Mat frame;
	bool ok;
	// create a tracker object
	Ptr<Tracker> tracker = TrackerKCF::create();
	// set input video
	VideoCapture video("C:\\Users\\User\\Desktop\\david.webm");
	// get bounding box
	video >> frame;
	roi = selectROI("tracker", frame);
	//quit if ROI was not selected
	if (roi.width == 0 || roi.height == 0)
		return 0;
	// initialize the tracker
	tracker->init(frame, roi);
	// perform the tracking process
	printf("Start the tracking process, press ESC to quit.\n");
	for (;; ) {
		// get frame from the video
		video >> frame;
		// stop the program if no more images
		if (frame.rows == 0 || frame.cols == 0)
			break;
		// update the tracking result
		ok = tracker->update(frame, roi);
		// draw the tracked object
		if (ok)rectangle(frame, roi, Scalar(255, 0, 0), 2, 1);
		else {
			rectangle(frame, roi, Scalar(0, 0, 255), 2, 1);
			waitKey(0);
		} 
		// show image with the tracked object
		imshow("tracker", frame);
		//quit on ESC button
		if (waitKey(1) == 27)break;
	}
#endif
#if fmatch
	Mat img_1 = imread("C:\\Users\\User\\Desktop\\1523807140838.jpg");
	Mat img_2 = imread("C:\\Users\\User\\Desktop\\1523807206156.jpg");
	Mat img_3 = imread("C:\\Users\\User\\Desktop\\1523787983188.jpg");
	Mat img_4 = imread("C:\\Users\\User\\Desktop\\1523787952829.jpg");
	resize(img_1, img_1, Size(img_1.cols*2, img_1.rows*2));
	resize(img_2, img_2, Size(img_2.cols*2, img_2.rows*2));
	//-- Step 1: Detect the keypoints using SURF Detector, compute the descriptors
	int minHessian = 400;
	Ptr<SURF> detector = SURF::create();
	detector->setHessianThreshold(minHessian);
	std::vector<KeyPoint> keypoints_1, keypoints_2;
	Mat descriptors_1, descriptors_2;
	detector->detectAndCompute(img_1, Mat(), keypoints_1, descriptors_1);
	detector->detectAndCompute(img_2, Mat(), keypoints_2, descriptors_2);
	//-- Step 2: Matching descriptor vectors using FLANN matcher
	FlannBasedMatcher matcher;
	std::vector< DMatch > matches;
	matcher.match(descriptors_1, descriptors_2, matches);
	double max_dist = 0; double min_dist = 100;
	//-- Quick calculation of max and min distances between keypoints
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		double dist = matches[i].distance;
		if (dist < min_dist) min_dist = dist;
		if (dist > max_dist) max_dist = dist;
	}
	printf("-- Max dist : %f \n", max_dist);
	printf("-- Min dist : %f \n", min_dist);
	//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
	//-- or a small arbitrary value ( 0.02 ) in the event that min_dist is very
	//-- small)
	//-- PS.- radiusMatch can also be used here.
	std::vector< DMatch > good_matches;
	for (int i = 0; i < descriptors_1.rows; i++)
	{
		if (matches[i].distance <= max(2 * min_dist, 0.02))
		{
			good_matches.push_back(matches[i]);
		}
	}
	//-- Draw only "good" matches
	Mat img_matches;
	drawMatches(img_1, keypoints_1, img_2, keypoints_2,
		good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);
	//-- Show detected matches
	imshow("Good Matches", img_matches);
	for (int i = 0; i < (int)good_matches.size(); i++)
	{
		printf("-- Good Match [%d] Keypoint 1: %d  -- Keypoint 2: %d  \n", i, good_matches[i].queryIdx, good_matches[i].trainIdx);
	}
	////L2
	//resize(img_2, img_2, Size(img_1.cols, img_1.rows));
	//double dis1 = norm(img_1, img_2, NORM_L2);
	//printf("dis1:%lf\n", dis1);
	//resize(img_4, img_4, Size(img_3.cols, img_3.rows));
	//double dis2 = norm(img_3, img_4, NORM_L2);
	//printf("dis2:%lf\n", dis2);
	waitKey(0);
#endif
#if tmatch
	Mat roiImg = imread("C:\\Users\\User\\Desktop\\1523807140838.jpg");
	Mat src = imread("C:\\Users\\User\\Desktop\\1523807206156.jpg");
	Mat displayImg = src.clone();
	Mat result;
	Mat rotate1, rotate2;

    matchTemplate(src, roiImg, result, CV_TM_SQDIFF);
    double minVal1; 
    Point minLoc1;
    minMaxLoc(result, &minVal1, 0, &minLoc1, 0);

	Point2f pt1(src.cols / 2., src.rows / 2.);
	Mat r1 = getRotationMatrix2D(pt1, 180, 1.0);
	warpAffine(src, rotate1, r1, Size(src.cols, src.rows));
	Point2f pt2(roiImg.cols / 2., roiImg.rows / 2.);
	Mat r2 = getRotationMatrix2D(pt2, 180, 1.0);
	warpAffine(roiImg, rotate2, r2, Size(roiImg.cols, roiImg.rows));

	matchTemplate(rotate1, rotate2, result, CV_TM_SQDIFF);
	double minVal2;
	Point minLoc2;
	minMaxLoc(result, &minVal2, 0, &minLoc2, 0);

	rectangle(displayImg, minLoc1, RotatePoint(pt1, minLoc2, CV_PI), Scalar(0, 255, 0), 2);
	//rectangle(rotate1, minLoc2, Point(minLoc2.x + roiImg.cols, minLoc2.y + roiImg.rows), Scalar(0, 255, 0), 2);
    imshow("origin", src);
    imshow("roi", roiImg);
	imshow("result", result);
	imshow("display", displayImg);
    waitKey(0);

#endif
	
    return 0;
}

