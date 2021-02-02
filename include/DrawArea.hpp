#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QLabel>
#include "DrawLayer.hpp"

class ProcessLayer;

class DrawArea : public QLabel {
    Q_OBJECT
public:
    DrawArea(QWidget* parent = nullptr);
    ~DrawArea() = default;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;

    void resizeDrawArea(QSize aSize);

    const QVector<DrawLayer>& getListOfLayers() const;

    DrawLayer& getLayerById(uint aId);

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

signals:
    /**
     * @brief layer_update_handle
     * Notifies any parent or object that layers have
     * been removed or added.
     */
    void layerUpdateHandle(void);

private:
    void pDrawPoint(QPoint aPoint);

    ///**
    // * @brief _add_new_layer
    // * @param aPixmap
    // * Handles the addition of a new layer. Creates
    // * a QPair of uint and aPixmap and adds it to
    // * id_layer_pairs.
    // */
    //void pAddNewLayer(QPixmap aPixmap);

    /**
    * @brief loadComparisonImages
    * Load from memory a series of characters
    * to compare with the users drawings.
    */
    void loadComparisonImages();

private:
    // Set to true on mouse down. Set to false on mouse up
    bool mCurrentlyDrawing;
    // Hard layer is what is shown to the user. Contains
    // draw virtual layers that are enabled.
    DrawLayer mHardLayer;
    // Virtual layer is the induvidual layers that are drawn.
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
    QVector<ProcessLayer> mComparisonImages;

    // Keep track of the dimensions of the draw space.
    int mMax_x, mMax_y, mMin_x, mMin_y;
};

#endif // DRAWAREA_H
