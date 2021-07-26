#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <vector>
#include "opencv2/ml.hpp"

#include <QLabel>
#include <QMap>
#include <QPointer>

#include "DrawLayer.hpp"
#include "Log.hpp"

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
    * @brief Modify the width of the line drawn in
    * the DrawArea layers.
    * @param width A new width to set pen to.
    */
    void setPenWidth(int width);

    /**
    * @brief Takes the currently drawn hardlayer dimensions
    * and scales the set of comparison images to match.
    * The important part is that we will compute the comparison
    * value between the hardlayer image and the comparison sets.
    */
    int compareLayer();

    /**
    * @brief Grab the comparison image at index. To be used
    * with value returned from compareLayer()
    * @param aLabel numeric value representing a label for a character
    */
    QImage getResourceCharacterImage(int aLabel);

    /**
    * @brief Remove from mVirtualLayerVector the layer
    * at the head. Note, this does not disable
    * the layer, but instead removes it.
    */
    void undoLayer();

signals:
    /**
     * @brief Notifies any parent or object that layers have
     * been removed or added.
     */
    void layerUpdateHandle();

private:
    /**
     * @brief Draws a point between the previous point drawn and
     * the new point created by the users mouse press.
     * @param aPoint A new point to draw a line to from previous point.
     */
    void pDrawPoint(QPoint aPoint);

    /**
    * @brief Load from memory a series of characters
    * to compare with the users drawings.
    */
    void pResourceCharacterImages();

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

#endif // DRAWAREA_H
