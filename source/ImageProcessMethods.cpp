#include "ImageProcessMethods.hpp"
#include <QString>
#include <QDebug>
#include <QImage>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include "Log.hpp"

cv::Mat
ImageMethods::qImageToCvMat(QImage aImage) {
	cv::Mat unaltered_mat, grey_mat;
	unaltered_mat = cv::Mat(aImage.height(), aImage.width(), CV_8UC4, (uchar *)aImage.bits(), aImage.bytesPerLine());
	cv::cvtColor(unaltered_mat, grey_mat, cv::COLOR_BGRA2GRAY);
	return grey_mat.clone();
}

int
ImageMethods::passThroughKNNModel(const cv::Ptr<cv::ml::KNearest> &aKNNModel, const cv::Mat &aProcessedImage) {
    auto flatImage = ImageMethods::prepareMatrixForKNN(aProcessedImage);
    cv::Mat input, output;
    input.push_back(flatImage);
    input.convertTo(input, CV_32F);
    output.convertTo(output, CV_32F);
    try {
        aKNNModel->findNearest(input, 4, output);
    } catch(const cv::Exception& ex) {
        ex.what();
        return 0;
    }
    return static_cast<int>(output.at<float>(0));
}

int
ImageMethods::passThroughKNNModel(const cv::Ptr<cv::ml::KNearest>& aKNNModel,
                                  const std::vector<cv::Mat>& aProcessedImages) {
    auto calculatedLabels = std::vector<int>();
    calculatedLabels.reserve(aProcessedImages.size());
    for(auto& mats : aProcessedImages) {
        auto flatImage = ImageMethods::prepareMatrixForKNN(mats);
        cv::Mat input, output;
        input.push_back(flatImage);
        input.convertTo(input, CV_32F);
        output.convertTo(output, CV_32F);
        try {
            aKNNModel->findNearest(input, 4, output);
        } catch(const cv::Exception& ex) {
            ex.what();
            return 0;
        }
        calculatedLabels.push_back((int)output.at<float>(0));
    }

    return ImageMethods::findMostFrequentLabel(calculatedLabels);
}

cv::Rect
ImageMethods::obtainROI(cv::Mat aMat) {
    int max_x = -1, max_y = -1;
    int min_x = INT32_MAX, min_y = INT32_MAX;

    for(int x = 0; x < aMat.rows; ++x) { // width
        for(int y = 0; y < aMat.cols; ++y) { // cols
            if(aMat.at<uchar>(x, y) == 255) {
                if (max_x < x)
                    max_x = x;
                if (max_y < y)
                    max_y = y;
                if (min_x > x)
                    min_x = x;
                if (min_y > y)
                    min_y = y;
            }
        }
    }

//    qInfo() << "Dimensions (x_min, y_min): " << min_x << " " << min_y;
//    qInfo() << "Dimensions (x_max, y_max): " << max_x << " " << max_y;

    return cv::Rect(min_y, min_x, max_y-min_y, max_x-min_x);
}

cv::Mat
ImageMethods::prepareMatrixForKNN(cv::Mat aMat) {
    cv::resize(aMat, aMat, cv::Size(IMAGE_DIMENSION,IMAGE_DIMENSION));
    cv::threshold(aMat, aMat, 15, 255, cv::THRESH_BINARY);
    // Possible alternative. Needs testing to make sure it doesn't cause
    // issues with finding the ROI.
    //cv::threshold(aMat, aMat, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    aMat.convertTo(aMat, CV_32F);
    return aMat.reshape(0, 1);
}

cv::Mat
ImageMethods::translocateROI(const cv::Mat& aROI, int aHeight, int aWidth) {

    uint16_t half_height = aHeight / 2;
    uint16_t half_width = aWidth / 2;

    uint16_t roi_height = aROI.rows;
    uint16_t roi_width = aROI.cols;
    uint16_t roi_half_height = roi_height / 2;
    uint16_t roi_half_width = roi_width / 2;

    uint16_t position_height = std::floor(half_height-roi_half_height);
    uint16_t position_width = std::floor(half_width-roi_half_width);

    cv::Rect target_spot = cv::Rect(position_width, position_height, roi_width, roi_height);

    cv::Mat base_image = cv::Mat(aHeight, aWidth, aROI.type(), cv::Scalar(0,0,0));
    aROI.copyTo(base_image(target_spot));

    return base_image;
}

std::vector<cv::Mat>
ImageMethods::rescaleROI(const std::vector<float>& aTargetScalars, const cv::Mat& aROI,
                         int aHeight, int aWidth, bool debugFlag)
{
    auto scaledROIMats = std::vector<cv::Mat>();

    if(aTargetScalars.empty())
        return scaledROIMats;

    scaledROIMats.reserve(aTargetScalars.size());

    for(const auto& scalar : aTargetScalars) {
        auto roiCopy = aROI.clone();
        if (scalar * roiCopy.rows >= aHeight || scalar * roiCopy.cols >= aWidth)
            continue;

        cv::resize(roiCopy, roiCopy, cv::Size(), scalar, scalar);
        auto outputMat = ImageMethods::translocateROI(roiCopy, aHeight, aWidth);
        if(debugFlag) cv::imwrite("scaled_image" + std::to_string(scalar) + ".png", outputMat);
        scaledROIMats.emplace_back(std::move(outputMat));
    }

    return scaledROIMats;
}

int
ImageMethods::findMostFrequentLabel(const std::vector<int>& aLabels) {
    std::map<int, int> labelCounter;

    for(const auto& label : aLabels) {
        auto iter = labelCounter.find(label);
        if(iter != labelCounter.end())
            iter->second += 1;
        else
            labelCounter.insert({label, 1});
    }

    int mostFrequentLabel = 0;
    int maxValue = -1;
    for(const auto& labels : labelCounter) {
        // LOG(level::standard, "ImageMethods::findMostFrequentLabel()",
        //  "Label: " + QString::number(labels.first) + " count: " + QString::number(labels.second));
        if(labels.second > maxValue) {
            mostFrequentLabel = labels.first;
            maxValue = labels.second;
        }
    }
    // LOG(level::standard, "ImageMethods::findMostFrequentLabel()",
    //  "Most frequent label: " + QString::number(mostFrequentLabel));
    return mostFrequentLabel;
}


cv::Mat
TechniqueMethods::ROITranslocation(const cv::Mat& aBaseImage, bool debugFlag) {

    if(debugFlag) cv::imwrite("RAW_IMAGE.png", aBaseImage);

    cv::Rect roi = ImageMethods::obtainROI(aBaseImage);
    cv::Mat translocatedImage = ImageMethods::translocateROI(aBaseImage(roi).clone(),
                                                             aBaseImage.cols, aBaseImage.rows);

    return translocatedImage;
}

std::vector<cv::Mat>
TechniqueMethods::ROIRescaling(const cv::Mat& aBaseImage, bool debugFlag) {

    if(debugFlag) cv::imwrite("RAW_IMAGE.png", aBaseImage);

    cv::Rect roi = ImageMethods::obtainROI(aBaseImage);

    // Current selected scalar values are not based on any empirical data.
    // just what feels right.
    auto rescaledMats = ImageMethods::rescaleROI({.70, .75, .80, .85, .90, .95, 1.0, 1.05, 1.10},
                                                 aBaseImage(roi).clone(),
                                                 aBaseImage.rows, aBaseImage.cols, debugFlag);

    if(rescaledMats.empty())
        LOG(level::warning, "TechniqueMethods::ROIRescaling()", "rescaleROI() returned a empty vector.");
    return rescaledMats;
}

