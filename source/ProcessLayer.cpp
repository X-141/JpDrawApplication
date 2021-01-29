#include "ProcessLayer.hpp"
#include <QImage>
#include <QString>

//TEST
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"

#include "Log.hpp"

ProcessLayer::ProcessLayer(QImage& image)
	: mPngMatrix(qImageToCvMat(image))
{
	LOG("Creating processing layer.\n");
	//LOG("Writing out imagePath to png file: ");
	//LOG("C:\\Users\\seanp\\source\\JpDrawApplication\\build\\testFILE.png\n");
	//cv::imwrite("C:\\Users\\seanp\\source\\JpDrawApplication\\build\\testFILE.png", mPngMatrix);
}

ProcessLayer::ProcessLayer(const QString& imagePath) 
{
	LOG("Creating CV Matrix from filesystem.\n");
	QImage image(imagePath, "PNG");
	mPngMatrix = qImageToCvMat(image);
	// Lets try writing out the matrix file.
	// LOG("Writing out imagePath to png file: ");
	// LOG("C:\\Users\\seanp\\source\\JpDrawApplication\\build\\testFILE.png\n");
	// cv::imwrite("C:\\Users\\seanp\\source\\JpDrawApplication\\build\\testFILE.png", mPngMatrix);
}


cv::Mat
ProcessLayer::qImageToCvMat(QImage& image) {
	LOG("Converting QImage to CV Matrix Format.");
	// We have to clone the matrix. See https://stackoverflow.com/questions/11170485/qimage-to-cvmat-convertion-strange-behaviour
	return cv::Mat(image.height(), image.width(), CV_8UC4, (uchar *)image.bits(), image.bytesPerLine()).clone();
}