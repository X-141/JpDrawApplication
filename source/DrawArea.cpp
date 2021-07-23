#include "DrawArea.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QDir>
#include <QVector>
#include <QRegularExpression>

#include "LineMethods.hpp"
#include "ImageProcessMethods.hpp"

#include "opencv2/imgproc.hpp"

#include "Log.hpp"

std::string resourcePath = "../resource/";

DrawArea::DrawArea(QWidget* parent)
    : QLabel(parent),
      mCurrentlyDrawing(false),
      mHardLayer(this->size(), 0),
      mVirtualLayer(this->size(), 0),
      mId(1),
      mPenWidth(30),
      mKnn(cv::ml::KNearest::load(resourcePath + "kNN_ETL_Subset.opknn")),
      mKnnDictFilepath(resourcePath + "kNNDictionary.txt")
{
    this->clear();

    mHardLayer.fill(); // set to white.
    this->setPixmap(mHardLayer);

    mVirtualLayerVector.reserve(32);

    pLoadComparisonImages();
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
        LOG(level::warning, "DrawArea::setPenWidth()","Tried to set pen width < 1.");
}

int
DrawArea::compareLayer() {
    // we will get the entire draw area.
    cv::Mat hardLayerMat = ImageMethods::qImageToCvMat(generateImage().copy(0,0, 384, 384));
    // for our image we do need to invert the colors from white-bg black-fg to white-fg black-bg
    cv::bitwise_not(hardLayerMat, hardLayerMat);

    //return TechniqueMethods::ROITranslocation(mKnn, hardLayerMat, true);
    auto scaledImages = TechniqueMethods::ROIRescaling(hardLayerMat, true);
    return ImageMethods::passThroughKNNModel(mKnn, scaledImages);
}

QImage 
DrawArea::getComparisonImage(int index) {
    return mComparisonImagesDict[index];
}

void
DrawArea::undoLayer() {
    uint vectorSize = mVirtualLayerVector.size();
    if (vectorSize) {
        mVirtualLayerVector.remove(vectorSize - 1);
        updateDrawArea();
    }
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
DrawArea::pLoadComparisonImages() {
    QFile knnDictFile = QFile(mKnnDictFilepath.c_str());
    if(!knnDictFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QDir resourceDir = QDir(QString(resourcePath.c_str()));
    QStringList images = resourceDir.entryList(QStringList() << "*.png" << "*.PNG", QDir::Files);

    QTextStream in(&knnDictFile);
    QRegularExpression reg_txt("(?<character>\\w+),(?<number>\\d+)");
    QRegularExpression reg_png("(?<character>\\w+).png");

    while(!in.atEnd()) {
        QString line = in.readLine();
        auto match_txt = reg_txt.match(line);
        for(auto png : images) {
            auto match_png = reg_png.match(png);
            if (match_png.captured("character") == match_txt.captured("character")) {
                LOG(level::standard, "DrawArea::pLoadComparisonImages()",
                    QString(match_png.captured("character") + " " + match_txt.captured("number")));

                mComparisonImagesDict.insert(match_txt.captured("number").toInt(),
                                             QImage(resourceDir.filePath(png)));
                images.removeOne(png);
                break;
            }
        }
    }
}