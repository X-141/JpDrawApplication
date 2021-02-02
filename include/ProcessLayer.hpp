#ifndef PROCESSLAYER_HPP
#define PROCESSLAYER_HPP

#include "opencv2/core/mat.hpp"

/** 
* The purpose of the process layer is to create a Opencv Matrix
* object based on a PNG passed on construction. 
* This class will allow for two pngs to be compared in a quantifiable fashion.
*/

/*The assumed format of the ProcessLayer will be png*/

class QImage;
class QString;

class ProcessLayer {
public:
	// ProcessLayer(/*PNG file path*/);
	ProcessLayer(QImage& image);
	ProcessLayer(const QString& imagePath);
	~ProcessLayer() = default;
public:
	cv::Mat mPngMatrix;
	// May need a png object.
};

// References: http://qtandopencv.blogspot.com/2013/08/how-to-convert-between-cvmat-and-qimage.html
cv::Mat qImageToCvMat(QImage& image);

#endif // !PROCESSLAYER_HPP
