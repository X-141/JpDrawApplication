#include "DrawArea.hpp"

#include <QPainter>
#include <QMouseEvent>
#include <QPixmap>
#include <QDir>
#include <QVector>
#include <QRegularExpression>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

#include "LineMethods.hpp"
#include "ProcessLayer.hpp"
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
      mKnnDictFilepath(resourcePath + "knnDictionary.txt")
{
    this->clear();

    mHardLayer.fill(); // set to white.
    this->setPixmap(mHardLayer);

    mVirtualLayerVector.reserve(32);

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
        LOG("ERROR: Tried to set pen width < 1");
}

int
DrawArea::compareLayer() {
    // we will get the entire draw area.
    cv::Mat hardLayerMat = qImageToCvMat(generateImage().copy(0,0, 384, 384));
    // for our image we do need to invert the colors from white-bg black-fg to white-fg black-bg
    cv::imwrite("PRE_TEST.png", hardLayerMat);
    cv::bitwise_not(hardLayerMat, hardLayerMat);
    cv::resize(hardLayerMat, hardLayerMat, cv::Size(32, 32));
    cv::threshold(hardLayerMat, hardLayerMat, 15, 255, cv::THRESH_BINARY);
    hardLayerMat.convertTo(hardLayerMat, CV_32F);
    cv::Mat flat_hardLayerMat = hardLayerMat.reshape(0, 1);

    cv::imwrite("POST_TEST.png", hardLayerMat);

    cv::Mat input, output;
    input.push_back(flat_hardLayerMat);

    input.convertTo(input, CV_32F);
    output.convertTo(output, CV_32F);
    qInfo() << flat_hardLayerMat.rows << " " << flat_hardLayerMat.cols;
    qInfo() << input.rows << " " << input.cols;
    mKnn->findNearest(input, 4, output);
    qInfo() << "Calculated Label: " << (    int)output.at<float>(0) << "w/ value " << 4;
//    for(int k = 1; k < 5; k++) {
//        mKnn->findNearest(input, k, output);
//        qInfo() << "Calculated Label: " << (    int)output.at<float>(0) << "w/ value " << k;
//    }

    return (int)output.at<float>(0);
}

QImage 
DrawArea::getComparisonImage(int index) {
//    return mComparisonImages.at(index);
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
DrawArea::loadComparisonImages() {
//    // Reference@: https://forum.qt.io/topic/64817/how-to-read-all-files-from-a-selected-directory-and-use-them-one-by-one/3
////    QString target_directory = "C:\\Users\\seanp\\source\\JpDrawApplication\\images";
//    QString target_directory = "../images/digits/";
//    QDir imageDir(target_directory);
//    QStringList images = imageDir.entryList(QStringList() << "*.png" << "*.PNG", QDir::Files);
//
//    QRegularExpression reg("(?<number>\\d).png");
//    for (auto png : images) {
//        auto match = reg.match(png);
//        qInfo() << match.captured("number");
//        mComparisonImagesDict.insert(match.captured("number").toInt(), QImage(imageDir.filePath(png)));
//    }

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
                qInfo() << match_png.captured("character") << match_txt.captured("number").toInt();
                mComparisonImagesDict.insert(match_txt.captured("number").toInt(),
                                             QImage(resourceDir.filePath(png)));
                images.removeOne(png);
                break;
            }
        }
    }
}