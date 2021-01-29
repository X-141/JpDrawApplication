#ifndef DRAWAREA_H
#define DRAWAREA_H

#include <QLabel>
#include "DrawLayer.hpp"


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
     * @brief _add_new_layer
     * @param aPixmap
     * Handles the addition of a new layer. Creates
     * a QPair of uint and aPixmap and adds it to
     * id_layer_pairs.
     */
    void pAddNewLayer(QPixmap aPixmap);

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
};

#endif // DRAWAREA_H
