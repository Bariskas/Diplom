#include "FaceDetector.h"
#include <boost/thread/locks.hpp> // for boost::lock_guard
#include <QMessageBox>
#include <QtDebug>

FaceDetector::FaceDetector(const std::string& faceCascadePath)
	: m_roiScaleFactor(1.16)
{
	m_classifier.load(faceCascadePath);
}

void FaceDetector::processFrame()
{
	 cv::Mat frame = getLastFrame();

	if (frame.empty()) return;

	detectFaces(frame);
}

void FaceDetector::setFrame(const cv::Mat& frame)
{
	boost::lock_guard<boost::mutex>(this->m_frameLock);
	m_frame = frame;
}

cv::Mat FaceDetector::getLastFrame()
{
	boost::lock_guard<boost::mutex>(this->m_frameLock);
	return m_frame;
}

void FaceDetector::detectFaces(const cv::Mat& frame)
{
	if (m_classifier.empty())
	{
		qDebug() << QString("Face cascade classifier not founded while detecting face.");
		return;
	}

	try
	{
		std::vector<cv::Rect> tempRects;
		runFaceDetect(frame, tempRects);
		setRects(tempRects);
	}
	catch (cv::Exception& e)
	{
		QMessageBox msgBox;
		msgBox.setText("Exception while detecting face on image");
		msgBox.exec();
	}
}

std::vector<cv::Rect> FaceDetector::getRects()
{
	boost::lock_guard<boost::mutex>(this->m_rectsLock);
	//the scope is not left until return statement is completed
	return m_rects;
}

bool FaceDetector::setRects(std::vector<cv::Rect> newRects)
{
	if (m_rects.size() == newRects.size() &&
		std::equal(m_rects.begin(), m_rects.end(), newRects.begin()))
	{
		qDebug() << QString("Equal.");
		//there is nothing new, don't update and notify!
		return false;
	}

	m_rectsLock.lock();
	m_rects = newRects;
	m_rectsLock.unlock();

	return true;
}

void FaceDetector::detectFacesImpl(std::vector<cv::Rect>& lastRects, const cv::Mat& frame)
{
	std::vector<cv::Rect> newRects;

	for (auto iterator = lastRects.begin(); iterator != lastRects.end(); ++iterator)
	{
		std::vector<cv::Rect> tempRects;


		//make sure the current rectange doesn't contain unrealistic values!
		//this can cause openCV to throw!
		if (iterator->x < 0 ||
			iterator->y < 0 ||
			iterator->width < 2 ||
			iterator->height < 2)
			continue;

		//openCV detects a rect, so only the width is sufficent
		int adjustFactor = std::max(
			(static_cast<int>(iterator->width * m_roiScaleFactor) - iterator->width) / 2,
			35
		);

		//set the region of interest to the previous frame
		cv::Mat roiFrame = frame(*iterator);

		//make the frame bigger, but do not exceed boundaries!
		roiFrame.adjustROI(adjustFactor * 1.10, adjustFactor * 1.10, adjustFactor, adjustFactor);

		//detect on smaller frame
		runFaceDetect(roiFrame, tempRects);

		//add the results to the newRects vector!
		for (auto iRect = tempRects.begin(); iRect != tempRects.end(); ++iRect)
		{
			iRect->x += iterator->x - adjustFactor;
			iRect->y += iterator->y - adjustFactor;
			newRects.push_back(*iRect);
		}
	}

	//if no faces were found, scan the whole image!
	if (newRects.empty())
		runFaceDetect(frame, newRects);

	//update the area 'result' variable
	setRects(newRects);
}

inline void FaceDetector::runFaceDetect(const cv::Mat& frame, std::vector<cv::Rect>& results)
{
	m_classifier.detectMultiScale(frame, /*the frame, input frame for detection*/
	                              results, /*the the result vector, the output*/
	                              1.13, /*the the scale factor opencv uses to increase the window each pass, default 1.1*/
	                              3, /*minNeighbors, default: 3 (the min. number of rects to group together to call it a face)*/
	                              cv::CASCADE_DO_ROUGH_SEARCH, //,
	                              /*flags, Canny Prunning runs the canny edge detector to elimiate regions
	                              which are unlikely to contain faces*/
	                              cv::Size(55, 55) /*min rect check size, the minimum!*/
	);
}
