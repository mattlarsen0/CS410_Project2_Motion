#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>
#include <Windows.h>

void detect(std::string filename)
{
	char exitKey = '0';
	cv::VideoCapture vid;
	cv::Mat prevFrame;
	cv::Mat currFrame;
	std::vector<std::vector<cv::Point>> contours;
	cv::Scalar color = cv::Scalar(0,255,0);
	cv::Mat mask; //Mask results from MOG2 subtraction
	cv::Mat temp; //Temp image for manipulation of current frame
	cv::Rect boundRect;
	
	if(!vid.open(filename))
	{
		std::cout << "\nERROR. Failed to load video file." << std::endl;
		std::cout << "Make sure video codecs are installed." << std::endl;
		std::cout << "Hit Enter to exit.";
		std::cin.ignore();
		return;
	}

	FreeConsole();

	//prep first frame
	vid.read(prevFrame);
	cv::cvtColor(prevFrame, prevFrame, CV_RGB2GRAY);	
	cv::blur(prevFrame, prevFrame, cv::Size(9,9));

	while(vid.read(currFrame) == true)
	{
		temp = currFrame.clone();
		cv::cvtColor(temp, temp, CV_RGB2GRAY);	
		cv::blur(temp, temp, cv::Size(9,9));
		
		//take difference between frames to see motion
		cv::absdiff(temp, prevFrame, mask);
		prevFrame = temp.clone();

		//make motion visible
		cv::threshold(mask, mask, 2, 255, CV_THRESH_BINARY);

		//cleanup edges
		cv::erode(mask, mask, cv::Mat());
		cv::dilate(mask, mask, cv::Mat());

		cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		//Draw rectangles
		for(size_t i = 0; i < contours.size(); i++)
		{
			if(cv::contourArea(contours[i]) > 80)
			{
				boundRect = cv::boundingRect(contours[i]);
				rectangle(currFrame, boundRect.tl(), boundRect.br(), color, 2);	
			}
		}


		cv::imshow("Background Subtraction", mask);
		cv::imshow("Result", currFrame);
		exitKey = cv::waitKey(1);
		if(exitKey == 'q' || exitKey == 'Q')
		{
			return;
		}
	}
}