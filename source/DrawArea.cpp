#include "DrawArea.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QDir>
#include <QVector>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"


#include "LineMethods.hpp"
#include "ProcessLayer.hpp"
#include "Log.hpp"

DrawArea::DrawArea(QWidget* parent)
    : QLabel(parent),
      mCurrentlyDrawing(false),
      mHardLayer(this->size(), 0),
      mVirtualLayer(this->size(), 0),
      mId(1),
      mPenWidth(30),
      mMax_x(-1), mMax_y(-1),
      mMin_x(INT32_MAX), mMin_y(INT32_MAX),
      mKnn(cv::ml::KNearest::load("knn_data_60000.opknn"))
{
    this->clear();

    mHardLayer.fill(); // set to white.
    this->setPixmap(mHardLayer);

    // Reserve some space in memory to avoid
    // heap allocating more space.
    mVirtualLayerVector.reserve(32);
    mComparisonImages.reserve(32);

    loadComparisonImages();
}

void
DrawArea::mousePressEvent(QMouseEvent* event) {
//    qDebug() << "Mouse press: " << event->pos() << "\n";
    //mVirtualLayer = DrawLayer(this->size(), mId++);
    mVirtualLayer.setId(mId++);
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

    // Reference@: https://stackoverflow.com/questions/35051482/qt-what-is-the-most-scalable-way-to-iterate-over-a-qimage
//    mMax_x = mMax_y = -1;
//    mMin_x = mMin_y = INT32_MAX;
//    int x, y;
//    QRgb* line = nullptr;
//    auto hardLayerImage = mHardLayer.toImage();
//    for (y = 0; y < hardLayerImage.height(); ++y) {
//        QRgb* line = (QRgb*) hardLayerImage.scanLine(y);
//        for (x = 0; x < hardLayerImage.width(); ++x) {
//            if (line[x] == QColor(Qt::black).rgb()) {
//                if (mMax_x < x) mMax_x = x;
//                if (mMax_y < y) mMax_y = y;
//                if (mMin_x > x) mMin_x = x;
//                if (mMin_y > y) mMin_y = y;
//            }
//        }
//    }
    //QImage image = QImage("C:\\Users\\seanp\\source\\JpDrawApplication\\images\\ho.png").scaled(mMax_x - mMin_x, mMax_y - mMin_y);
//    QPainter painter_2(&mHardLayer);
//    //painter_2.drawImage(QPoint(mMin_x, mMin_y), image);
//    painter_2.drawRect(mMin_x, mMin_y, mMax_x - mMin_x, mMax_y - mMin_y);
//    this->setPixmap(mHardLayer);
//    painter_2.end();
}

void
DrawArea::mouseMoveEvent(QMouseEvent *event) {
    if(mCurrentlyDrawing)
        pDrawPoint(event->pos());
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
    //mHardLayer = DrawLayer(this->size(), mVirtualLayer.getId());
    mHardLayer.fill();
    QPainter painter = QPainter(&mHardLayer);
    for(const auto& layers: mVirtualLayerVector)
        if(layers.isEnabled())
            painter.drawPixmap(0,0, layers);
    this->setPixmap(mHardLayer);
    painter.end();
}

QImage
DrawArea::generateImage() {
    return mHardLayer.toImage();
}

void
DrawArea::setPenWidth(int width) {
    if (width >= 1)
        mPenWidth = width;
    else
        LOG("ERROR: Tried to set pen width < 1");
}

int
DrawArea::compareLayer() {
    QVector<cv::Mat> processedCompareImages;
    processedCompareImages.reserve(mComparisonImages.size());

    for (const auto& preprocImages : mComparisonImages) {
        processedCompareImages.push_back(
            qImageToCvMat(preprocImages));
    }

    // First we need to scale the image to a 28 x 28, then convert it into an array
    // then we need to load in the .npz file that has our knn trained model.
    // for now print out the label.

    // we will get the entire draw area.
    cv::Mat hardLayerMat = qImageToCvMat(generateImage().copy(0,0, 400, 400));
    // for our image we do need to invert the colors from white-bg black-fg to white-fg black-bg
    cv::imwrite("PRE_TEST.png", hardLayerMat);
    cv::bitwise_not(hardLayerMat, hardLayerMat);
    cv::resize(hardLayerMat, hardLayerMat, cv::Size(28, 28));
    hardLayerMat.convertTo(hardLayerMat, CV_32F);
    cv::Mat flat_hardLayerMat = hardLayerMat.reshape(0, 1);

    cv::imwrite("POST_TEST.png", hardLayerMat);

    cv::Mat input, output;
    input.push_back(flat_hardLayerMat);

    input.convertTo(input, CV_32F);
    output.convertTo(output, CV_32F);
    qInfo() << flat_hardLayerMat.rows << " " << flat_hardLayerMat.cols;
    qInfo() << input.rows << " " << input.cols;
    mKnn->findNearest(input, 24, output);

    qInfo() << "Calculated Label: " << (int)output.at<float>(0);
    // We will return 1 for now. Since we are modifying the code
    // to use knn instead of the primitive method above.
    return 0;
}

QImage 
DrawArea::getComparisonImage(int index) {
    return mComparisonImages.at(index);
}

void
DrawArea::pDrawPoint(QPoint aPoint) {
    auto painter_hard = QPainter(&mHardLayer);
    auto painter_virt = QPainter(&mVirtualLayer);
    QPen pen = QPen(Qt::black, mPenWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter_hard.setPen(pen);
    painter_virt.setPen(pen);

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
    painter_hard.end();
    painter_virt.end();
    this->setPixmap(mHardLayer);
}

void
DrawArea::loadComparisonImages() {
    // Reference@: https://forum.qt.io/topic/64817/how-to-read-all-files-from-a-selected-directory-and-use-them-one-by-one/3
//    QString target_directory = "C:\\Users\\seanp\\source\\JpDrawApplication\\images";
    QString target_directory = "../images";
    QDir imageDir(target_directory);
    QStringList images = imageDir.entryList(QStringList() << "*.png" << "*.PNG", QDir::Files);
    for (auto png : images) {
        // qInfo() << imageDir.filePath(png);
        mComparisonImages.push_back(QImage(imageDir.filePath(png)));
    }
}