#include "stdfix.h"
#include <unistd.h>
#include </usr/local/include/opencv2/opencv.hpp>
#include </usr/local/include/opencv2/highgui/highgui.hpp>
#include </usr/local/include/opencv2/core/core.hpp>
#include </usr/local/include/opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include </usr/local/include/opencv/cv.h>
#include </usr/local/include/opencv2/features2d/features2d.hpp>
#include </usr/local/include/opencv2/calib3d/calib3d.hpp>
#include </usr/local/include/opencv2/core/core_c.h>
#include </usr/local/include/opencv2/calib3d/calib3d.hpp>
#include <cstring>
#include <time.h>
#include <limits.h>
#include <sys/time.h>

#include <time.h>
#include <iostream>
#include <stdio.h>     
#include <sys/timeb.h>


using namespace cv;
using namespace std;

void sleepcp(int milliseconds);

void sleepcp(int milliseconds) // cross-platform sleep function
{
    clock_t time_end;
    time_end = clock() + milliseconds * CLOCKS_PER_SEC/1000;
    while (clock() < time_end)
    {
    }
}

struct byXposition {

	bool operator () (const Rect & a, const Rect & b) {
		return (a.x) < (b.x);
	}
};
struct byYposition {
	bool operator () (const Rect & a, const Rect & b) {
		return (a.y) < (b.y);
	}
};
struct byHeight {
	bool operator () (const Rect & a, const Rect & b) {
		return (a.height) > (b.height);
	}
};
struct byWidth {
	bool operator () (const Rect & a, const Rect & b) {
		return (a.width) > (b.width);
	}
};

bool valueInRange(int value, int min, int max, float range)
{
	return (value >= min - range) && (value <= max + range);
}



cv::Rect  GettIntersection(cv::Rect A, cv::Rect B)
{

	int x = 0, y = 0, width = 0, height = 0;

	if (A.x > B.x)

		x = A.x;

	else

		x = B.x;

	if (A.y > B.y)

		y = A.y;

	else

		y = B.y;



	if (A.x + A.width > B.x + B.width)

		width = (B.x + B.width) - x;

	else

		width = (A.x + A.width) - x;

	if (A.y + A.height > B.y + B.height)

		height = (B.y + B.height) - y;

	else

		height = (A.y + A.height) - y;

	return Rect(x, y, width, height);

}

cv::Rect  GettUnion(cv::Rect A, cv::Rect B)
{

	int x = 0, y = 0, width = 0, height = 0;

	if (A.x < B.x)

		x = A.x;

	else

		x = B.x;

	if (A.y < B.y)

		y = A.y;

	else

		y = B.y;



	if (A.x + A.width < B.x + B.width)

		width = (B.x + B.width) - x;

	else

		width = (A.x + A.width) - x;

	if (A.y + A.height < B.y + B.height)

		height = (B.y + B.height) - y;

	else

		height = (A.y + A.height) - y;

	return Rect(x, y, width, height);

}

bool AreOverlapingSameSize(Rect A, Rect B)
{

	int BlobHeight;

	if (A.height > A.width)

		BlobHeight = A.width;

	else

		BlobHeight = A.height;

	//Checking to see the both rectangles have almose same on dimension

	bool IsSameSize = B.height > (BlobHeight - 20) * 0.5;

	float range = 0;

	bool xOverlap = valueInRange(A.x, B.x, B.x + B.width, range) || valueInRange(B.x, A.x, A.x + A.width, range);



	bool yOverlap = valueInRange(A.y, B.y, B.y + B.height, range) ||
		valueInRange(B.y, A.y, A.y + A.height, range);



	return xOverlap && yOverlap && IsSameSize;

}

bool AreOverlaping(Rect A, Rect B, Rect* rect)
{

	float range;

	//if (A.height / 2 < B.height / 2)

	// range = A.height * 0.05;

	//else

	// range = B.height * 0.05;

	range = 0;

	bool xOverlap = valueInRange(A.x, B.x, B.x + B.width, range) || valueInRange(B.x, A.x, A.x + A.width, range);

	range = 0;

	bool yOverlap = valueInRange(A.y, B.y, B.y + B.height, range) || valueInRange(B.y, A.y, A.y + A.height, range);



	if (xOverlap && yOverlap)

		*rect = GettIntersection(A, B);



	return xOverlap && yOverlap;

}



