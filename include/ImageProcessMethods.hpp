#ifndef IMAGEPROCESSMETHODS_HPP
#define IMAGEPROCESSMETHODS_HPP

#include <vector>
#include "opencv2/ml.hpp"

// In the future, this could be set by information stored in resource files.
// This avoids errors if the kNN model parameters change.
constexpr int IMAGE_DIMENSION = 48;

class QImage;

namespace ImageMethods {
    /**
    * @brief Converts a QImage to a compatible OpenCV matrix. Ideally, used when converting a the hardlayer
    * to OpenCV matrix to begin processing with available libraries.
    * Note: Returned image will be grayscale.
    * References: http://qtandopencv.blogspot.com/2013/08/how-to-convert-between-cvmat-and-qimage.html
    * References: https://stackoverflow.com/questions/11170485/qimage-to-cvmat-convertion-strange-behaviour
    * @param aImage a Qt Image 
    * @return Returns a converted QImage to a grayscale Opencv matrix.
    */
    cv::Mat qImageToCvMat(QImage aImage);

    /**
     * @brief Pass a processed image through the kNN model.
     * @param aKNNModel Pointer to currently loaded model.
     * @param aProcessedImage An image processed through one of the builtin techniques.
     * @return int label calculated by the kNN model.
     */
    int passThroughKNNModel(const cv::Ptr<cv::ml::KNearest>& aKNNModel, const cv::Mat& aProcessedImage);

    /**
    * @brief Pass a set of processed images through the kNN model.
    * @param Pointer to currently loaded model.
    * @param aProcessedImages A set of processed images give by one of the builtin techniques.
    * @return int label calculated by the kNN model.
    */
    int passThroughKNNModel(const cv::Ptr<cv::ml::KNearest>& aKNNModel, const std::vector<cv::Mat>& aProcessedImages);

    /**
    * @brief Resizes the passed matrix to correct and datatype to passed through the loaded kNN model.
    * The dimension the matrix will be converted to is IMAGE_DIMENSION x IMAGE_DIMENSION. In addition,
    * the datatype will be converted to CV_32F.
    * @param aMat An OpenCV matrix that will be converted to correct dimension and datatype.
    * @return converted matrix of dimension IMAGE_DIMENSION x IMAGE_DIMENSION and CV_32F datatype.
    */
    cv::Mat prepareMatrixForKNN(cv::Mat aMat);

    /**
    * @brief Given a drawn image, find the region
    * of interest that contains the character.
    * NOTE: The passed aMat must be grayscaled and thresholded! Otherwise,
    * finding the correct ROI will not be possible.
    * @param aMat An OpenCV matrix that must be greyscaled and thresholded.
    * @return OpenCV rect containing the dimensions for the character drawn
    * by the user.
    */
    cv::Rect obtainROI(cv::Mat aMat);

    /**
     * @brief given an ROI image, create a blank image of aHeight x aWidth and
     * center the ROI image into that blank image.
     * @param aROI An OpenCV matrix containing the character drawn by the user.
     * @param aHeight Height dimension of the original image the ROI was extracted from.
     * @param aWidth Width dimension of the original image the ROI was extracted from.
     * @return OpenCV mat containing a centered ROI.
     */
    cv::Mat translocateROI(const cv::Mat& aROI, int aHeight, int aWidth);

    /**
     * @brief Given a set of set of scalar values, rescale the ROI by each value, center each rescaled ROI,
     * and return back to caller. Used in conjuction with ROIRescaling.
     * @param aTargetScalar Set of scalar values to rescale ROI with.
     * @param aROI an OpenCV matrix containing the ROI, which is the character drawn by the user.
     * @param aHeight Height of original image.
     * @param aWidth Width of original image.
     * @param debugFlag Boolean flag to write sample images and print statements.
     * @return A vector of rescaled ROI centered in original image dimensions.
     */
    std::vector<cv::Mat> rescaleROI(const std::vector<float>& aTargetScalars,
                                           const cv::Mat& aROI, int aHeight, int aWidth, bool debugFlag);

    /**
     * @brief Given a set of calculated labels, find the most frequently occuring label.
     * @param aLabels vector of calculated labels.
     * @return Most frequently occuring label in vector.
     */
    int findMostFrequentLabel(const std::vector<int>& aLabels);
}

namespace TechniqueMethods {
    /**
     * @brief Takes a given drawn image by the user, find the ROI, and center the ROi into the
     * image's original dimension.
     * @param aBaseImage Drawn character by the user. Needs to be an OpenCV matrix.
     * @param debugFlag Boolean flag to write sample images and print statements.
     * @return Return an image which has a centered ROI.
     */
    cv::Mat ROITranslocation(const cv::Mat& aBaseImage, bool debugFlag);

    // Method Description:
    // Takes a given base image, find ROI, and centers it into image of original dimensions.
    // After centering, the ROI is then scaled to various scalar values.
    // Purpose:
    // Because characters can vary in dimensions, we can scale the ROI to mimic
    // how varied characters can be drawn.

    /**
     * @brief Takes a given base image, find ROI, and rescale ROI by scalar values and center ROI into original image dimension.
     * @param aBaseImage Drawn character by the user. Needs to be an OpenCV matrix.
     * @param debugFlag Boolean flag to write sample images and print statements.
     * @return Return a vector of ROI rescaled images.
     */
    std::vector<cv::Mat> ROIRescaling(const cv::Mat& aBaseImage, bool debugFlag);
}

#endif // !IMAGEPROCESSMETHODS_HPP
