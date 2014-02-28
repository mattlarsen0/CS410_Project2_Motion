#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/video/video.hpp>
#include <iostream>

//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
void detect(std::string filename);
void track(std::string filename);

int main()
{
	int answer = 0;
	std::string filename;
	while(answer != 1 && answer != 2)
	{
		std::cout << "Enter 1 for Motion Detection, 2 for Motion Tracking\n" << std::endl;
		std::cin >> answer;
	}

	std::cout << "Enter filename of the video:\n";
	std::cin >> filename;
	
	if(answer == 1)
	{
		detect(filename);
	}
	else if(answer == 2)
	{
		track(filename);
	}
}
		
