#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

int main()
{
	int answer;
	std::cout << "1 for motion detection, 2 for motion tracking" << std::endl;
	std::cin >> answer;

	if(answer != 1 && answer != 2)
		answer = 2;


	cv::VideoCapture vid;
	cv::Mat prevFrame;
	cv::Mat currFrame;
	std::vector<std::vector<cv::Point>> contours;
	cv::BackgroundSubtractorMOG2 subtractor(0, 12, false);
	cv::Scalar color = cv::Scalar(0,255,0);
	cv::Mat mask; //Mask results from MOG2 subtraction
	cv::Mat temp; //Temp image for manipulation of current frame
	
	vid.open("test.avi");
	
	if(answer == 1)
	{	
		while(vid.read(currFrame) == true)
		{
			temp = currFrame.clone();
			cv::blur(temp, temp, cv::Size(9,9));
			cv::threshold(temp, temp, 180, 255, CV_THRESH_TRUNC);

			cv::Rect boundRect;

			subtractor.operator()(temp, mask);
				
			cv::imshow("Background Subtraction", mask);
			//motion analysis and tracking, template

			cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

			//Draw rectangles
			for(size_t i = 0; i < contours.size(); i++)
			{
				cv::Moments moment = moments((cv::Mat)contours[i]);
				double area = moment.m00;

				if((area > 80))
				{
					//std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
					//cv::approxPolyDP(contours[i], contours_poly[i], 3, true);
					boundRect = cv::boundingRect(contours[i]);
					rectangle(currFrame, boundRect.tl(), boundRect.br(), color, 2);	
				}
			}

			cv::imshow("Result", currFrame);
			cv::waitKey(1);
		}
	}
	else if(answer == 2)
	{
		//Init first frame
		//select features in first frame
		cv::Mat prevDescriptors;
		cv::Mat currDescriptors;
		std::vector<cv::Point2f> prevPoints;
		std::vector<cv::Point2f> currPoints;
		std::vector<cv::KeyPoint, std::allocator <cv::KeyPoint>> prevKey;
		std::vector<cv::KeyPoint, std::allocator <cv::KeyPoint>> currKey;
		cv::Mat currGray;
		cv::Mat prevGray;
		cv::Size winSize(10,10);
		cv::Size zeroZone(-1,-1);
		std::vector<uchar> status;
	    std::vector<float> err;
		
		cv::TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);

		vid.read(prevFrame);
		cv::cvtColor(prevFrame, prevGray, CV_BGR2GRAY);

		//detect features
		cv::goodFeaturesToTrack(prevGray, prevPoints, 900, 0.01, 5);

		//refine corners
		cv::cornerSubPix(prevGray, prevPoints, winSize, zeroZone, termcrit);

		prevGray.copyTo(currGray);
		

		cv::calcOpticalFlowPyrLK(prevGray, currGray, prevPoints, currPoints, status, err, winSize, 3, termcrit);


		
		while(vid.read(currFrame))
		{
			cv::cvtColor(currFrame, currGray, CV_BGR2GRAY);
			
			//create optical flow pyramid
			calcOpticalFlowPyrLK(prevGray, currGray, prevPoints, currPoints, status, err, winSize, 3, termcrit);

			
			//	computer similarity with corresponding feature in the previous frame or in the first frame
			//find more features to track
			//draw bounding rects
			for(int i=0; i< currPoints.size(); i++)
				circle(currFrame, currPoints[i], 3, cv::Scalar(0,255,0));

			prevFrame = currFrame.clone();
			prevGray = currGray.clone();
			prevPoints = currPoints;

			cv::imshow("Result", currFrame);
			cv::waitKey(1);
		}
	}
}


