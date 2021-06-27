#include "ProcessLayer.hpp"
#include <QImage>
#include <QString>

//TEST
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include "Log.hpp"

ProcessLayer::ProcessLayer(QImage& image)
	: mPngMatrix(qImageToCvMat(image))
{
	LOG("Creating processing layer.\n");
}

ProcessLayer::ProcessLayer(const QString& imagePath) 
{
	LOG("Creating CV Matrix from filesystem.\n");
	QImage image(imagePath, "PNG");
	mPngMatrix = qImageToCvMat(image);
}


cv::Mat 
qImageToCvMat(QImage image) {
	LOG("Converting QImage to CV Matrix Format.");
	// We have to clone the matrix. See https://stackoverflow.com/questions/11170485/qimage-to-cvmat-convertion-strange-behaviour
	cv::Mat unaltered_mat, grey_mat;
	unaltered_mat = cv::Mat(image.height(), image.width(), CV_8UC4, (uchar *)image.bits(), image.bytesPerLine());
	cv::cvtColor(unaltered_mat, grey_mat, cv::COLOR_BGRA2GRAY);
	return grey_mat.clone();
}