vector<Rect> removeExtraPoints(vector<Rect> bs)
{
	vector<Rect> blobs;
	for (int a = 0; a < bs.size(); a++)
		blobs.push_back(Rect(bs[a]));
	std::sort(blobs.begin(), blobs.end(), byXposition());
	for (int a = 0; a < blobs.size(); a++)
	{
		int centerPoint = (blobs[a].height / 2) + blobs[a].y;
		int factor = blobs[a].height*1.8;
		Rect rec = Rect(blobs[a].x - factor, blobs[a].y, blobs[a].width + (factor * 2), blobs[a].height);
		for (int b = a + 1; b < blobs.size(); b++)
		{
			if (b != a)
			{
				Rect temp;
				bool overlapping = AreOverlapingSameSize(rec, blobs[b]);
				if (overlapping && a < blobs.size())
				{
					int centerPointNext = (blobs[b].height / 2) + blobs[b].y;;
					int diff = 0;
					if (centerPoint > centerPointNext)
						diff = centerPoint - centerPointNext;
					else
						diff = centerPointNext - centerPoint;
					if (centerPointNext > 1 && centerPoint > 1 && (diff == 1 || diff == 0))// || diff == 2))
					{
						Rect ltemp = GettUnion(blobs[a], blobs[b]);
						if (ltemp.height > blobs[a].height*0.2 & ltemp.height < blobs[a].height * 2)
						{
							blobs[a] = ltemp;
							rec = Rect(blobs[a].x - factor, blobs[a].y, blobs[a].width + (factor * 2), blobs[a].height);
							blobs.erase(blobs.begin() + b);
							b--;
						}
					}
				}
			}
		}
	}
	return blobs;
}



