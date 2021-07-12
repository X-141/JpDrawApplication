#ifndef IMAGEPROCESSMETHODS_HPP
#define IMAGEPROCESSMETHODS_HPP

#include <vector>
#include "opencv2/ml.hpp"

class QImage;

namespace ImageMethods {
    /**
    * @brief qImageToCvMat
    * Converts a QImage to a opencv matrix.
    * References: http://qtandopencv.blogspot.com/2013/08/how-to-convert-between-cvmat-and-qimage.html
    */
    cv::Mat qImageToCvMat(QImage image);

    /**
    * @brief passThroughKNNModel
    * convenience method used to pass
    * a processed image through a knn model
    * and return a label
    */
    int passThroughKNNModel(const cv::Ptr<cv::ml::KNearest>& aKNNModel, const cv::Mat& aProcessedImage);

    /**
    * @brief passThroughKNNModel
    * convenience method used to pass
    * a processed image through a knn model
    * and return a label.
    *
    * This is alternative method if user has
    * a set of images that need to be passed
    * through a knn model.
    */
    int passThroughKNNModel(const cv::Ptr<cv::ml::KNearest>& aKNNModel, const std::vector<cv::Mat>& aProcessedImages);

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
    int findMostFrequentLabel(const std::vector<int>& aLabels);
}

namespace TechniqueMethods {
    // Method Description:
    // Takes a given base image, find ROI, and centers it into image of original dimensions.
    // Purpose:
    // The dataset used to create the kNN model has images that are centered.
    cv::Mat ROITranslocation(const cv::Mat& aBaseImage, bool debugFlag);

    // Method Description:
    // Takes a given base image, find ROI, and centers it into image of original dimensions.
    // After centering, the ROI is then scaled to various scalar values.
    // Purpose:
    // Because characters can vary in dimensions, we can scale the ROI to mimic
    // how varied characters can be drawn.
    std::vector<cv::Mat> ROIRescaling(const cv::Mat& aBaseImage, bool debugFlag);
}

#endif // !IMAGEPROCESSMETHODS_HPP
