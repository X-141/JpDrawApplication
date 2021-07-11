#ifndef IMAGEPROCESSMETHODS_HPP
#define IMAGEPROCESSMETHODS_HPP

#include <vector>
#include "opencv2/imgproc.hpp"
#include "opencv2/ml.hpp"
#include "opencv2/imgcodecs.hpp"

#include <QImage>
#include <QDebug>

namespace ImageMethods {

    cv::Mat qImageToCvMat(QImage image);

    /**
    * @brief prepareMatrixForKNN
    * Given aMat, resize, threshold, and
    * convert it to the proper format to
    * be used in the kNN model.
    */
    cv::Mat prepareMatrixForKNN(cv::Mat aMat);

    /**
    * @brief obtainROI
    * Given a drawn image, find the region
    * of interest that contains the character
    */
    cv::Rect obtainROI(cv::Mat aMat);

    /*
     * @brief translocateROI
     * Given a ROI, it will draw the ROI
     * on a base_image of aHeight and aWidth.
     */
    cv::Mat translocateROI(const cv::Mat& aROI, int aHeight, int aWidth);

    /**
    * @brief rescaleROI
    * Given a ROI, rescale to target scalar values
    * and place ROI images into defined image aHeight and
    * aWidth
    */
    std::vector<cv::Mat> rescaleROI(const std::vector<float>& aTargetScalars,
                                           const cv::Mat& aROI, int aHeight, int aWidth, bool debugFlag);

    /**
    * @brief findMostFrequentLabel
    * Given a vector of labels, return the label
    * that occurs most frequently.
    */
    int findMostFrequentLabel(const std::vector<int>& aLabels, bool debugFlag);
}

namespace TechniqueMethods {
    int ROITranslocation(const cv::Ptr<cv::ml::KNearest>& aKNNModel, const cv::Mat& aBaseImage, bool debugFlag);

    int ROIRescaling(const cv::Ptr<cv::ml::KNearest>& aKNNModel, const cv::Mat& aBaseImage, bool debugFlag);
}

// References: http://qtandopencv.blogspot.com/2013/08/how-to-convert-between-cvmat-and-qimage.html
//cv::Mat qImageToCvMat(QImage& image);

#endif // !IMAGEPROCESSMETHODS_HPP
