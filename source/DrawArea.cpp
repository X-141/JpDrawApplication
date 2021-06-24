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
        LOG("ERROR: Tried to set pen width < 1");
}

int
DrawArea::compareLayer() {
    // we will get the entire draw area.
    cv::Mat hardLayerMat = qImageToCvMat(generateImage().copy(0,0, 384, 384));
    // for our image we do need to invert the colors from white-bg black-fg to white-fg black-bg
    cv::bitwise_not(hardLayerMat, hardLayerMat);

    cv::Rect roi = pObtainROI(hardLayerMat);
    std::vector<cv::Mat> translocatedImages = pTranslocateROI(hardLayerMat(roi).clone(), hardLayerMat.cols, hardLayerMat.rows);

    QMap<int, int> tally;

    int image_number = 0;
    int calculated_label = -1;
    for(auto& image : translocatedImages) {
        cv::imwrite("PRE_IMAGE_" + std::to_string(image_number) + ".png", image);
        cv::resize(image, image, cv::Size(32,32));
        cv::threshold(image, image, 15, 255, cv::THRESH_BINARY);
        image.convertTo(image, CV_32F);
        cv::imwrite("POST_IMAGE_" + std::to_string(image_number++) + ".png", image);
        cv::Mat flat_image = image.reshape(0,1);

        cv::Mat input, output;
        input.push_back(flat_image);
        input.convertTo(input, CV_32F);
        output.convertTo(output, CV_32F);

        mKnn->findNearest(input, 8, output);
        calculated_label = (int)output.at<float>(0);
        qInfo() << "Calculated Label: " << calculated_label;

        auto label = tally.find(calculated_label);

        if(label == tally.end())
            label = tally.insert(calculated_label, 0);

        label.value() = label.value() + 1;
    }

    int best_value = -1;
    int best_label = -1;
    for(auto begin = tally.begin(); begin != tally.end(); begin++) {
        if(begin.value() > best_value) {
            best_value = begin.value();
            best_label = begin.key();
        }
    }

    qInfo() << "Best label: " << best_label << " With value: " << best_value;

    return 0;
//    cv::resize(hardLayerMat, hardLayerMat, cv::Size(32, 32));
//    cv::threshold(hardLayerMat, hardLayerMat, 15, 255, cv::THRESH_BINARY);
//
//    hardLayerMat.convertTo(hardLayerMat, CV_32F);
//    cv::Mat flat_hardLayerMat = hardLayerMat.reshape(0, 1);
//
//    cv::imwrite("POST_TEST.png", hardLayerMat);
//
//    cv::Mat input, output;
//    input.push_back(flat_hardLayerMat);
//
//    input.convertTo(input, CV_32F);
//    output.convertTo(output, CV_32F);
//    qInfo() << flat_hardLayerMat.rows << " " << flat_hardLayerMat.cols;
//    qInfo() << input.rows << " " << input.cols;
//    mKnn->findNearest(input, 4, output);
//    qInfo() << "Calculated Label: " << (    int)output.at<float>(0) << "w/ value " << 4;
//
//    return (int)output.at<float>(0);
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
                qInfo() << match_png.captured("character") << match_txt.captured("number").toInt();
                mComparisonImagesDict.insert(match_txt.captured("number").toInt(),
                                             QImage(resourceDir.filePath(png)));
                images.removeOne(png);
                break;
            }
        }
    }
}

cv::Rect
DrawArea::pObtainROI(cv::Mat aMat) {
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

    return cv::Rect(min_y, min_x,max_y-min_y,max_x-min_x);
}

std::vector<cv::Mat>
DrawArea::pTranslocateROI(const cv::Mat& aROI, int aHeight, int aWidth) {

    uint16_t half_height = aHeight / 2;
    uint16_t half_width = aWidth / 2;

    uint16_t roi_height = aROI.rows;
    uint16_t roi_width = aROI.cols;
    uint16_t roi_half_height = roi_height / 2;
    uint16_t roi_half_width = roi_width / 2;

    uint16_t wriggle_height = std::floor(half_height - roi_half_height);
    uint16_t wriggle_width = std::floor(half_width - roi_half_width);
    // Margin value can be adjusted to reflect what "feels" like a good bounding box!
    uint16_t margin = std::floor(std::sqrt(aHeight) + std::sqrt(wriggle_height));

    uint16_t position_height = std::floor(half_height-roi_half_height);
    uint16_t position_width = std::floor(half_width-roi_half_width);

    constexpr uint16_t spots = 5;

    // We need to be sure when we are translocating the image
    // to the right or bottom that we are staying within bounds.
    // [spots][0] = height and [spots][1] = width
    int locations[spots][2] = {
            {position_height, position_width}, // Center
            {position_height - wriggle_height + margin, position_width}, // Top
            // {position_height + wriggle_height - margin, position_width}, // Bottom
            {position_height, position_width - wriggle_width + margin}, // Left
            // {position_height, position_width + wriggle_height - margin} // Right
    };

    uint16_t possible_position = position_height + wriggle_height - margin;
    // Check if translocating to the bottom exceeds height bound
    if (possible_position + roi_height >= aHeight) {
        // if so, go ahead and translocate as far down as possible
        // minus the margin to leave some space.
        locations[3][0] = aHeight - roi_height - margin;
        locations[3][1] = position_width;
    } else {
        locations[3][0] = possible_position;
        locations[3][1] = position_width;
    }

    possible_position = position_width + wriggle_width - margin;
    // Check if translocating to the right exceeds width bound.
    if (possible_position + roi_width >= aWidth) {
        // If so, go ahead and translocate as far right as possible
        // minus the margin to leave some space.
        locations[4][0] = position_height;
        locations[4][1] = aWidth - roi_width - margin;
    } else {
        locations[4][0] = position_height;
        locations[4][1] = possible_position;
    }

    std::vector<cv::Mat> translocatedImages;
    translocatedImages.resize(spots);

    for(uint8_t index = 0; index < spots; ++index) {
        // Create Black image at the index
        auto& img = translocatedImages.at(index);
        cv::Rect target_spot = cv::Rect(locations[index][1], locations[index][0],
                                        roi_width, roi_height);
        img = cv::Mat(aHeight, aWidth, aROI.type(), cv::Scalar(0,0,0));

        try {
            aROI.copyTo(img(target_spot));
        } catch (cv::Exception& exp) {
            qInfo() << exp.what();
            qInfo() << "At index: " << index;
            qInfo() << "Exception when attempting to paste ROI onto base image";
            qInfo() << "Base Position (height, width): " << position_height << " " << position_width;
            qInfo() << "(Wriggle, Margin): " << wriggle_height << " " << margin;
            qInfo() << "ROI Dimensions (height, width): " << roi_height << " " << roi_width;
            qInfo() << "Base Image Dimensions (height, width): " << aHeight << " " << aWidth;
            qInfo() << "Target Location (height, width): " << locations[index][0] << " " << locations[index][1];
            exit(-1);
        }
//        cv::imwrite("TEST_IMG_" + std::to_string(index) + ".png", img);
    }

    return translocatedImages;
}