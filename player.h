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

class Player : public QThread
{
	Q_OBJECT

signals :
	//Signal to output m_frame to be displayed
	void processedImage(const QImage &image);

public:
	//Constructor
	Player(QObject *parent = 0);
	//Destructor
	~Player();
	//Load a video from webcam
	bool loadVideo();
	//Play the video
	void Play();
	//Stop the video
	void Stop();
	//check if the player has been stopped
	bool isStopped() const;

protected:
	void run();
	bool msleep(int ms);

private:
	void Player::applyFunctionToAllPixels(std::function<void(cv::Vec3b)> functionToPerform);
	void deleteBlueColor();
	void deleteGreenColor();

private:
	bool m_stop;
	QMutex m_mutex;
	QWaitCondition m_condition;
	cv::Mat m_frame;
	int m_frameRate;
	cv::VideoCapture m_capture;
	cv::Mat m_RGBframe;
	QImage m_img;

	const int RGB_BLUE_INDEX = 2;
	const int RGB_GREEN_INDEX = 2;
};

#endif // PLAYER_H
