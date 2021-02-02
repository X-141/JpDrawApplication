#include "mainwindow.hpp"
#include "ui_mainwindow.h"

#include "DrawArea.hpp"
#include "ProcessLayer.hpp"

#include <QMouseEvent>
#include <QPixmap>

// Test
#include <QListWidget>
#include <QPushButton>
#include <QDebug>
#include <QSlider>

#include "Log.hpp"
#include "opencv2/core.hpp"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mUi(new Ui::MainWindow),
    mDrawArea(nullptr)
{
    mUi->setupUi(this);
    mUi->centralwidget->resize(750, 750);

    mDrawArea = new DrawArea(mUi->centralwidget);
    mDrawArea->setObjectName("DrawArea");
    mDrawArea->setEnabled(true);
    mDrawArea->setCursor(QCursor(Qt::CrossCursor));
    mDrawArea->resizeDrawArea(QSize(400, 400));
    mUi->gridLayout->addWidget(mDrawArea, 0,0,1,1);

    mListWidget = new QListWidget(mUi->centralwidget);
    mListWidget->setObjectName("LayerListWidget");
    mListWidget->setEnabled(true);
    mUi->gridLayout->addWidget(mListWidget,1,0,1,1);

    mEnableButton = new QPushButton(mUi->centralwidget);
    mEnableButton->setObjectName("EnableLayerButton");
    mEnableButton->setText("Enable Layer");
    mEnableButton->setEnabled(true);
    mUi->gridLayout->addWidget(mEnableButton, 2,0,1,1);

    mDisableButton = new QPushButton(mUi->centralwidget);
    mDisableButton->setObjectName("DisableLayerButton");
    mDisableButton->setText("Disable Layer");
    mDisableButton->setEnabled(true);
    mUi->gridLayout->addWidget(mDisableButton, 3,0,1,1);

    mCompareButton = new QPushButton(mUi->centralwidget);
    mCompareButton->setObjectName("CompareLayerButton");
    mCompareButton->setText("Compare Layer");
    mCompareButton->setEnabled(true);
    mUi->gridLayout->addWidget(mCompareButton, 4, 0, 1, 1);

    mPenWidthSlider = new QSlider(Qt::Horizontal, mUi->centralwidget);
    mPenWidthSlider->setObjectName("PenWidthSlider");
    mPenWidthSlider->setEnabled(true);
    //mPenWidthSlider->setTickPosition(QSlider::TicksBelow);
    mPenWidthSlider->setMinimum(1);
    mPenWidthSlider->setMaximum(100);
    mUi->gridLayout->addWidget(mPenWidthSlider, 5, 0, 1, 1);

    // Whenever, we add a layer to drawArea, we send a signal to
    // update
    QObject::connect(mDrawArea, SIGNAL(layerUpdateHandle(void)),
                     this, SLOT(updateLayerList(void)));

    QObject::connect(mEnableButton, SIGNAL(clicked(bool)),
                     this, SLOT(enableLayer(bool)));

    QObject::connect(mDisableButton, SIGNAL(clicked(bool)),
                     this, SLOT(disableLayer(bool)));

    QObject::connect(mCompareButton, SIGNAL(clicked(bool)),
                     this, SLOT(compareLayer(bool)));

    QObject::connect(mPenWidthSlider, SIGNAL(valueChanged(int)),
                     this, SLOT(changePenWidth(int)));

    this->adjustSize();
    this->setWindowFlags(Qt::MSWindowsFixedSizeDialogHint);
}

MainWindow::~MainWindow()
{
    delete mUi;
    delete mDrawArea;
    delete mListWidget;
}

void
MainWindow::updateLayerList() {
    //qInfo() << "We received an update!";
    LOG("We received an update!");
    static uint lastAddedLayerID;
    const auto& layer_vector = mDrawArea->getListOfLayers();
    for(const auto& layer: layer_vector) {
        if(lastAddedLayerID < layer.getId()) {
            QListWidgetItem* item = new QListWidgetItem(mListWidget);
            if(layer.isEnabled())
                item->setForeground(Qt::blue);
            else
                item->setForeground(Qt::red);
            item->setText(QString::number(layer.getId()));
            lastAddedLayerID = layer.getId();
        }
    }
}

void
MainWindow::enableLayer(bool checked) {
    //qInfo() << "Add layer back!\n";
    LOG("Add layer back!\n");
    if(mListWidget) {
        auto selected_item = mListWidget->currentItem();
        auto& layer = mDrawArea->getLayerById(selected_item->text().toUInt());
        if(!layer.isEnabled()) {
            layer.setEnableStatus(true);
            mDrawArea->updateDrawArea();
            selected_item->setForeground(Qt::blue);
        }
    }
}

void
MainWindow::disableLayer(bool checked) {
    //qInfo() << "Removing layer!\n";
    LOG("Removing Layer!\n");
    if(mListWidget) {
        auto selected_item = mListWidget->currentItem();

        if (!selected_item) {
            LOG("listwidget current item is NULL.\n");
            return;
        }

        auto& layer = mDrawArea->getLayerById(selected_item->text().toUInt());

        if(layer.isEnabled()) {
            layer.setEnableStatus(false);
            mDrawArea->updateDrawArea();
            selected_item->setForeground(Qt::red);
        }
    }
}

#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"

void
MainWindow::compareLayer(bool checked) {
    LOG("Selected Compare layer button.\n");
    auto procLayer_SystemFile = ProcessLayer(DEBUG_targetFile);
    auto procLayer_HardLayer = ProcessLayer(mDrawArea->generateImage());

    // Reference https://stackoverflow.com/questions/9905093/how-to-check-whether-two-matrices-are-identical-in-opencv
    cv::imwrite("C:\\Users\\seanp\\source\\JpDrawApplication\\build\\grayscale_systemfile.png", procLayer_SystemFile.mPngMatrix);
    cv::imwrite("C:\\Users\\seanp\\source\\JpDrawApplication\\build\\grayscale_hardlayer.png", procLayer_HardLayer.mPngMatrix);

    cv::Mat diff;
    cv::compare(procLayer_SystemFile.mPngMatrix, procLayer_HardLayer.mPngMatrix, diff, cv::CMP_NE);
    int nz = cv::countNonZero(diff);
    LOG("DIFF: ");
    LOG(nz);
    LOG("\n");
}

void
MainWindow::changePenWidth(int value) {
    mDrawArea->setPenWidth(value);
}