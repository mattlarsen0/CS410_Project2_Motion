#include <FL/FL.h>
#include <FL/Fl_Window.h>
#include <FL/Fl_Menu_Item.h>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Box.h>
#include <opencv2/core/core.hpp>
#include <opencv2/stitching/stitcher.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/nonfree/nonfree.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main()
{
	cv::Mat prevFrame = cv::imread("first.jpg");
	cv::Mat currFrame = cv::imread("second.jpg");
	cv::Mat gray;
	cv::Size winSize(10,10);
	cv::TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);


	std::vector<cv::Point2f> prevPoints;
	std::vector<cv::Point2f> currPoints;

	std::vector<uchar> status;
	std::vector<float> err;
	
	cv::cvtColor(prevFrame, gray, CV_BGR2GRAY);
	cv::goodFeaturesToTrack(gray, prevPoints, 500, 0.01, 10);
	cv::cornerSubPix(gray, prevPoints, winSize, cv::Size(-1,-1), termcrit);	   

	cv::cvtColor(currFrame, gray, CV_BGR2GRAY);
	cv::goodFeaturesToTrack(gray, currPoints, 500, 0.01, 10);
	cv::cornerSubPix(gray, currPoints, winSize, cv::Size(-1,-1), termcrit);	   


	cv::calcOpticalFlowPyrLK(prevFrame, currFrame, prevPoints, currPoints, status, err);
	http://stackoverflow.com/questions/15729210/how-to-draw-a-rectangle-around-an-object-using-some-opencv-algorithm
	currPoints.resize(k);

	cv::imwrite("result.jpg", currFrame);
}