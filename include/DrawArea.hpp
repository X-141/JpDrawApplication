#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QLabel>
#include <QMap>
#include "DrawLayer.hpp"

#include "opencv2/ml.hpp"
//class ProcessLayer;

class DrawArea : public QLabel {
    Q_OBJECT
public:
    DrawArea(QWidget* parent = nullptr);
    ~DrawArea() = default;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    void resizeDrawArea(QSize aSize);

    void updateDrawArea();

    /**
    * @brief generateImage
    * Creates a QImage based on the current
    * Hardlayer.
    * Note, we don't use virtual layer since
    * a virtual layer only contains the currently
    * active pixels.
    */
    QImage generateImage();

    /**
    * @brief setPenWidth
    * Modify the width of the line drawn in
    * the DrawArea layers.
    */
    void setPenWidth(int width);

    /**
    * @brief compareLayer
    * Takes the currently drawn hardlayer dimensions
    * and scales the set of comparison images to match.
    * The important part is that we will compute the comparison
    * value betweent the hardlayer image and the comparison sets.
    */
    int compareLayer();

    /**
    * @brief getComparisonImage
    * Grab the comparison image at index. To be used
    * with value returned from compareLayer()
    */
    QImage getComparisonImage(int index);

    /**
    * @brief undoLayer
    * Remove from mVirtualLayerVector the layer
    * at the head. Note, this does not disable
    * the layer, but instead removes it.
    */
    void undoLayer();

signals:
    /**
     * @brief layer_update_handle
     * Notifies any parent or object that layers have
     * been removed or added.
     */
    void layerUpdateHandle(void);

private:
    void pDrawPoint(QPoint aPoint);

    /**
    * @brief loadComparisonImages
    * Load from memory a series of characters
    * to compare with the users drawings.
    */
    void pLoadComparisonImages();



private:
    // Set to true on mouse down. Set to false on mouse up
    bool mCurrentlyDrawing;
    // Hard layer is what is shown to the user. Contains
    // draw virtual layers that are enabled.
    DrawLayer mHardLayer;
    // Virtual layer is the individual layers that are drawn.
    DrawLayer mVirtualLayer;
    // Used to calculate a line of best fit as the mouse positions
    // are polled.
    // Primarily used in the pDrawPoint() method.
    QPoint mPrevPoint;

    // Vector of layers that the User has drawn. We only
    // store virtual layers in this vector. They are used
    // to draw the hard layer.
    QVector<DrawLayer> mVirtualLayerVector;
    // Incremented when _add_new_layer is called.
    uint mId;
    // Slide width
    uint mPenWidth;
    // A vector of images to display depending
    // on how closely they resemble the image the user has drawn.
    // QVector<QImage> mComparisonImages;
    QMap<int, QImage> mComparisonImagesDict;

    // kNN model used to predict what the user has drawn
    cv::Ptr<cv::ml::KNearest> mKnn;

    // text file path to load in numerical keys to images
    // based on the knn model.
    std::string mKnnDictFilepath;
};

namespace ImageMethods {

    /**
    * @brief prepareMatrixForKNN
    * Given aMat, resize, threshold, and
    * convert it to the proper format to
    * be used in the kNN model.
    */
    static cv::Mat prepareMatrixForKNN(cv::Mat aMat);

    /**
    * @brief obtainROI
    * Given a drawn image, find the region
    * of interest that contains the character
    */
    static cv::Rect obtainROI(cv::Mat aMat);

    /*
     * @brief translocateROI
     * Given a ROI, it will draw the ROI
     * on a base_image of aHeight and aWidth.
     */
    static cv::Mat translocateROI(const cv::Mat& aROI, int aHeight, int aWidth);

    /**
    * @brief rescaleROI
    * Given a ROI, rescale to target scalar values
    * and place ROI images into defined image aHeight and
    * aWidth
    */
    static std::vector<cv::Mat> rescaleROI(const std::vector<float>& aTargetScalars,
                                           const cv::Mat& aROI, int aHeight, int aWidth, bool debugFlag);

    /**
    * @brief findMostFrequentLabel
    * Given a vector of labels, return the label
    * that occurs most frequently.
    */
    static int findMostFrequentLabel(const std::vector<int>& aLabels, bool debugFlag);
}

namespace TechniqueMethods {
    static int ROITranslocation(const cv::Ptr<cv::ml::KNearest>& aKNNModel, const cv::Mat& aBaseImage, bool debugFlag);

    static int ROIRescaling(const cv::Ptr<cv::ml::KNearest>& aKNNModel, const cv::Mat& aBaseImage, bool debugFlag);
}


#endif // DRAWAREA_H
