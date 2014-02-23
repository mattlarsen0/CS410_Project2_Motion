#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/video/video.hpp>

void detect()
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
	
	vid.open("test2.avi");

	while(vid.read(currFrame) == true)
	{
		temp = currFrame.clone();
		cv::blur(temp, temp, cv::Size(9,9));

		subtractor.operator()(temp, mask);
		
		cv::erode(mask,mask, cv::Mat());
		cv::dilate(mask, mask, cv::Mat());
		
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