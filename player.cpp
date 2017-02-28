#include "player.h"
#include <time.h> 
#include <windows.h>

using namespace std;
using namespace cv;

Player::Player(QObject *parent)
	: QThread(parent)
{
	m_stop = true;
}

Player::~Player()
{
	m_mutex.lock();
	m_stop = true;
	m_capture.release();
	m_condition.wakeOne();
	m_mutex.unlock();
	wait();
}

bool Player::loadVideo()
{
	m_capture.open(0);
	if (m_capture.isOpened())
	{
		m_frameRate = 60; // static_cast<int>(m_capture.get(CV_CAP_PROP_FPS));
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
			m_stop = false;
		}
		start(LowPriority);
	}
	else
	{
		m_stop = true;
	}
}

void Player::run()
{
	int delay = (1000 / m_frameRate);
	while (!m_stop) {
		if (!m_capture.read(m_frame))
		{
			m_stop = true;
		}
		if (m_frame.channels() == 3) {
			cv::cvtColor(m_frame, m_RGBframe, CV_BGR2RGB);
			deleteBlueColor();
			deleteGreenColor();
			m_img = QImage(static_cast<const uchar*>(m_RGBframe.data),
				m_RGBframe.cols, m_RGBframe.rows, QImage::Format_RGB888);
		}
		else
		{
			m_img = QImage(static_cast<const uchar*>(m_frame.data),
				m_frame.cols, m_frame.rows, QImage::Format_Indexed8);
		}
		emit processedImage(m_img);
		Sleep(static_cast<DWORD>(delay));
		//this->msleep(delay);
	}
}

bool Player::msleep(int ms)
{
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
	m_stop = true;
}

bool Player::isStopped() const 
{
	return this->m_stop;
}

void Player::deleteBlueColor()
{
	applyFunctionToAllPixels([&](Vec3b pixel) { pixel[RGB_BLUE_INDEX] = 0; });
}

void Player::deleteGreenColor()
{
	applyFunctionToAllPixels([&](Vec3b pixel) { pixel[RGB_GREEN_INDEX] = 0; });
}

// TODO: доделать функцию
void Player::applyFunctionToAllPixels(function<void(Vec3b)> functionToPerform)
{
	for (auto y = 0; y < m_RGBframe.rows; y++)
	{
		for (int x = 0; x < m_RGBframe.cols; x++)
		{
			Vec3b pixel = m_RGBframe.at<Vec3b>(Point(x, y));

			functionToPerform(pixel);

			m_RGBframe.at<Vec3b>(Point(x, y)) = pixel;
		}
	}
}