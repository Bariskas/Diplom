#ifndef PLAYER_H
#define PLAYER_H

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <functional>
#include <memory>
#include "FaceDetector.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>
#include <QPainter>

class Player : public QObject
{
	Q_OBJECT
public slots:
	void processFrame();
	void toggle();
signals :
	//Signal to output m_frame to be displayed
	void sendFrame(const QImage &image);

public:
	//Constructor
	Player();
	//Destructor
	~Player();
	//Load a video from webcam
	bool loadVideo();
	//check if the player has been stopped
	bool isRunning() const;

protected:
	void run();
	bool msleep(int ms);

private:
	void Player::applyFunctionToAllPixels(std::function<void(cv::Vec3b)> functionToPerform);
	void deleteBlueColor();
	void deleteGreenColor();

private:
	bool m_isRunning;
	QMutex m_mutex;
	QWaitCondition m_condition;
	cv::Mat m_frame;
	int m_frameRate;
	cv::VideoCapture m_capture;
	cv::Mat m_RGBframe;
	QImage m_img;
	std::shared_ptr<FaceDetector> m_faceDetector;
	mutable boost::mutex m_playerLock;

	const int RGB_BLUE_INDEX = 2;
	const int RGB_GREEN_INDEX = 2;
};

#endif // PLAYER_H
