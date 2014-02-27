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
		std::vector<cv::Point2f> newPoints;
		cv::Mat roi;
		std::vector<std::vector<cv::Point2f>> groupedPoints;
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
		cv::BackgroundSubtractorMOG2 subtractor(60, 3, false);
		
		vid.open("test2.avi");

		std::cout << "Learning background...\n";
		//learn the background
		for(int i = 0; i < 120; i ++)
		{
			vid.read(currFrame);
			cv::cvtColor(currFrame, currFrame, CV_RGB2GRAY);	
			cv::blur(currFrame, currFrame, cv::Size(9,9));
			subtractor.operator()(currFrame, mask,0.1);
		}
		std::cout << "Finished background learning, init first frame...\n";

		//do the first frame
		vid.read(prevFrame);

		cv::cvtColor(prevFrame, prevFrame, CV_RGB2GRAY);	
		cv::blur(prevFrame, prevFrame, cv::Size(9,9));
		cv::goodFeaturesToTrack(prevFrame, prevPoints, 500, 0.01, 3);
		cv::cornerSubPix(prevFrame, prevPoints, winSize, zeroZone, termcrit);
	
		while(vid.read(currFrame) == true)
		{
			temp = currFrame.clone();
			
			cv::cvtColor(temp, temp, CV_RGB2GRAY);	
			cv::blur(temp, temp, cv::Size(9,9));
			

			//create optical flow pyramid
			cv::calcOpticalFlowPyrLK(prevFrame, temp, prevPoints, currPoints, status, err, winSize, 3, termcrit);
			prevFrame = temp.clone();
			
			//subtract background to get objects
			subtractor.operator()(temp, mask, 0);
			cv::erode(mask,mask, cv::Mat());
			cv::dilate(mask, mask, cv::Mat());
			cv::imshow("Background Mask", mask);

			
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
						if(amount > 1)
						{
							goodContours.push_back(contours[i]);
							amount = 0;
							j = currPoints.size();
						}
					}
				}
			}
			
			for(size_t i=0; i< currPoints.size(); i++)
			{	
				if(status[i] !=0 )
			{	circle(currFrame, currPoints[i], 3, cv::Scalar(0,255,0));}
			}

			//draw rectangles and find new points
			for(size_t i = 0; i < goodContours.size(); i++)
			{
				boundRect = cv::boundingRect(goodContours[i]);
			
				//create mask from rectangle and search for points
				cv::Mat featureMask = cv::Mat::zeros(temp.size(), CV_8U);
				//TODO: CATCH AND FIND EXCEPTION
				//TODO: DELETE OLD POINTS
				try
				{
					roi = cv::Mat(featureMask, cv::Rect(boundRect.x, boundRect.y, boundRect.width + 10, boundRect.height + 10));
				}
				catch(int exception) //near edge
				{
					roi = cv::Mat(featureMask, cv::Rect(boundRect.x, boundRect.y, boundRect.width, boundRect.height));
				}

				roi = cv::Scalar(255, 255, 255);
				cv::goodFeaturesToTrack(temp, newPoints, 3, 0.01, 3, featureMask);

				for(size_t j = 0; j < newPoints.size(); j++)
				{
					currPoints.push_back(newPoints[j]);
				}

				newPoints.clear();
				cv::rectangle(currFrame, boundRect.tl(), boundRect.br(),  cv::Scalar(255,0,0), 2);	
			}

			goodContours.clear();
			prevPoints = currPoints;
			cv::imshow("Result", currFrame);
			cv::waitKey(1);
		}	
	}
}
