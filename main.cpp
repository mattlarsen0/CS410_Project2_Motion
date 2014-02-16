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
#include <opencv2/video/background_segm.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main()
{
	cv::VideoCapture vid;
	vid.open("test.avi");
	cv::Mat prevFrame;
	cv::Mat currFrame;
	cv::Mat diffFrame;
	cv::Mat corners;
	cv::Mat status;
	cv::Mat err;
	cv::Mat gray;
	cv::Size winSize(10,10);
	std::vector< std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	std::vector<cv::Point2f> prevPoints;
	std::vector<cv::Point2f> currPoints;

	cv::TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
	cv::BackgroundSubtractorMOG2 subtractor(0, 4, false);
	cv::Mat mask;
	vid.read(prevFrame);
	cv::GaussianBlur(prevFrame, prevFrame, cv::Size(3,3), 7);
		cv::threshold(currFrame, currFrame, 150, 200, CV_THRESH_TOZERO);
	subtractor.operator()(prevFrame, mask);
	cv::namedWindow("result.jpg");
	while(vid.read(currFrame) == true)
	{
		cv::Mat temp = currFrame.clone();
		cv::GaussianBlur(temp, temp, cv::Size(3,3), 7);
		cv::threshold(temp, temp, 150, 255, CV_THRESH_BINARY);

	
		 //init first frame
	
		subtractor.operator()(temp, mask);

		cv::findContours(mask ,contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
		std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
		for(int i = 0; i < contours.size(); i++)
		{
			cv::Moments moment = moments((cv::Mat)contours[i]);
			double area = moment.m00;

			if(area > 10)
			{
				
				std::vector<cv::Rect> boundRect( contours.size() );

			//	cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 4, true );
				boundRect[i] = cv::boundingRect( cv::Mat(contours[i]) );
										
				for( int j = 0; j < contours.size(); j++ )
				{
					cv::Scalar color = cv::Scalar(0,255,0);
					rectangle( currFrame, boundRect[j].tl(), boundRect[j].br(), color, 2, 8, 0 );

				}
			}
		}

		cv::imshow("result.jpg", currFrame);
		cv::waitKey(1);
	}
	
}


