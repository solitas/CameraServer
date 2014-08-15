#pragma once
#include <opencv2/opencv.hpp>
using namespace cv;

class CCameraCapture
{
public:
	CCameraCapture();
	~CCameraCapture();

public:
	VideoCapture* capture;

	int CreateCapture(int device_id);

	void GetFrame(Mat* frame);
};

