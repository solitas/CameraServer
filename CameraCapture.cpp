#include "CameraCapture.h"


CCameraCapture::CCameraCapture()
{
}


CCameraCapture::~CCameraCapture()
{
}

int CCameraCapture::CreateCapture(int device_id)
{
	capture = new VideoCapture(device_id);
	if (!capture->isOpened())
		return -1;
}

void CCameraCapture::GetFrame(Mat* frame)
{
	if (!capture->isOpened())
	{
		return;
	}
	else
	{
		capture->retrieve(*frame);
	}
}
