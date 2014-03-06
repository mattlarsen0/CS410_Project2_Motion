#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/video.hpp>
#include <FL/Fl_Browser.H>
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <iostream>

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
void detect(std::string filename);
void track(std::string filename);


int main()
{
	std::string answer;
	std::string filename;

	while(answer != "1" && answer != "2")
	{
		std::cout << "Enter 1 for Motion Detection, 2 for Motion Tracking" << std::endl;
		std::cin >> answer;
		std::cin.ignore();
	}

	std::cout << "Select a video file" << std::endl;
	
	Fl_Native_File_Chooser browser;
	browser.title("Select a Video File");
	browser.type(Fl_Native_File_Chooser::BROWSE_FILE);
	
	switch(browser.show())
	{
		case -1: exit(0); //error
		case 1:	exit(0); //cancel
		default: break; //chosen
	}

	std::cout << "Hit Enter to Start, Hit Q to Exit after starting\n";
	std::cin.ignore();
	
	if(answer == "1")
	{
		detect(browser.filename());
	}
	else if(answer == "2")
	{
		track(browser.filename());
	}
}
		
