#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
void detect();
void track();

int main()
{
	int answer =2;
	//std::cout << "1 for motion detection, 2 for motion tracking" << std::endl;
//	std::cin >> answer;

	if(answer != 1 && answer != 2)
		answer = 2;

	if(answer == 1)
	{
		detect();
	}
	else if(answer == 2)
	{
		cv::VideoCapture vid;
		std::vector<cv::Point2f> prevPoints;
		std::vector<cv::Point2f> currPoints;
		std::vector<cv::Point2f> searchPoints;
		cv::Mat background;
		std::vector<cv::Point2f> topPoints;
		std::vector<cv::Point2f> bottomPoints;
		std::vector<cv::Point2f> leftPoints;
		cv::Rect boundRect;
		std::vector<cv::Point2f> rightPoints;
		cv::Mat mask;
		cv::Mat prevMask;
		cv::Mat currFrame;
		cv::Mat prevFrame;
		cv::Mat temp;
		std::vector<std::vector<cv::Point>> contours;
		std::vector<std::vector<cv::Point>> goodContours;
		cv::Mat searchArea;
		cv::Size winSize(10,10);
		cv::Size zeroZone(-1,-1);
		std::vector<uchar> status;
	    std::vector<float> err;
		
		cv::TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
		cv::BackgroundSubtractorMOG2 subtractor(50, 12, false);
		
		vid.open("test2.avi");


		//do the first frame
		vid.read(temp);

		cv::blur(temp, temp, cv::Size(9,9));
		subtractor.operator()(temp, mask);

		vid.read(prevFrame);

		cv::cvtColor(prevFrame, prevFrame, CV_RGB2GRAY);	
		cv::blur(prevFrame, prevFrame, cv::Size(9,9));
		int frame=120;
		
		while(vid.read(currFrame) == true)
		{
			temp = currFrame.clone();

			cv::cvtColor(temp, temp, CV_RGB2GRAY);	
			cv::blur(temp, temp, cv::Size(9,9));

			if(frame == 120)
			{
				cv::goodFeaturesToTrack(prevFrame, prevPoints, 1000, 0.01, 3);

				cv::cornerSubPix(prevFrame, prevPoints, winSize, zeroZone, termcrit);
				frame = 0;
			}

			frame++;

			//create optical flow pyramid
			cv::calcOpticalFlowPyrLK(prevFrame, temp, prevPoints, currPoints, status, err, winSize, 3, termcrit);

			prevFrame = temp.clone();
			cv::Mat kernel = cv::Mat::ones(3,3, CV_32F);

			//find and draw rectangles around movement
			subtractor.operator()(temp, mask, 0.01);
			cv::dilate(mask, mask, kernel);
		//	cv::erode(mask,mask, kernel);
			
			
			cv::imshow("asdfasddddddf", mask);
			
			cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
			
			int amount = 0;
			//find points within a countour
			for(size_t i = 0; i < contours.size(); i++)
			{
				for(size_t j = 0; j < currPoints.size(); j++)
				{
					if(cv::pointPolygonTest(contours[i], currPoints[j], false) >= 0)
					{
						amount++;
						if(amount > 2)
						{
							goodContours.push_back(contours[i]);
							amount = 0;
							j = currPoints.size();
						}
					}
				}
			}
			
			
			//draw rectangles
			for(size_t i = 0; i < goodContours.size(); i++)
			{
				cv::Moments moment = moments((cv::Mat)goodContours[i]);
				double area = moment.m00;

					//std::vector<std::vector<cv::Point> > contours_poly( contours.size() );
					//cv::approxPolyDP(contours[i], contours_poly[i], 3, true);
					boundRect = cv::boundingRect(goodContours[i]);
					cv::rectangle(currFrame, boundRect.tl(), boundRect.br(),  cv::Scalar(255,0,0), 2);	
				
			}
			
				
			for(size_t i=0; i< currPoints.size(); i++)
				if(status[i] !=0 )
				//	circle(currFrame, currPoints[i], 3, cv::Scalar(0,255,0));


			//TODO: add detection for non moving people
			//TODO: add detection of features only around current objects
			
			prevPoints = currPoints;
			cv::imshow("Result", currFrame);
			cv::waitKey(1);
		}	
	}
}
