#include "player.h"
#include <time.h> 
#include <windows.h>
#include <QMessageBox>
#include <QtDebug>
#include <QPainter>
using namespace std;
using namespace cv;

Player::Player()
{
	m_isRunning = true;
	m_faceDetector = shared_ptr<FaceDetector>(new FaceDetector("E:/diplom/visual-control-exe/haarcascades/haarcascade_frontalface_alt.xml"));
	//= shared_ptr<EigenfaceRecognizer>(new cognition::EigenfaceRecognizer);
}

Player::~Player()
{
	m_mutex.lock();
	m_isRunning = false;
	m_capture.release();
	m_condition.wakeOne();
	m_mutex.unlock();
}

void Player::processFrame()
{
	qDebug() << QString("Widget");
	int delay = (1000 / m_frameRate);
	while (m_isRunning)
	{
		if (!m_capture.read(m_frame))
		{
			m_isRunning = false;
		}
		m_faceDetector->detectFaces(m_frame);

		

		if (m_frame.channels() == 3)
		{
			cv::cvtColor(m_frame, m_RGBframe, CV_BGR2RGB);
			m_img = QImage(static_cast<const uchar*>(m_RGBframe.data),
			               m_RGBframe.cols, m_RGBframe.rows, QImage::Format_RGB888);
		}
		else
		{
			m_img = QImage(static_cast<const uchar*>(m_frame.data),
			               m_frame.cols, m_frame.rows, QImage::Format_Indexed8);
		}

		std::vector<cv::Rect> faces = m_faceDetector->getRects();
		QPainter painter;
		painter.begin(&m_img);
		painter.setPen(QPen(Qt::blue, 2));
		if (faces.size() > 0)
		{
			for (auto i = faces.begin();
				i != faces.end(); ++i)
			{
				painter.drawRect((*i).x, (*i).y, (*i).width, (*i).height);
			}
		}
		painter.end();

		emit sendFrame(m_img);
		Sleep(static_cast<DWORD>(delay));
		//this->msleep(delay);
	}
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

void Player::toggle()
{
	qDebug() << QString("toggle");
	//boost::lock_guard<boost::mutex>(this->m_playerLock);
	if (isRunning())
	{
		m_isRunning = false;
	}
	else
	{
		m_isRunning = true;
	}
}

void Player::run()
{
	int delay = (1000 / m_frameRate);
	while (!m_isRunning)
	{
		if (!m_capture.read(m_frame))
		{
			m_isRunning = true;
		}
		if (m_frame.channels() == 3)
		{
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
		emit sendFrame(m_img);
		Sleep(static_cast<DWORD>(delay));
		//this->msleep(delay);
	}
}

bool Player::msleep(int ms)
{
	LONGLONG ns = ms * 1000 * 1000;

	/* Declarations */
	HANDLE timer; /* Timer handle */
	LARGE_INTEGER li; /* Time defintion */
	/* Create timer */
	if (!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
		return FALSE;
	/* Set timer properties */
	li.QuadPart = -ns;
	if (!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE))
	{
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

bool Player::isRunning() const
{
	//boost::lock_guard<boost::mutex>(this->m_playerLock);
	return this->m_isRunning;
}

void Player::deleteBlueColor()
{
	applyFunctionToAllPixels([&](Vec3b pixel)
		{
			pixel[RGB_BLUE_INDEX] = 0;
		});
}

void Player::deleteGreenColor()
{
	applyFunctionToAllPixels([&](Vec3b pixel)
		{
			pixel[RGB_GREEN_INDEX] = 0;
		});
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