int main()
{
        
int fc =10000;
	int frameSizeW = 640;
	int frameSizeH = 480;

	//cv::Rect tmpFrame = Rect(0, 0, frameSizeW, frameSizeH);
       
	Mat frame, grayFrame, threshold, canny_image;
	VideoCapture capture("rtmp://10.99.10.50:51937/static/bein1raw");    
        
	if (capture.isOpened())
	{
		
		while (true)
		{
			struct timeval tp;
			struct timeval tp2;
			gettimeofday(&tp, NULL);
			long int ms1 = tp.tv_sec * 1000 + tp.tv_usec / 1000;
			long int ms2=0;


			//int64 e1 = cv::getTickCount();
			capture >> frame;
			cv::Mat resized;
			cv::Mat newFrame;
			if (frame.rows > 0 && frame.cols > 0)
			{
				int imgSizeX = frameSizeW;//frame.cols*0.3;
				int imgSizeY = frameSizeH;//frame.rows*0.3;	

		
				cv::resize(frame, resized, Size(imgSizeX, imgSizeY));
				cv::Mat inrangeImage;
				cv::inRange(resized, cv::Scalar(235, 235, 235), cv::Scalar(255, 255, 255), inrangeImage);
				newFrame = resized.clone();
				//cv::imshow("image", inrangeImage);
				//Convert image into gray if it's color image
				if (resized.channels() > 1)
					cv::cvtColor(resized, grayFrame, CV_BGR2GRAY);
				//imshow("Gray Frames", grayFrame);
				cv::threshold(grayFrame, threshold, 235, 255, 3);
				//Dilate Image
				Mat dilate;
				Mat element1 = getStructuringElement(MORPH_ELLIPSE, Size(3, 3), Point(1, 1));
				cv::dilate(threshold, dilate, element1);
				//cv::imshow("image", dilate);
				//Detect Edges by appling canny filter
				cv::Canny(dilate, canny_image, 235, 255, 3);
			//	imshow("Blobs Frames", canny_image);
				//imwrite("testFrame.png", canny_image);
				//Find Contours
				std::vector<std::vector<cv::Point> > contours;
				std::vector<Vec4i> hierarchy;	
				std:vector<Rect> rects_list;

				cv::findContours(canny_image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
				//Convert image back into color from gray image
				cv::cvtColor(canny_image, canny_image, CV_GRAY2BGR);

//struct timeval tp101;
//struct timeval tp202;
//gettimeofday(&tp101, NULL); 
//long int ms101 = tp101.tv_sec * 1000 + tp101.tv_usec / 1000;
cout<< "blob size" << contours.size() << "\n";
				//filter blobs according to specific width and height
				for (int i = 0; i < contours.size(); i++)
				{
					cv::Point p(boundingRect(contours[i]).x, boundingRect(contours[i]).y);
					Rect rec = boundingRect(contours[i]);
					//double wf (canny_image.cols / 192);
					double wf = 3;// canny_image.cols / 192;
					if (rec.width > 2 && rec.height > 5 && rec.width < wf * 3 && rec.height < wf * 4)
					{
						rects_list.push_back(rec);
					}
				}

//gettimeofday(&tp202, NULL);
//long int ms202 = tp202.tv_sec * 1000 + tp202.tv_usec / 1000;
//cout  <<ms202-ms101 << " remove blobs \n";


//struct timeval tp101;
//struct timeval tp202;
//gettimeofday(&tp101, NULL);
//long int ms101 = tp101.tv_sec * 1000 + tp101.tv_usec / 1000;

                                //remove Extra points
                                rects_list = removeExtraPoints(rects_list);
//gettimeofday(&tp202, NULL);
//long int ms202 = tp202.tv_sec * 1000 + tp202.tv_usec / 1000;
//cout  <<ms202-ms101 << "end time \n";

				std::sort(rects_list.begin(), rects_list.end(), byWidth());

				for (int i = 0; i<rects_list.size(); i++)
				{
					if (rects_list.size() > 0 && rects_list[i].width > rects_list[i].height * 4)
					{
						try
						{
							int xx = rects_list[i].x - 10;
							int yy = rects_list[i].y - 2;
							int ww = rects_list[i].width + 15;
							int hh = rects_list[i].height + 7;
							
							if (xx < 0 || xx + ww > newFrame.cols)
							{
								xx = 0;
							}
							if (yy < 0 || yy + hh > newFrame.rows)
							{
								yy = 0;
							}
							if (rects_list[i].x + ww > newFrame.cols)
							{
								ww = newFrame.cols;
							}
							if (rects_list[i].y + hh > newFrame.rows)
							{
								hh = newFrame.rows;
							}

							//	Rect roi(rects_list[i].x, rects_list[i].y, rects_list[i].width, rects_list[i].height);
							if (xx > 0 && yy > 0 && ww > 0 && hh > 0 && Rect(0,0,640,480).contains(Point(xx,yy)))
							{
								Rect roi(xx, yy, ww, hh);
								cv::GaussianBlur(newFrame(roi), newFrame(roi), Size(0, 0), 4);
//cout << " Blurring ";
							}
							/*if (tmpFrame.contains(Point(xx, yy)))
							{
								if (saveDigitsPosition.size() > 0)
								{
									for (size_t i = 0; i < saveDigitsPosition.size(); i++)
									{
										if (saveDigitsPosition[i].x == xx)
										{

										}
									}
								}
								saveDigitsPosition.push_back(roi);
							}*/

						}
						catch (std::exception& ex)
						{
cout << "stuck";
							/*cout << ex.what();
							int xx = rects_list[i].x;
							int yy = rects_list[i].y;
							int ww = rects_list[i].width;
							int hh = rects_list[i].height;
							cout << "\n\n\n\n" << xx << "," << yy << "," << ww << "," << hh;*/

						}
						//cv::rectangle(frame, rects_list[0], Scalar(230, 230, 230), 3, 8, 0);
					}

				}



			//	namedWindow("showme", WINDOW_NORMAL);

//out << "Writing video";
std::ostringstream stm;
							stm << fc;
std::string path = "/root/build/server/output/mjpeg/out"+stm.str()+".mjpg";

				VideoWriter outStream(path, CV_FOURCC('M', 'J', 'P', 'G'), 2, Size(imgSizeX, imgSizeY), true);
fc = fc-1;
cout << fc << "fc \n";
				if (outStream.isOpened() && newFrame.rows > 0 && newFrame.cols > 0)
			{	outStream.write(newFrame);
//cout << "written";

}
						//_sleep(100);
						
				//if (frame.rows > 0 && frame.co
				gettimeofday(&tp2, NULL);
				long int  ms2 = tp2.tv_sec * 1000 + tp2.tv_usec / 1000;
				cout <<  ms2-ms1 << "ms \n";
int res = 40-(ms2-ms1);
//cout << "res = "<< res<< "\n";
if(res > 0 )
{
//cout << "res = "<< res"\n";
sleepcp(res);
////= 30-(ms2-ms1);
//struct timeval tp111;
//long int  ms111=0;
/*while(res>ms111)
{
gettimeofday(&tp111, NULL); 
ms111 =(tp111.tv_sec * 1000 + tp111.tv_usec / 1000)-ms2;
//ms111 = res-ms111;
cout << "ms =>" << ms111;

}
*/
}

			}
			/*float time = (cv::getTickCount() - e1) / cv::getTickFrequency();
			cout << time << "fps \n";*/
		}

	}



	return 0;

}
