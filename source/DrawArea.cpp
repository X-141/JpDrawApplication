#include "DrawArea.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>

#include "LineMethods.hpp"

DrawArea::DrawArea(QWidget* parent)
    : QLabel(parent),
      mCurrentlyDrawing(false),
      mHardLayer(this->size(), 0),
      mVirtualLayer(this->size(), 0),
      mId(1)
{
    this->clear();

    mHardLayer.fill(); // set to white.
    this->setPixmap(mHardLayer);

    // Reserve some space in memory to avoid
    // heap allocating more space.
    mVirtualLayerVector.reserve(32);
}

void
DrawArea::mousePressEvent(QMouseEvent* event) {
//    qDebug() << "Mouse press: " << event->pos() << "\n";
    mVirtualLayer = DrawLayer(this->size(), mId++);
    mVirtualLayer.fill(Qt::transparent);
    updateDrawArea();
    // Indicate that we are beginning to draw.
    mCurrentlyDrawing = true;
    // Draw the starting point.
    mPrevPoint = event->pos();
    pDrawPoint(event->pos());
}

void
DrawArea::mouseReleaseEvent(QMouseEvent* event) {
//    qDebug() << "Mouse release: " << event->pos() << "\n";
    // Add the finished layer to layer vector
    mVirtualLayer.setEnableStatus(true);
    mVirtualLayerVector.append(mVirtualLayer);
    // Call update handle to let other objects or owner
    // know of the changes.
    layerUpdateHandle();
    // We have finished drawing the layer
    mCurrentlyDrawing = false;
    // Indicate that the mPrevPoint is invalid.
    mPrevPoint = QPoint(-1,-1);
}

#include <QElapsedTimer>

void
DrawArea::mouseMoveEvent(QMouseEvent *event) {
//    QElapsedTimer timer;
//    timer.start();
    //qDebug() << "Mouse moved: " << event->pos() << "\n";
    if(mCurrentlyDrawing) {
        QElapsedTimer timer;
        timer.start();
        //qDebug() << "We are dragging!\n";
        pDrawPoint(event->pos());
    }
//    qInfo() << "Time it took to calculate " << timer.nsecsElapsed() << " nanoseconds\n";
}

void
DrawArea::resizeDrawArea(QSize aSize) {
    // First clear out current label and resize
    this->clear();
    resize(aSize);
    // Be careful, We are not really copying any pixel data
    // from the original pixmap(s), we are just resizing.
    mHardLayer = DrawLayer(aSize, mHardLayer.getId());
    mVirtualLayer = DrawLayer(aSize, mVirtualLayer.getId());
    // Set the pixmap in this object (recall we are a QLabel).
    this->setPixmap(mHardLayer);
}

const QVector<DrawLayer>&
DrawArea::getListOfLayers() const {
    return mVirtualLayerVector;
}

DrawLayer&
DrawArea::getLayerById(uint aId) {
    return mVirtualLayerVector[aId-1];
}

void
DrawArea::updateDrawArea() {
    this->clear();
    mHardLayer = DrawLayer(this->size(), mVirtualLayer.getId());
    mHardLayer.fill();
    QPainter painter = QPainter(&mHardLayer);
    for(const auto& layers: mVirtualLayerVector)
        if(layers.isEnabled())
            painter.drawPixmap(0,0, layers);
    painter.end();
    this->setPixmap(mHardLayer);
}

void
DrawArea::pDrawPoint(QPoint aPoint) {
    auto painter_hard = QPainter(&mHardLayer);
    auto painter_virt = QPainter(&mVirtualLayer);
    painter_hard.setBrush(QBrush(Qt::black));
    painter_virt.setBrush(QBrush(Qt::black));

    // For now we will just check if it works.
    QVector<QPoint> VectorOfCalcdPoints;
    VectorOfCalcdPoints.reserve(64);
    calculateQPoints(mPrevPoint, aPoint, VectorOfCalcdPoints);
    for(const auto& point : VectorOfCalcdPoints) {
        //qInfo() << "Point calculated: " << point;
        painter_hard.drawPoint(point);
        painter_virt.drawPoint(point);
    }
    mPrevPoint = aPoint;

    this->setPixmap(mHardLayer);
}
