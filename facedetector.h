#pragma once

#include "opencv2/objdetect.hpp"
#include <boost/thread/mutex.hpp>

class FaceDetector
{
public:
	FaceDetector(const std::string& faceCascadePath);
	void processFrame();
	void setFrame(const cv::Mat& frame);
	cv::Mat getLastFrame();
	void detectFaces(const cv::Mat& frame);
	std::vector<cv::Rect> getRects();
	bool setRects(std::vector<cv::Rect> rects);
protected:
	void detectFacesImpl(std::vector<cv::Rect>& lastRects, const cv::Mat& frame);
	void runFaceDetect(const cv::Mat& frame, std::vector<cv::Rect> &results);
private:
	cv::Mat m_frame;
	std::vector<cv::Rect> m_rects;
	cv::CascadeClassifier m_classifier;
	boost::mutex m_frameLock;
	boost::mutex m_rectsLock;
	double m_roiScaleFactor;
};
