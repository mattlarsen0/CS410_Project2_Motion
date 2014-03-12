#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/video.hpp>
#include <Windows.h>
#include <iostream>

void track(std::string filename)
{
	cv::VideoCapture vid;

	char exitKey = '0';
	int frameCount = 0;
	int amount = 0;
	int noObjectFrames = 0;

	std::vector<uchar> status; //status of points from lucas kanade, set to 1 if feature was tracked
	std::vector<float> err;
	std::vector<cv::Point2f> prevPoints; //points for the previous frame
	std::vector<cv::Point2f> currPoints; //points for the current frame
	std::vector<cv::Point2f> newPoints; //points found around tracked objects
	std::vector<std::vector<cv::Point>> contours;
	std::vector<std::vector<cv::Point>> goodContours; //contors that have tracked points inside
	std::vector<int> pointCount; //amount of points inside a contour

	cv::Size winSize(10,10);
	cv::Size zeroZone(-1,-1);
		
	cv::Mat roi; //region of interest, for masking
	cv::Mat currFrame; //frame to be displayed
	cv::Mat prevFrame; //previous frame, blurred
	cv::Mat temp; //copy of currFrame with operations applied to it
	cv::Mat mask; //result of background subtraction
		
	cv::Rect boundRect;

	cv::TermCriteria termcrit(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS,20,0.03);
	cv::BackgroundSubtractorMOG2 subtractor(60, 3, false);
		
	if(!vid.open(filename))
	{
		std::cout << "\nERROR. Failed to load video file." << std::endl;
		std::cout << "Make sure video codecs are installed." << std::endl;
		std::cout << "Hit Enter to exit.";
		std::cin.ignore();
		return;
	}

	std::cout << "Learning background...\n";

	//learn the background
	for(int i = 0; i < 120; i ++)
	{
		vid.read(currFrame);
		cv::cvtColor(currFrame, currFrame, CV_RGB2GRAY);	
		cv::blur(currFrame, currFrame, cv::Size(9,9));
		subtractor.operator()(currFrame, mask,0.01);
	}
	std::cout << "Finished background learning, init first frame...\n";

	//do the first frame
	vid.read(prevFrame);

	cv::cvtColor(prevFrame, prevFrame, CV_RGB2GRAY);	
	cv::blur(prevFrame, prevFrame, cv::Size(9,9));

	//detect features 
	std::cout << "Detecting features...\n";
	cv::goodFeaturesToTrack(prevFrame, prevPoints, 500, 0.01, 3);
	
	FreeConsole();

	//Main loop
	while(vid.read(currFrame))
	{
		temp = currFrame.clone();
			
		cv::cvtColor(temp, temp, CV_RGB2GRAY);	
		cv::blur(temp, temp, cv::Size(9,9));

		//create optical flow pyramid
		cv::calcOpticalFlowPyrLK(prevFrame, temp, prevPoints, currPoints, status, err,
								 winSize, 3, termcrit, 0, 0.001);

		//remove untracked points
		for(size_t i = 0; i < currPoints.size(); i++)
		{
			if(status[i] == 0)
			{
				currPoints.erase(currPoints.begin() + i);
			}
		}
			
		//subtract background to get objects
		subtractor.operator()(temp, mask, 0);
		cv::imshow("Background Mask", mask);
		cv::erode(mask,mask, cv::Mat());
		cv::dilate(mask, mask, cv::Mat());
					
		cv::findContours(mask, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

		//find points within a countour
		for(size_t i = 0; i < contours.size(); i++)
		{
			for(size_t j = 0; j < currPoints.size(); j++)
			{
				if(cv::pointPolygonTest(contours[i], currPoints[j], false) >= 0)
				{
					amount++;
					if(amount == 2) //when we find 2 points inside the object
					{
						goodContours.push_back(contours[i]);
					}
					
					circle(currFrame, currPoints[j], 3, cv::Scalar(0,255,0)); //mark the point
				}
			}

			if(amount >= 2)
			{
				pointCount.push_back(amount);
			}
			amount = 0;
		}
		

		//draw rectangles and find new points
		for(size_t i = 0; i < goodContours.size(); i++)
		{
			boundRect = cv::boundingRect(goodContours[i]);

			try
			{
				if(pointCount[i] < 10)
				{
					//create mask from rectangle and search for points
					cv::Mat featureMask = cv::Mat::zeros(temp.size(), CV_8U);
				
					roi = cv::Mat(featureMask, cv::Rect(boundRect.x, boundRect.y, boundRect.width + 10,
								  boundRect.height + 10));
					roi = cv::Scalar(255, 255, 255);
				
					cv::goodFeaturesToTrack(temp, newPoints, 10, 0.01, 3, featureMask);
				}
			}
			catch(cv::Exception e) 
			{
				//don't detect points, object near edge. 
			}

				

			for(size_t j = 0; j < newPoints.size(); j++)
			{
				currPoints.push_back(newPoints[j]);
			}

			newPoints.clear();
			cv::rectangle(currFrame, boundRect.tl(), boundRect.br(),  cv::Scalar(255,0,0), 2);	
		}

		frameCount++;
		if(goodContours.size() < 1)
		{
			noObjectFrames++;
		}

		//every 3000 frames reset points or if there are no tracked objects for 5 frames
		if(noObjectFrames == 5 || frameCount == 3000)
		{
			cv::goodFeaturesToTrack(temp, currPoints, 500, 0.01, 3);
			noObjectFrames = 0;
			frameCount = 0;
		}

		//cleanup
		pointCount.clear();
		goodContours.clear();
		prevPoints = currPoints;
		prevFrame = temp.clone();
		cv::imshow("Result", currFrame);
		exitKey = cv::waitKey(1);
		if(exitKey == 'q' || exitKey == 'Q')
		{
			return;
		}
	}	
}
