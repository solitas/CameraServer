#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <WinSock2.h>

using namespace std;

#pragma comment(lib,"ws2_32.lib")
#pragma pack(1)

#define DEFAULT_PORT        5150
#define DEFAULT_BUFFER		4096

int iPort = DEFAULT_PORT;
BOOL   bInterface = FALSE;
char   szAddress[128];

void Listen();
void CaptureCamera();

void main()
{
	Listen();
}

void CaptureCamera()
{
	IplImage* frame;

	CvCapture* capture = cvCaptureFromCAM(0);


	while (1)
	{
		frame = cvRetrieveFrame(capture);
		cvShowImage("camera", frame);
		if (cvWaitKey(10) >= 0)
		{
			break;
		}
	}

	cvReleaseCapture(&capture);
}

void Listen()
{
	WSADATA wsd;
	SOCKET sListen, sClient;
	int iAddrSize;
	HANDLE hThread;
	DWORD dwThreadID;
	struct sockaddr_in serveraddr, client;

	if ((WSAStartup(MAKEWORD(2, 2), &wsd) != 0))
	{
		cout << "failed to load winsock! \n" << endl;
		return;
	}

	sListen = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (sListen == SOCKET_ERROR)
	{
		cout << "socket() failed : " << WSAGetLastError() << endl;
		return;
	}

	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(5150);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(sListen, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR)
	{
		cout << "bind failed" << endl;
		return;
	}

	listen(sListen, 8);

	while (1)
	{
		cout << "WaitClient ..." << endl;
		iAddrSize = sizeof(client);
		sClient = accept(sListen, (struct sockaddr *)&client, &iAddrSize);

		if (sClient == INVALID_SOCKET)
		{
			cout << "accept failed : " << WSAGetLastError() << endl;
			break;
		}
		cout << "Accepted client " << inet_ntoa(client.sin_addr) << " : " << ntohs(client.sin_port) << endl;
		hThread = CreateThread(NULL, 0, ClientThread, (LPVOID)sClient, 0, &dwThreadID);
		if (hThread == NULL)
		{
			cout << "CreateThread failed" << endl;
			break;
		}

		CloseHandle(hThread);
	}
	closesocket(sListen);
	WSACleanup();
}

DWORD WINAPI ClientThread(LPVOID lpParam)
{
	SOCKET sock = (SOCKET)lpParam;
	char szBuffer[DEFAULT_BUFFER];
	int ret, nLeft, idx;

	IplImage* frame;

	CvCapture* capture = cvCaptureFromCAM(0);

	while (1)
	{
		frame = cvRetrieveFrame(capture);
		
		char* frameData = (char*)frame->imageData;
		int totalSize = frame->nSize;
		idx = 0;
		while (totalSize > 0)
		{
			ret = send(sock, &frameData[idx], nLeft, 0);
			if (ret == 0)
				break;
			else if (ret == SOCKET_ERROR)
			{
				cout << "Send Error " << WSAGetLastError() << endl;
				break;
			}
			totalSize -= ret;
			idx += ret;
		}
	}

	cvReleaseCapture(&capture);
}