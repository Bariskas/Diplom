#include "player.h"
#include <time.h> 
#include <windows.h>

using namespace std;
using namespace cv;

Player::Player(QObject *parent)
	: QThread(parent)
{
	stop = true;
}

Player::~Player()
{
	mutex.lock();
	stop = true;
	capture.release();
	condition.wakeOne();
	mutex.unlock();
	wait();
}

bool Player::loadVideo() {
	capture.open(0);
	if (capture.isOpened())
	{
		frameRate = 60; // static_cast<int>(capture.get(CV_CAP_PROP_FPS));
		return true;
	}
	else
	{
		return false;
	}
}

void Player::Play()
{
	if (!isRunning()) 
	{
		if (isStopped()) 
		{
			stop = false;
		}
		start(LowPriority);
	}
	else
	{
		stop = true;
	}
}

void Player::run()
{
	int delay = (1000 / frameRate);
	while (!stop) {
		if (!capture.read(frame))
		{
			stop = true;
		}
		if (frame.channels() == 3) {
			cv::cvtColor(frame, RGBframe, CV_BGR2RGB);
			img = QImage(static_cast<const uchar*>(RGBframe.data),
				RGBframe.cols, RGBframe.rows, QImage::Format_RGB888);
		}
		else
		{
			img = QImage(static_cast<const uchar*>(frame.data),
				frame.cols, frame.rows, QImage::Format_Indexed8);
		}
		emit processedImage(img);
		Sleep(static_cast<DWORD>(delay));
		//this->msleep(delay);
	}
}

bool Player::msleep(int ms) {
	LONGLONG ns = ms * 1000 * 1000;

	/* Declarations */
	HANDLE timer;	/* Timer handle */
	LARGE_INTEGER li;	/* Time defintion */
						/* Create timer */
	if (!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
		return FALSE;
	/* Set timer properties */
	li.QuadPart = -ns;
	if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE)) {
		CloseHandle(timer);
		return FALSE;
	}
	/* Start & wait for timer */
	WaitForSingleObject(timer, INFINITE);
	/* Clean resources */
	CloseHandle(timer);
	/* Slept without problems */
	return TRUE;
}


void Player::Stop()
{
	stop = true;
}

bool Player::isStopped() const {
	return this->stop;
}