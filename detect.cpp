#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/video/video.hpp>
#include <Windows.h>

void detect(std::string filename)
{
	cv::VideoCapture vid;
	cv::Mat prevFrame;
	cv::Mat currFrame;
	std::vector<std::vector<cv::Point>> contours;
	cv::BackgroundSubtractorMOG2 subtractor(0, 12, false);
	cv::Scalar color = cv::Scalar(0,255,0);
	cv::Mat mask; //Mask results from MOG2 subtraction
	cv::Mat temp; //Temp image for manipulation of current frame
	cv::Rect boundRect;
	
	if(!vid.open(filename))
	{
		return;
	}

	FreeConsole();

	while(vid.read(currFrame) == true)
	{
		temp = currFrame.clone();
		cv::blur(temp, temp, cv::Size(9,9));

		subtractor.operator()(temp, mask);
		cv::imshow("Background Subtraction", mask);
		cv::erode(mask,mask, cv::Mat());
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

		cv::imshow("Result", currFrame);
		cv::waitKey(1);
	}
}