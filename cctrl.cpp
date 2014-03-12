#include "stdafx.h"
#include <opencv2\opencv.hpp>
#include <Windows.h>

int SCREENX = 0, SCREENY = 0;
int XRATIO = 0, YRATIO = 0;

IplImage* threshImage(IplImage *imgOrig, CvScalar lower, CvScalar upper, int n)
{
	IplImage* imgHSV = cvCreateImage(cvGetSize(imgOrig), 8, 3);   //size, depth, channels
	cvCvtColor(imgOrig, imgHSV, CV_BGR2HSV);   //check!

	IplImage* imgThresh = cvCreateImage(cvGetSize(imgOrig), 8, 1);
	cvInRangeS(imgHSV, lower, upper, imgThresh);

	
	CvMoments *moments = (CvMoments*)malloc(sizeof(CvMoments));
	cvMoments (imgThresh, moments, 1);

	double moment10 = cvGetSpatialMoment(moments, 1, 0);
	double moment01 = cvGetSpatialMoment(moments, 0, 1);
	double area = cvGetSpatialMoment(moments, 0, 0);

	static int posX = 0;
	static int posY = 0;


	posX = moment10/area;
	posY = moment01/area;

	int curX = posX * XRATIO;
	int curY = posY * YRATIO;

	SetCursorPos(1366-curX, curY);

	delete moments;
	cvReleaseImage(&imgHSV);

	return imgThresh;
}


int main()
{
	SetCursorPos(0,0);
	CvCapture* capture = 0;
	capture = cvCaptureFromCAM(0);
	
	SCREENX = GetSystemMetrics(SM_CXSCREEN);
	SCREENY = GetSystemMetrics(SM_CYSCREEN);

	XRATIO = SCREENX/640;
	YRATIO = SCREENY/480;
	if(!capture)
	{
		printf("couldn't capture...\n");
		return -1;
	}

	IplImage* imgScribble = NULL;

	cvNamedWindow("video", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("yellow_thresh", CV_WINDOW_AUTOSIZE);

	while(1)
	{
		IplImage* frame;
		frame = cvQueryFrame(capture);

		if(!frame)
			break;

		//IplImage* YellowThreshImage = threshImage(frame, cvScalar(20, 80, 80), cvScalar(30, 255, 255), 0);
		IplImage* YellowThreshImage = threshImage(frame, cvScalar(0, 105, 95), cvScalar(14, 130, 101), 0);
		//IplImage* RedThreshImage = threshImage(frame, cvScalar(0, 80, 80), cvScalar(5, 255, 255), 1);
		//IplImage* BlueThreshImage = threshImage(frame, cvScalar(110, 100, 50), cvScalar(120, 255, 255), 2);
		
		cvShowImage("video", frame);
		cvShowImage("yellow_thesh", YellowThreshImage);
		//cvShowImage("red_thresh", RedThreshImage);
		//cvShowImage("blue_thresh", BlueThreshImage);
		int c = cvWaitKey(10);

		if(c != -1)
		{
			break;
		}

		cvReleaseImage(&YellowThreshImage);
		//cvReleaseImage(&BlueThreshImage);
		//cvReleaseImage(&RedThreshImage);
		
	}

	cvReleaseCapture(&capture);
	
	return 0;
}

