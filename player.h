#ifndef PLAYER_H
#define PLAYER_H

#include <QMutex>
#include <QThread>
#include <QImage>
#include <QWaitCondition>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class Player : public QThread
{
	Q_OBJECT

signals :
	//Signal to output frame to be displayed
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
	bool stop;
	QMutex mutex;
	QWaitCondition condition;
	cv::Mat frame;
	int frameRate;
	cv::VideoCapture capture;
	cv::Mat RGBframe;
	QImage img;
};

#endif // PLAYER_H
